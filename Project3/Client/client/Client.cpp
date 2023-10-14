#include "Client.h"

char Buffer[BUFFER_LENGTH];
char Img_Buffer[IMAGE_LENGTH];
const int DEFAULT_PORT = 8080;
int countImage = 1;
Client::Client()
{
    _ClientSocket = INVALID_SOCKET;
    _serverAddr.sin_family = AF_INET;
    _serverAddr.sin_port = htons(DEFAULT_PORT);
    inet_pton(AF_INET, "127.0.0.1", &_serverAddr.sin_addr);
    memset(Buffer, 0, BUFFER_LENGTH);
    C_privateKey = "";
    S_publicKey = "";
    token = "";
}


void Client::setToken(std::string &token) {
    this->token = token;
}

Client::Client(int Port, const char* IPAddress)
{
    _ClientSocket = INVALID_SOCKET;
    _serverAddr.sin_family = AF_INET;
    _serverAddr.sin_port = htons(Port);
    inet_pton(AF_INET, IPAddress, &_serverAddr.sin_addr);
    memset(Buffer, 0, BUFFER_LENGTH);
}

Client::~Client()
{
    closesocket(_ClientSocket);
    WSACleanup();
}

void Client::setServerAddress(int port, const char* IPAddress)
{
    _serverAddr.sin_port = htons(port);
    inet_pton(AF_INET, IPAddress, &_serverAddr.sin_addr);
}

int Client::initializeWinsock()
{
    WSADATA wsaData; // wsaData: lưu trữ thông tin về phiên bản Winsock được sử dụng
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    /* WSAStartup(wVersionRequested, lpWSAData)
    wVersionRequested: Phiên bản Winsock, ở đây là phiên bản 2.2
    lpWSAData: Lưu trữ thông tin về phiên bản
    */
    if (result != 0)
    {
        printf("WSAStartup failed: %d\n", result);
        return 1;
    }
    return 0;
}

int Client::createSocket()
{
    _ClientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    /*
    socket(int domain, int type, int protocol)
    AF_INET: IPv4
    SOCK_STREAM: truyền dữ liệu tin cậy, trình tự và giao tiếp 2 phía
    */
    if (_ClientSocket == INVALID_SOCKET)
    {
        printf("Failed to create socket: %d\n", WSAGetLastError());
        return 1;
    }
    return 0;
}

int Client::connectToServer()
{
    // connect to server
    if (connect(_ClientSocket, (sockaddr*)&_serverAddr, sizeof(_serverAddr)) == SOCKET_ERROR)
    {
        printf("Failed to connect: %d\n", WSAGetLastError());
        exit(0);
        return 1;
    }
    return 0;
}

void Client::clearBuffer()
{
    memset(Buffer, 0, BUFFER_LENGTH);
}


int Client::receiveImage()
{
    memset(Img_Buffer, 0, IMAGE_LENGTH);
    std::string filename = "image" + std::to_string(countImage) + ".bmp";
    std::ofstream file(filename, std::ios::binary | std::ios::out);
    if (!file.is_open())
    {
        std::cerr << "Error creating BMP file: " << '\n';
        return 1;
    }
    int result = IMAGE_LENGTH;
    while (result == IMAGE_LENGTH)
    {
        
        result = recv(_ClientSocket, Img_Buffer, IMAGE_LENGTH, 0);
        if (result == SOCKET_ERROR)
        {
            std::cerr << "Error receiving data from server: " << WSAGetLastError() << '\n';
            return SOCKET_ERROR;
        }
        if (result == 0)
        {
            break; // End of data received
        }
        file.write(Img_Buffer, result);
    }
    file.close();
    std::cout << filename + " was received\n";
    countImage++;
    return 0;
}

void Client::catchKeyInput()
{
    while (true)
    {
        int ch = _getch();

        // Exit if ESCAPE key is pressed
        if (ch == VK_ESCAPE)
        {
            send(_ClientSocket, "stop", 4, 0);
            break;
        }
        std::string str_ch = std::to_string(ch);
        const char* char_num = str_ch.c_str();
        int bytes_sent = send(_ClientSocket, char_num, str_ch.length(), 0);
        if (bytes_sent < 0)
        {
            std::cerr << "Error sending data" << std::endl;
            break;
        }
    }
}

int Client::userAuthentication() {

    while (1) {
        receiveData(); // menu
        sendData();
        if (strcmp(Buffer, "0") == 0) {
            return 0;
        }
        else if (strcmp(Buffer, "1") == 0) {
            receiveData(); // username
            sendData();
            receiveData(); // password
            sendData();
            receiveData(); // result
        }
        else if (strcmp(Buffer, "2") == 0) {
            receiveData(); // username
            sendData();
            receiveData(); // password
            sendData();
            receiveData(); // result
            if (strcmp(Buffer, "Logged in successfully\n") == 0)
                return 1;
        }
        else {
            receiveData();
        }
        
    }
    return 0;
}


std::string Client::receiveToken() {
    clearBuffer();
    int recvResult = recv(_ClientSocket, Buffer, BUFFER_LENGTH, 0);
    if (recvResult == SOCKET_ERROR)
    {
        printf("Failed to receive: %d\n", WSAGetLastError());
        return "";
    }

    if (recvResult == 0)
    {
        printf("Connection closed\n");
        return "";
    }
    std::string res(Buffer);
    return res;
}


int Client::sendDataWithToken()
{
    clearBuffer();
    while (true)
    {
        std::cin.getline(Buffer, BUFFER_LENGTH);
        std::string message(Buffer);
        if (strcmp(Buffer, "") != 0)
        {
            // Append the token to the message
            message += token;

            int sendResult = send(_ClientSocket, message.c_str(), message.length(), 0);
            if (sendResult == SOCKET_ERROR)
            {
                printf("Failed to send: %d\n", WSAGetLastError());
                return 1;
            }
            break;
        }
        else
        {
            std::cout << "Empty, please try again.\n";
        }
    }

    return 0;
}


int Client::sendData() {
    clearBuffer();
    while (true)
    {
       // std::cout << "Enter a message: ";
        std::cin.getline(Buffer, BUFFER_LENGTH);

        if (strcmp(Buffer, "") != 0)
        {
            int sendResult = send(_ClientSocket, Buffer, strlen(Buffer), 0);
            if (sendResult == SOCKET_ERROR)
            {
                printf("Failed to send: %d\n", WSAGetLastError());
                return 1;
            }
            break;
        }
        else
        {
            std::cout << "Empty, please try again.\n";
        }
    }

    return 0;
}
int Client::receiveData() {
    clearBuffer();
    int recvResult = recv(_ClientSocket, Buffer, BUFFER_LENGTH, 0);
    if (recvResult <= 0) {
        std::cout << "Error: no menu\n";
        return 0;
    }
    std::cout << Buffer;
    return 1;
}

