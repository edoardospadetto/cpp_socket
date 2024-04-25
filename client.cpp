#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <thread>
#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>

#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>


void client_recv(int serverSocket, bool &exit)
{
	while(!exit)
	{
		char buffer[1024] = {};
        recv(serverSocket, buffer, 1024,0);
        std::cout << buffer << std::endl;
	}

}


void client_send(int serverSocket, bool &exit)
{
	while(!exit)
	{
		std::string data;
        std::getline (std::cin,data);
        if(data == "!q") exit = true; 
        send(serverSocket, data.c_str(), data.length()*sizeof(char), 0);
        
	}

}

int main(){
	int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

	sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(8080);
	serverAddress.sin_addr.s_addr = INADDR_ANY;

	connect(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));

	bool exit = false; 

	std::thread sender(client_send,
							   serverSocket,
                               std::ref(exit));

 	std::thread recver(client_recv,
 							   serverSocket,
                               std::ref(exit));


 	recver.join();
 	sender.join();

}