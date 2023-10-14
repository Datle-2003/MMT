#ifndef SERVER_H
#define SERVER_H
#include <WS2tcpip.h>
#include <WinSock2.h>
#include <iostream>
#include <string>
#include <TlHelp32.h>
#include <conio.h>
#include <unordered_set>
#include <fstream>
#include <vector>
#include <filesystem>
#include <random>
#include <sstream>
#include <nlohmann/json.hpp>
#include <chrono>
#include <thread>
using json = nlohmann::json;




namespace fs = std::filesystem;
const int BUFFER_LENGTH = 10000;
const int IMAGE_LENGTH = 2048;
extern char Buffer[BUFFER_LENGTH];
const int TOKEN_LENGTH = 10;


class Server
{
private:
    SOCKET _ServerSocket;
    sockaddr_in _serverAddress;
    std::string username;
    std::string password;
    std::string token;
    std::string currentFolder;

public:
    Server();
    Server(int port, char* ipAddress);
    ~Server();

    void setPortNumber(int port);
    void setToken(std::string token);
    int initializeWinsock();
    int createSocket();
    int bindSocket();
    int listenForClient();

    SOCKET acceptConnect();

    void clearBuffer();
    void sendInvalidChoiceMessage(SOCKET& Client);
    void sendExitMessage(SOCKET& Client);

    int sendData(SOCKET clientSocket, std::string &);
    int sendImage(SOCKET clientSocket, const char*, int length);

    int receiveDataNoToken(SOCKET ClientSocket);
    int receiveData(SOCKET ClientSocket);

    void processOptions(SOCKET& Client);
    void processExitOption(SOCKET& Client);
    void processEnumerateAppsOption(SOCKET& Client);
    void processEnumerateProcessesOption(SOCKET& Client);
    void processCatchKeyPressOption(SOCKET& Client);
    void processTakeScreenshotOption(SOCKET& Client);
    void traversingDirectoryTree(SOCKET& Client);
    void viewActivityLog(SOCKET& Client);

    std::string enumerateApps(HKEY HKeyRoot, LPCSTR SubKey);
    std::string enumerateRunningProcesses();

    int catchKeyPress(SOCKET ClientSocket);

    std::string startApp(const char*);
    std::string stopProcess(const char*);

    void takeScreenshot(std::string filename);

    std::string changeDirectory(const std::string& newPath);
    std::string list();
    std::string changeName(const std::string& path, const std::string& new_name);
    std::string moveFile(const std::string& src, const std::string& dest);
    std::string changeToParentDirectory();
    std::string copyFile(const std::string& src, const std::string& dest);
    std::string deleteFile(const std::string& path);

    bool checkCredentials(SOCKET Client);
    std::string generateToken();

    std::string createAccount(SOCKET Client);
    
    bool addObjectToJsonFile(const std::string& username, const std::string& password);
    bool processLogin(SOCKET &Client);

    void addLogEntry(std::string& logEntry);
        
};

std::string getCurrentTimeAsString();

#endif