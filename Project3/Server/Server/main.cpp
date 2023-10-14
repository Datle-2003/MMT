#include "Server.h"

int main()
{
    
	Server S;
	int port = 8080;
	//std::cout << "Set port for server: ";
	//std::cin >> port;
	S.setPortNumber(port);
	S.initializeWinsock();
	S.createSocket();
	S.bindSocket();
	S.listenForClient();
	SOCKET Client = S.acceptConnect();

	if (S.processLogin(Client)) {
		// generate token
		std::string token = S.generateToken();
		S.setToken(token);

		S.sendData(Client, token);
		S.processOptions(Client);

	}
	
	std::string log = "[ " + getCurrentTimeAsString() + " ] " + " Disconnected from the device";
	S.addLogEntry(log);
	return 0;
}