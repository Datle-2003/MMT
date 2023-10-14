#include "Server.h"


char Buffer[BUFFER_LENGTH];
const int DEFAULT_PORT = 8080;
int countImage = 1;

Server::Server()
{
    _serverAddress.sin_family = AF_INET;
    _serverAddress.sin_port = htons(DEFAULT_PORT);
    _serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    // set buffer to all zeros
    memset(Buffer, 0, BUFFER_LENGTH);
    _ServerSocket = INVALID_SOCKET;
    username = "";
    password = "";
    token = "";
    currentFolder = fs::current_path().string();
}

Server::Server(int Port, char* IPAddress)
{
    _serverAddress.sin_family = AF_INET;
    _serverAddress.sin_port = htons(Port);
    inet_pton(AF_INET, IPAddress, &_serverAddress.sin_addr);
    // set buffer to all zeros
    memset(Buffer, 0, BUFFER_LENGTH);
    _ServerSocket = INVALID_SOCKET;
}

Server::~Server()
{
    closesocket(_ServerSocket);
    WSACleanup();
}

void Server::setPortNumber(int port)
{
    _serverAddress.sin_port = htons(port);
}


void Server::setToken(std::string _token) {
    this->token = _token;
}

int Server::initializeWinsock()
{
    WSADATA wsadata;
    WORD wVersionRequested = MAKEWORD(2, 2);
    int result = WSAStartup(wVersionRequested, &wsadata);
    if (result != 0)
    {
        std::cerr << "WSAStartup failed: " << '\n';
        std::cerr << "The status: " << wsadata.szSystemStatus << '\n';
        return 1;
    }
    return 0;
}

int Server::createSocket()
{
    _ServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (_ServerSocket == INVALID_SOCKET)
    {
        std::cerr << "Error at socket: " << WSAGetLastError() << '\n';
        return 1;
    }
    return 0;
}

int Server::bindSocket()
{
    if (bind(_ServerSocket, (sockaddr*)&_serverAddress, sizeof(_serverAddress)) == SOCKET_ERROR)
    {
        std::cerr << "Error binding socket: " << WSAGetLastError() << '\n';
        return 1;
    }
    return 0;
}

