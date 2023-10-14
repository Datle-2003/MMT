#include "Client.h"

int main() {
    Client C;
    int port = 8080;
   // std::string ipAddress = "";
   // std::cout << "Enter ip address of server(if server is the same device as client, press S): ";
   // std::cin >> ipAddress;
   // std::cout << "Enter port: ";
    //std::cin >> port;
    //if (ipAddress != "S") {
      //  C.setServerAddress(port, ipAddress.c_str());
    //}
    //std::cin.ignore();
    C.initializeWinsock();
    C.createSocket();
    C.connectToServer();


    if (C.userAuthentication()) {
        std::string token = C.receiveToken(); // receive token
        C.setToken(token);
        while (true) {
            C.receiveData(); // menu
            C.sendDataWithToken();
            if (strcmp(Buffer, "0") == 0) {
                C.receiveData();
                break;
            }
            else if (strcmp(Buffer, "1") == 0) {

                while (true) {
                    C.receiveData(); // menu
                    C.sendDataWithToken();
                    if (strcmp(Buffer, "0") == 0) {
                        C.receiveData();
                        break;
                    }
                    else if (strcmp(Buffer, "1") == 0) {
                        C.receiveData();// data
                    }
                    else if (strcmp(Buffer, "2") == 0) {
                        C.receiveData(); // nameMessage
                        C.sendDataWithToken();
                        C.receiveData();// empty or not
                    }
                    else {
                        // invalid
                        C.receiveData();
                    }
                }
            }

            else if (strcmp(Buffer, "2") == 0) {
                while (true) {
                    C.receiveData(); // menu
                    C.sendDataWithToken();
                    if (strcmp(Buffer, "0") == 0) {
                        C.receiveData();
                        break;
                    }
                    else if (strcmp(Buffer, "1") == 0) {
                        C.receiveData();// data
                    }
                    else if (strcmp(Buffer, "2") == 0) {
                        C.receiveData(); // nameMessage
                        C.sendDataWithToken();
                        C.receiveData();// empty or not
                    }
                    else {
                        // invalid
                        C.receiveData();
                    }
                }
            }

            else if (strcmp(Buffer, "3") == 0) {
                C.receiveImage();
                
            }

            else if (strcmp(Buffer, "4") == 0) {
                C.receiveData();
                C.catchKeyInput();
            }

            else if (strcmp(Buffer, "5") == 0) {
                C.receiveData(); // menu
                while (strcmp(Buffer, "exit") != 0) {
                    if ((strstr(Buffer, "cd") != NULL) || (strstr(Buffer, "..") != NULL)) {
                        C.receiveData();
                        C.sendDataWithToken();
                    }
                    else {
                        C.receiveData();
                        C.receiveData();
                        C.sendDataWithToken();
                    }
                }
            }
            else if (strcmp(Buffer, "6") == 0) {
                C.receiveData();
            }
            else {
                // invalid or message
                C.receiveData();
            }
        }
    }
    return 0;
}
