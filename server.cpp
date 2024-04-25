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

#define PORT 8080

// trim from start
static inline std::string &ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
            std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}

// trim from end
static inline std::string &rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(),
            std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    return s;
}

// trim from both ends
static inline std::string &trim(std::string &s) {
    return ltrim(rtrim(s));
}


struct c_client{
    std::string name;
    sockaddr_in addr;
    int sock;
    socklen_t tsize;

    c_client(int server_sock)
    {
        tsize = sizeof(addr);
        sock= accept(server_sock,
            (struct sockaddr *)&addr,
            (socklen_t*)&tsize );
    }
};

void messager(std::vector<c_client>& clients, int idx, bool& quit)
{


    int data;
    bool c_exit = false;
    std::string name = clients[idx].name;
    int nmlen = sizeof(name)/sizeof(char);

    while(!c_exit)
    {   char buffer[1024] = {};
        data = recv(clients[idx].sock, buffer, 1024,0);
        if(data <= 0)
        {
            c_exit = true;
        }
        else
        {

            std::string message = rtrim(ltrim(name))+"~ " + std::string(buffer) + "\n|";
            std::cout << message << "           length:" << trim(message).length() << std::endl;
            
            for (int i=0; i<clients.size(); i++)
            {
                if(i!=idx)
                {

                    send(clients[i].sock, message.c_str(), message.length()*sizeof(char) , 0 );
                }
            }

        }

    }


}

void acceptance(std::vector<c_client> &clients, bool &quit, int server_sock) //   ,   )
{
    //char* ehi = "Welcome";

    char buffer[1024] = {};
    std::string welcome = "Welcome Insert your name:";
    int numclient = 0;
    std::cout << "Server Started: "<< std::endl;
    std::vector<std::thread> mex;
    while (!quit)
    {
        sockaddr_in newclient;
        clients.push_back(c_client(server_sock));
        numclient+=1;
       
        if(clients[numclient-1].sock<0) quit = true;
        
        send(clients[numclient-1].sock, welcome.c_str(), sizeof(welcome) , 0 );
        recv(clients[numclient-1].sock, buffer, 1024,0);
        std::cout << "accepted: " << buffer << std::endl;
        clients[numclient-1].name = std::string(buffer);
       
        std::thread mexx(messager,std::ref(clients), numclient-1, std::ref(quit));
        mex.push_back(std::move(mexx));
        mex[mex.size()-1].detach();

    }

}




int main ( int argument, char const *argv[] )
{
    int server_sock;
    sockaddr_in server_address;
    int opted = 1;
    int server_address_length = sizeof(server_address);


    if (( server_sock = socket ( AF_INET, SOCK_STREAM, 0)) == 0)
    {
        std::cout<< "Opening of Socket Failed !" ;
        return -1;
    }
    if ( setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR,
                                &opted, sizeof ( opted )))
    {
         std::cout << "Can't set the socket" << std::endl;
         return -2;
    }

    // BIND SOCKET TO IP and PORT
    server_address.sin_family = AF_INET; //The server_address family for the transport server_address.
    server_address.sin_addr.s_addr = INADDR_ANY; //IPV4 server_address of the clients
    server_address.sin_port = htons( PORT ); //A transport protocol port number
    std::string address = "127.0.0.1";
    inet_pton(AF_INET, address.c_str() , &server_address.sin_addr);
    std::cout << (address) << std::endl;


    //BIND SOCKET
    if (bind(server_sock , (struct sockaddr *)&(server_address) ,sizeof(server_address))<0)
    {
        std::cout <<  "Binding of socket failed !" << std::endl;
        return -1;
    }

    //LISTEN
    if (listen ( server_sock, SOMAXCONN) < 0)
    {
        std::cout <<  "Can't listen from the server !" << std::endl;
        return -3;
    }

    std::vector<c_client> guests;
    bool quit = 0;
    std::thread accept_clients(acceptance,std::ref(guests),
                                    std::ref(quit),
                                    server_sock   );//, server_sock, , server_address_length);
    accept_clients.join();
    

    return 0;
    }