int Server::listenForClient()
{
    if (listen(_ServerSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        std::cerr << "Error listening on socket: " << WSAGetLastError() << std::endl;
        return 1;
    }
    return 0;
}

SOCKET Server::acceptConnect()
{
    // Accept a new client connection
    sockaddr_in clientAddress;
    int clientAddressSize = sizeof(clientAddress);
    SOCKET clientSocket = accept(_ServerSocket, (sockaddr*)&clientAddress, &clientAddressSize);
    if (clientSocket == INVALID_SOCKET)
    {
        std::cerr << "Error accepting client connection: " << WSAGetLastError() << '\n';
        return INVALID_SOCKET;
    }

    // Return the handle to the client socket
    return clientSocket;
}

void Server::clearBuffer()
{
    memset(Buffer, 0, BUFFER_LENGTH);
}

void Server::sendExitMessage(SOCKET& Client)
{
    std::string exitMessage = "Exited\n";
    sendData(Client, exitMessage);
}

void Server::sendInvalidChoiceMessage(SOCKET& Client)
{
    std::string invalidChoiceMessage = "Invalid choice, please try again\n";

    sendData(Client, invalidChoiceMessage);
}

std::string getCurrentTimeAsString() {
    std::time_t currentTime = std::time(nullptr);
    char temp[80];
    struct tm timeInfo;

    localtime_s(&timeInfo, &currentTime);
    std::strftime(temp, sizeof(temp), "%Y-%m-%d %H:%M:%S", &timeInfo);

    return std::string(temp);
}

void Server::processOptions(SOCKET& Client)
{
    std::string menuOptions = "0. Exit\n1. List app\n2. List process\n3. Take a screenshot\n4. Catch key press\n5. Directory\n6. View Activity Log\nChoose your option: ";
    std::string emptyChoice = "Empty choice, please try again!\n";
 
   
    while (true)
    {
        sendData(Client, menuOptions);
        receiveData(Client);
        // option 0: exit
        if (strcmp(Buffer, "0") == 0)
        {
            processExitOption(Client);
            break;
        }

        // option 1: list app
        else if (strcmp(Buffer, "1") == 0) {
            processEnumerateAppsOption(Client);
          

        }
        // option 2: list process
        else if (strcmp(Buffer, "2") == 0) {
            processEnumerateProcessesOption(Client);
        }
        // option 3: take a screenshot
        else if (strcmp(Buffer, "3") == 0) {
            processTakeScreenshotOption(Client);
        }
        // option 4: catch key press
        else if (strcmp(Buffer, "4") == 0) {
            processCatchKeyPressOption(Client);
        }
        // option 5: directory tree traversal
        else if (strcmp(Buffer, "5") == 0) {
            traversingDirectoryTree(Client); 
        }
        else if (strcmp(Buffer, "6") == 0) {
            viewActivityLog(Client);
        }

        else {
            sendInvalidChoiceMessage(Client);
        }
    }
}

void Server::processExitOption(SOCKET& Client)
{

    std::string exitMessage = "Abort connection!\n";
    sendData(Client, exitMessage);
}

void Server::processEnumerateAppsOption(SOCKET& Client)
{
    std::string appMenu = "0.Exit\n1.List App\n2.Start App\nChoose your option: ";

    while (true)
    {
        sendData(Client, appMenu);
        receiveData(Client);
        if (strcmp(Buffer, "0") == 0)
        {
            sendExitMessage(Client);
            break;
        }
        else if (strcmp(Buffer, "1") == 0)
        {
            std::string appsName = enumerateApps(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall");
            std::string apps = appsName.c_str();
            sendData(Client, apps);
            std::string log = "[ " + getCurrentTimeAsString() + "] " + "List application";
            addLogEntry(log);
        }

        else if (strcmp(Buffer, "2") == 0)
        {
            std::string startProcessNameMessage = "Enter name of process: ";
            sendData(Client, startProcessNameMessage);
            receiveData(Client);
            std::string result = startApp(Buffer);
            std::string appName(Buffer);
            sendData(Client, result);
            std::string log = "[ " + getCurrentTimeAsString() + "] " + "Start application: " + appName;
            addLogEntry(log);
        }
        else
        {
            sendInvalidChoiceMessage(Client);
        }
    }
}

void Server::processEnumerateProcessesOption(SOCKET& Client)
{
    std::string processMenu = "0.Exit\n1.List process\n2.Stop processing\nChoose your option: ";
    while (true)
    {
        sendData(Client, processMenu);
        receiveData(Client);

        if (strcmp(Buffer, "0") == 0)
        {
            sendExitMessage(Client);
            break;
        }
        else if (strcmp(Buffer, "1") == 0)
        {
            std::string processesName = enumerateRunningProcesses();
            sendData(Client, processesName);
            std::string log = "[ " + getCurrentTimeAsString() + "] " + "List processes";
            addLogEntry(log);
        }

        else if (strcmp(Buffer, "2") == 0)
        {
            std::string stopProcessNameMessage = "Enter name of process: ";
            sendData(Client, stopProcessNameMessage);
            receiveData(Client);
            std::string result = stopProcess(Buffer);
            std::string processName(Buffer);
            sendData(Client, result);
            
            std::string log = "[ " + getCurrentTimeAsString() + "] " + "Stop process: " + processName;
            addLogEntry(log);
        }
        else
            sendInvalidChoiceMessage(Client);
    }
}

void Server::processCatchKeyPressOption(SOCKET& Client)
{
    std::string startCatchKeyPress = "Listening for key press events...\n";
    sendData(Client, startCatchKeyPress);
    catchKeyPress(Client);
}

void Server::processTakeScreenshotOption(SOCKET& Client)
{
    // take an image
    std::string filename = "image" + std::to_string(countImage);
    takeScreenshot(filename);
    // Open the .bmp file
    filename = filename + ".bmp";
    std::ifstream file(filename, std::ios::binary | std::ios::in);
    if (!file.is_open())
    {
        std::cerr << "Error opening BMP file: " << '\n';
        return;
    }
    // Get the file size
    file.seekg(0, std::ios::end);
    int fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> buffer(IMAGE_LENGTH);
    std::size_t bytesSent = 0;
    // send an image in packets of length IMAGE_LENGTH
    while (bytesSent < fileSize)
    {
        int bytesLeft = fileSize - bytesSent;
        int bytesToSend = min(bytesLeft, IMAGE_LENGTH);
        file.read(buffer.data(), bytesToSend);
        sendImage(Client, buffer.data(), bytesToSend);
        bytesSent += bytesToSend;
        if (bytesToSend < IMAGE_LENGTH)
            break;
    }
    std::chrono::milliseconds duration(3000);
    std::this_thread::sleep_for(duration);
    file.close();
    countImage++;

    std::string log = "[ " + getCurrentTimeAsString() + "] " + "Capture the screen image - Image saved: " + filename;
    addLogEntry(log);
}

void Server::traversingDirectoryTree(SOCKET& Client)
{
    fs::current_path(fs::path("/"));
    std::string helpMessage = "Commands:\n"
        "  ls                               List the contents of current directory\n"
        "  cd <path>                        Change the current directory\n"
        "  mv <filename/source> <dest>      Move a file or directory to a new location\n"
        "  cp <filename> <dest>             Copy a file to a new location\n"
        "  rm <filename>                    Remove a file\n"
        "  rn <old_name> <new_name>         Rename a file\n"
        "  ..                               Go up one directory level\n"
        "  help                             Show this help message\n"
        "  exit                             Quit the program\n\n";
    sendData(Client, helpMessage);

    while (true)
    {
        std::string currentPath = fs::current_path().string();
        std::string pathMessage = currentPath + '>';
        sendData(Client, pathMessage);
        receiveData(Client);
        std::string command(Buffer);
        std::string sendMessage = "";

        if (command == "exit")
            break;

        else if (command == "help")
        {
            sendMessage = (std::string)helpMessage;
            sendData(Client, sendMessage);
            std::string log = "[ " + getCurrentTimeAsString() + " ] " + "Traversing directory tree - help";
            addLogEntry(log);
        }
        else if (command == "ls")
        {
            sendMessage = list();
            if (sendMessage == "")
            {
                sendMessage = "\n";
            }
            sendData(Client, sendMessage);
            std::string log = "[ " + getCurrentTimeAsString() + " ] " + "Traversing directory tree - list files/folders in " + currentPath;
            addLogEntry(log);
        }

        else if (command.substr(0, 3) == "cd ")
        {
            std::string path = command.substr(3);
            std::string log = "[ " + getCurrentTimeAsString() + " ] " + "Traversing directory tree - Change directory from " + currentPath + " to ";
            sendMessage = changeDirectory(path);
            currentPath = fs::current_path().string();
            log += currentPath;
            addLogEntry(log);
        }
        else if (command.substr(0, 3) == "mv ")
        {
            std::string source = command.substr(3, command.find(' ', 3) - 3);
            std::string dest = command.substr(command.find(' ', 3) + 1) + "\\" + source;
            sendMessage = moveFile(source, dest);
            sendData(Client, sendMessage);
        }
        else if (command.substr(0, 3) == "cp ")
        {
            std::string source = command.substr(3, command.find(' ', 3) - 3);
            std::string dest = command.substr(command.find(' ', 3) + 1) + "\\" + source;
            sendMessage = copyFile(source, dest);
            sendData(Client, sendMessage);
        }
        else if (command.substr(0, 3) == "rm ")
        {
            std::string path = command.substr(3);
            sendMessage = deleteFile(path);
            sendData(Client, sendMessage);
        }
        else if (command.substr(0, 3) == "rn ")
        {
            std::string old_name = command.substr(3, command.find(' ', 3) - 3);
            std::string new_name = command.substr(command.find(' ', 3) + 1);
            sendMessage = changeName(old_name, new_name);
            sendData(Client, sendMessage);
        }
        else if (command == "..")
        {
            changeToParentDirectory();
            currentPath = fs::current_path().string();
        }
        else
        {
            sendMessage = "Error: invalid command\n";
            sendData(Client, sendMessage);
        }
    }
}

std::string Server::enumerateApps(HKEY hKeyRoot, LPCSTR subKey)
{
    // Khi liệt kê có một số app bị trùng nên lưu vào set
    std::unordered_set<std::string> s;
    HKEY hKey;
    if (RegOpenKeyExA(hKeyRoot, subKey, 0, KEY_READ, &hKey) == ERROR_SUCCESS)
    {
        DWORD index = 0;
        CHAR keyName[255];
        DWORD keyNameSize = sizeof(keyName);
        FILETIME lastWriteTime;

        while (RegEnumKeyExA(hKey, index++, keyName, &keyNameSize, NULL, NULL, NULL, &lastWriteTime) == ERROR_SUCCESS)
        {
            HKEY hSubKey;
            if (RegOpenKeyExA(hKey, keyName, 0, KEY_READ, &hSubKey) == ERROR_SUCCESS)
            {
                CHAR displayName[255];
                DWORD displayNameSize = sizeof(displayName);
                DWORD type;

                if (RegQueryValueExA(hSubKey, "DisplayName", NULL, &type, (LPBYTE)displayName, &displayNameSize) == ERROR_SUCCESS)
                    s.insert(displayName);

                RegCloseKey(hSubKey);
            }
            keyNameSize = sizeof(keyName);
        }

        RegCloseKey(hKey);
    }

    std::string appNames;
    for (const auto& app : s)
        appNames += app + '\n';
    return appNames;
}

std::string wcharToString(WCHAR* t)
{
    char ch[260];
    char DefChar = ' ';
    WideCharToMultiByte(CP_ACP, 0, t, -1, ch, 260, &DefChar, NULL);
    std::string ss(ch);
    return ss;
}

std::string Server::enumerateRunningProcesses()
{
    std::unordered_set<std::string> s;
    HANDLE hProcessSnap;
    PROCESSENTRY32 pe32;
    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (hProcessSnap == INVALID_HANDLE_VALUE)
    {
        std::cerr << "Failed to create snapshot" << std::endl;
        return "Error when enumarate running process";
    }

    pe32.dwSize = sizeof(PROCESSENTRY32);

    if (!Process32First(hProcessSnap, &pe32))
    {
        std::cerr << "Failed to get first process" << std::endl;
        CloseHandle(hProcessSnap);
        return "Error when enumarate running process";
    }

    do
    {
        std::string ss = wcharToString(pe32.szExeFile);
        s.insert(ss);

    } while (Process32Next(hProcessSnap, &pe32));

    CloseHandle(hProcessSnap);

    std::string processNames;
    for (const auto& process : s)
        processNames += process + '\n';
    return processNames;
}

int Server::catchKeyPress(SOCKET clientSocket)
{
    while (true)
    {
        receiveData(clientSocket);
        if (strcmp(Buffer, "stop") == 0)
            // Client wants to stop sending key presses
            break;
        else
        {
            std::string key(Buffer);
            std::string log = "[ " + getCurrentTimeAsString() + " ] " + "Key catcher - Press: " + key;
            addLogEntry(log);

            // Convert the received message back to the key code
            int keyCode = atoi(Buffer);
            char asciiCode = static_cast<char>(keyCode);
            BYTE vk = VkKeyScan(asciiCode); // Chuyển đổi ký tự ASCII thành mã Virtual Key
            if (vk != -1)
            {
                INPUT input = { 0 };
                input.type = INPUT_KEYBOARD;
                input.ki.wVk = static_cast<WORD>(vk);
                input.ki.dwFlags = 0;

                UINT result = SendInput(1, &input, sizeof(INPUT));
            }
        }
    }

    return 0;
}

std::string Server::startApp(const char* name)
{
    std::string processName(name);
    int wstrLength = MultiByteToWideChar(CP_UTF8, 0, processName.c_str(), -1, nullptr, 0);
    std::wstring wideProcessName(wstrLength, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, processName.c_str(), -1, &wideProcessName[0], wstrLength);

    STARTUPINFO si = {};
    PROCESS_INFORMATION pi = {};
    if (CreateProcess(NULL, &wideProcessName[0], NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
    {
        std::cout << "Process started with ID " << pi.dwProcessId << std::endl;
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
        return "Start process successfully\n";
    }
    return "Failed to start process: " + processName + '\n';
}

std::string Server::stopProcess(const char* ProcessName)
{
    std::string process(ProcessName);
    std::string command = "taskkill /F /IM " + process;
    int result = system(command.c_str());

    if (result == 1)
        return "Failed to stop " + process + '\n';

    return "Stop " + process + "sucessfully\n";
}

void Server::takeScreenshot(std::string fileName)
{
    fileName = fileName + ".bmp";
    HDC hdcScreen = GetDC(NULL);
    HDC hdcMem = CreateCompatibleDC(hdcScreen);

    int width = GetSystemMetrics(SM_CXSCREEN);
    int height = GetSystemMetrics(SM_CYSCREEN);

    HBITMAP hBitmap = CreateCompatibleBitmap(hdcScreen, width, height);
    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdcMem, hBitmap);

    BitBlt(hdcMem, 0, 0, width, height, hdcScreen, 0, 0, SRCCOPY);
    hBitmap = (HBITMAP)SelectObject(hdcMem, hOldBitmap);

    BITMAP bmp;
    GetObject(hBitmap, sizeof(BITMAP), &bmp);

    BITMAPFILEHEADER bmfHeader;
    BITMAPINFOHEADER bi;

    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = bmp.bmWidth;
    bi.biHeight = bmp.bmHeight;
    bi.biPlanes = 1;
    bi.biBitCount = 24;
    bi.biCompression = BI_RGB;
    bi.biSizeImage = 0;
    bi.biXPelsPerMeter = 0;
    bi.biYPelsPerMeter = 0;
    bi.biClrUsed = 0;
    bi.biClrImportant = 0;

    DWORD dwBmpSize = ((bmp.bmWidth * bi.biBitCount + 31) / 32) * 4 * bmp.bmHeight;

    bmfHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    bmfHeader.bfSize = dwBmpSize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    bmfHeader.bfType = 'B' + ('M' << 8);

    char* lpbitmap = new char[dwBmpSize];
    GetDIBits(hdcScreen, hBitmap, 0, height, lpbitmap, (BITMAPINFO*)&bi, DIB_RGB_COLORS);

    std::ofstream file(fileName, std::ios::out | std::ios::binary | std::ios::trunc);

    file.write((char*)&bmfHeader, sizeof(BITMAPFILEHEADER));
    file.write((char*)&bi, sizeof(BITMAPINFOHEADER));
    file.write(lpbitmap, dwBmpSize);

    file.close();

    delete[] lpbitmap;
    DeleteDC(hdcMem);
    ReleaseDC(NULL, hdcScreen);
}

std::string Server::changeDirectory(const std::string& new_path)
{
    fs::path current_path = fs::current_path();

    // Kiểm tra xem đường dẫn mới có tồn tại hay không
    if (!fs::exists(new_path))
    {
        return "Error: directory '" + new_path + "' does not exist\n";
    }

    // Thiết lập đường dẫn hiện tại
    current_path /= new_path;
    fs::current_path(current_path);
    return " ";
}

std::string Server::list()
{
    fs::path current_path = fs::current_path();
    std::string filename = "";
    for (const auto& entry : fs::directory_iterator(current_path))
        filename += entry.path().filename().string() + '\n';

    return filename;
}

std::string Server::changeName(const std::string& path, const std::string& new_name)
{
    // Kiểm tra xem tệp tồn tại hay không
    if (!fs::exists(path))
    {
        std::cerr << "Error: file '" << path << "' does not exist\n";
        return "Error: file '" + path + "' does not exist\n";
    }

    // Lấy đường dẫn của tệp
    fs::path file_path = fs::path(path);

    // Lấy đường dẫn của thư mục chứa tệp
    fs::path directory_path = file_path.parent_path();

    // Thiết lập đường dẫn mới cho tệp
    fs::path new_path = directory_path / new_name;

    // Thay đổi tên tệp
    fs::rename(file_path, new_path);
    std::string log = "[ " + getCurrentTimeAsString() + " ] " + "Traversing directory tree - rename " + path + " to " + new_name;
    addLogEntry(log);

    return "Renamed " + path + " to " + new_name + '\n';
}

std::string Server::moveFile(const std::string& source_path, const std::string& dest_path)
{
    // Kiểm tra xem tệp nguồn có tồn tại hay không
    if (!fs::exists(source_path))
        return "Error: source file '" + source_path + "' does not exist\n";

    // Kiểm tra xem tệp đích đã tồn tại chưa
    if (fs::exists(dest_path))
        return "Error: destination file '" + dest_path + "' already exists\n";

    // Di chuyển tệp từ đường dẫn nguồn tới đường dẫn đích
    try
    {
        fs::rename(source_path, dest_path);
    }
    catch (const fs::filesystem_error& e)
    {
        return "Error: " + (std::string)e.what() + '\n';
    }

    std::string log = "[ " + getCurrentTimeAsString() + " ] " + "Traversing directory tree - move " + source_path + " to " + dest_path;
    addLogEntry(log);
    return "File moved successfully\n";
}

std::string Server::changeToParentDirectory()
{
    // Lấy đường dẫn hiện tại
    fs::path current_path = fs::current_path();

    // Lấy đường dẫn thư mục cha
    fs::path parent_path = current_path.parent_path();

    // Thay đổi đường dẫn hiện tại đến thư mục cha
    fs::current_path(parent_path);
    std::string log = "[ " + getCurrentTimeAsString() + " ] " + "Traversing directory tree - change to parent directory from " + current_path.string();
    addLogEntry(log);
    return current_path.string();
}

std::string Server::copyFile(const std::string& source_path, const std::string& dest_path)
{
    // Kiểm tra xem tệp nguồn có tồn tại hay không
    if (!fs::exists(source_path))
        return "Error: source file '" + source_path + "' does not exist\n";

    // Kiểm tra xem tệp đích đã tồn tại chưa
    if (fs::exists(dest_path))
        return "Error: destination file '" + dest_path + "' already exists\n";

    // Sao chép tệp từ đường dẫn nguồn tới đường dẫn đích
    try
    {
        fs::copy(source_path, dest_path);
    }
    catch (const fs::filesystem_error& e)
    {
        return "Error: " + (std::string)e.what() + '\n';
    }
    std::string log = "[ " + getCurrentTimeAsString() + " ] " + "Traversing directory tree - copy file " + source_path + " to " + dest_path;
    addLogEntry(log);

    return "File copied successfully\n";
}

std::string Server::deleteFile(const std::string& path)
{
    // Kiểm tra xem tệp tồn tại hay không
    if (!fs::exists(path))
    {
        return "Error: file '" + path + "' does not exist\n";
    }

    // Xóa tệp
    fs::remove(path);
    std::string log = "[ " + getCurrentTimeAsString() + " ] " + "Traversing directory tree - delete file " + path;
    addLogEntry(log);
    return "Remove " + path + " sucessfully\n";
}


bool Server::checkCredentials(SOCKET Client) {

    std::string requireUsername = "Enter your username: ";
    sendData(Client, requireUsername);
    receiveDataNoToken(Client);
    const std::string username(Buffer);
    std::string requirePassword = "Enter your password: ";
    sendData(Client, requirePassword);
    receiveDataNoToken(Client);
    const std::string password(Buffer);
    
    std::ifstream inputFile("user.json");
    nlohmann::json jsonData;

    // Kiểm tra xem tệp tin JSON có trống hay không
    if (inputFile.peek() != std::ifstream::traits_type::eof()) {
        // Đọc nội dung tệp tin JSON hiện có
        inputFile >> jsonData;
    }

    inputFile.close();

    this->username = username;
    this->password = password;

    // Kiểm tra xem có đối tượng nào trong tệp tin JSON có username và password đã cho hay không
    for (const auto& object : jsonData) {
        if (object["username"] == username && object["password"] == password) {
            std::string log = "[ " + getCurrentTimeAsString() + "] " + "Connected to device";
            addLogEntry(log);
            return true;
        }
    }

    return false;
}


bool isObjectExists(const std::string& username) {
    std::string filename = "user.json";
    std::ifstream inputFile(filename);
    nlohmann::json jsonData;

    // Kiểm tra xem tệp tin JSON có trống hay không
    if (inputFile.peek() != std::ifstream::traits_type::eof()) {
        // Đọc nội dung tệp tin JSON hiện có
        inputFile >> jsonData;
    }

    inputFile.close();

    // Kiểm tra xem có đối tượng nào trong tệp tin JSON có username và password đã cho hay không
    for (const auto& object : jsonData) {
        if (object["username"] == username) {
            return true;
        }
    }

    return false;
}

bool Server::addObjectToJsonFile(const std::string& username, const std::string& password) {
    std::string filename = "user.json";
    nlohmann::json newObject;
    newObject["username"] = username;
    newObject["password"] = password;
    newObject["log"] = nlohmann::json::array();

    std::ifstream inputFile(filename);
    nlohmann::json jsonData;

    // Kiểm tra xem tệp tin JSON có trống hay không
    if (inputFile.peek() != std::ifstream::traits_type::eof()) {
        // Đọc nội dung tệp tin JSON hiện có
        inputFile >> jsonData;
    }

    inputFile.close();
    bool exist = isObjectExists(username);
    if (exist == true) {
        return false;
    }
  

    // Thêm đối tượng mới vào tệp tin JSON
    jsonData.push_back(newObject);

    std::ofstream outputFile(filename);
    if (outputFile.is_open()) {
        // Ghi nội dung JSON mới vào tệp tin
        outputFile << jsonData.dump(4); // Thụt đầu dòng bằng 4 khoảng trắng
        outputFile.close();
        std::cout << "Đối tượng đã được thêm vào tệp tin JSON thành công." << std::endl;
    }
    else {
        std::cout << "Không thể mở tệp tin JSON." << std::endl;
    }
    return true;
}



std::string Server::createAccount(SOCKET Client) {
    std::string requireUsername = "Enter username: ";
    sendData(Client, requireUsername);
    receiveDataNoToken(Client);
    const std::string username(Buffer);
    std::string requirePassword = "Enter password: ";
    sendData(Client, requirePassword);
    receiveDataNoToken(Client);
    const std::string password(Buffer);

    bool added = addObjectToJsonFile(username, password);
    if (added == false) {
        return "Username already exists\n";
    }
    return "Create account successfully\n";
}



std::string Server::generateToken() {
    std::string tokenChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    std::string token;

    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<int> distribution(0, tokenChars.length() - 1);

    for (int i = 0; i < TOKEN_LENGTH; ++i) {
        int index = distribution(generator);
        token += tokenChars[index];
    }
    return token;
}


int Server::sendData(SOCKET ClientSocket, std::string &data) {
    clearBuffer();
    int bytesSent = send(ClientSocket, data.c_str(), data.length(), 0);
    if (bytesSent == SOCKET_ERROR)
    {
        std::cerr << "Error sending data to client: " << WSAGetLastError() << '\n';
        std::string log = "[ " + getCurrentTimeAsString() + " ] " + " Disconnected from the device";
        addLogEntry(log);
        exit(1);
        return SOCKET_ERROR;
    }
    return bytesSent;

}

int Server::sendImage(SOCKET clientSocket, const char* data, int length) {
    int bytesSent = send(clientSocket, data, length, 0);
    if (bytesSent == SOCKET_ERROR)
    {
        std::cerr << "Error sending data to client: " << WSAGetLastError() << '\n';
        std::string log = "[ " + getCurrentTimeAsString() + " ] " + " Disconnected from the device";
        addLogEntry(log);
        exit(1);
        return SOCKET_ERROR;
    }
    return bytesSent;
}


int Server::receiveData(SOCKET clientSocket){
    clearBuffer();
    // Receive the message from the client
    int bytesReceived = recv(clientSocket, Buffer, BUFFER_LENGTH, 0);

    if (bytesReceived == SOCKET_ERROR)
    {
        std::cerr << "Error receiving data from client: " << WSAGetLastError() << '\n';
        std::string log = "[ " + getCurrentTimeAsString() + " ] " + " Disconnected from the device";
        addLogEntry(log);
        exit(1);
        return 1;
    }

    // Process the received message
    std::string receivedMessage(Buffer, bytesReceived);

    // Extract the token from the received message
    std::size_t tokenPosition = receivedMessage.find(token);
    if (tokenPosition == std::string::npos)
    {
        std::string invalidToken = "Invalid token. Connection terminated.\n";
        sendData(clientSocket, invalidToken);
        std::string log = "[ " + getCurrentTimeAsString() + " ] " + " Disconnected from the device";
        addLogEntry(log);
        exit(1);
        return 1;
    }
    std::string message = receivedMessage.substr(0, tokenPosition);
    std::copy(message.begin(), message.end(), Buffer);

    // Add a null terminator at the end of the copied data
    Buffer[message.size()] = '\0';
    return 0;
}

int Server::receiveDataNoToken(SOCKET clientSocket)
{
    clearBuffer();
    // Receive the message from the client
    int bytesReceived = recv(clientSocket, Buffer, BUFFER_LENGTH, 0);

    if (bytesReceived == SOCKET_ERROR)
    {
        std::cerr << "Error receiving data from client: " << WSAGetLastError() << '\n';
        std::string log = "[ " + getCurrentTimeAsString() + " ] " + " Disconnected from the device";
        addLogEntry(log);
        exit(1);
        return 1;
    }
    return 0;
}


bool Server::processLogin(SOCKET& Client) {
    std::string menu = "0. Exit\n1. Create account\n2. Login\nChoose your option: ";
    while (1) {
        sendData(Client, menu);
       receiveDataNoToken(Client);
        if (strcmp(Buffer, "0") == 0) {
            std::string exit = "Exited\n";
            sendData(Client, exit);
            return 0;
        }
        else if (strcmp(Buffer, "1") == 0) {
            std::string createMess = createAccount(Client);
            sendData(Client, createMess);
        }
        else if (strcmp(Buffer, "2") == 0) {
            bool check = checkCredentials(Client);
            if (check) {
                std::string correct = "Logged in successfully\n";
                sendData(Client, correct);
                return 1;
            }
            else {
                std::string incorrect = "Username or password is incorrect\n";
                sendData(Client, incorrect);
            }
        }
        else {
            std::string invalid = "Invalid choice, please try again.\n";
            sendData(Client, invalid);
        }
    }
}

void Server::addLogEntry(std::string& logEntry) {
    std::string filePath = this->currentFolder + "\\user.json";
    std::ifstream inputFile(filePath);
    nlohmann::json jsonData;
    // Check if the JSON file is empty
    if (inputFile.peek() != std::ifstream::traits_type::eof()) {
        // Read the existing JSON content
        inputFile >> jsonData;
    }
    inputFile.close();

    // Search for the object with the matching username and password
    for (auto& object : jsonData) {
        if (object["username"] == this->username && object["password"] == this->password) {
                        // Add the log entry to the "log" array of the matched object
            object["log"].push_back(logEntry);

            std::ofstream outputFile(filePath);
            if (outputFile.is_open()) {
                // Write the updated JSON content to the file
                outputFile << jsonData.dump(4); // Indent with 4 spaces
                outputFile.close();
               // std::cout << "Log entry has been added successfully." << std::endl;
            }
            else {
                std::cout << "Unable to open the JSON file." << std::endl;
            }

            return;
        }
    }

    std::cout << "No object found with the specified username and password." << std::endl;
}

void Server::viewActivityLog(SOCKET& Client) {
    std::string filePath = this->currentFolder + "\\user.json";
    std::ifstream inputFile(filePath);
    nlohmann::json jsonData;
    // Check if the JSON file is empty
    if (inputFile.peek() != std::ifstream::traits_type::eof()) {
        // Read the existing JSON content
        inputFile >> jsonData;
    }
    inputFile.close();

    // Khai báo biến log bên ngoài vòng lặp
    std::string log = "";

    for (auto& object : jsonData) {
        if (object["username"] == this->username && object["password"] == this->password) {
            // Truy cập trực tiếp vào mảng log của đối tượng
            std::vector<std::string> logArray = object["log"];

            int num_lines = logArray.size();
            int start_line = (num_lines > 50) ? (num_lines - 50) : 0;

            // Lưu các dòng tối đa n vào biến string
            log = "";
            for (int i = start_line; i < num_lines; i++) {
                log += logArray[i] + "\n";
            }
            // Gửi biến log tới hàm sendData
            sendData(Client, log);
            // Thoát khỏi vòng lặp vì đã tìm thấy đối tượng phù hợp
            return;
        }
    }
    // Nếu không tìm thấy đối tượng phù hợp, gửi chuỗi rỗng tới hàm sendData
    sendData(Client, log);
}