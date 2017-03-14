//author: Witold Karaś
//based on examples from previous exercises and Linux documentation

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <sstream>

using namespace std;

std::vector<int>clients;
int listenSocketDescriptor;
fd_set watchedSockets;
sockaddr_in server_addr;

void checkForNewConnection()
{
    int new_socket;
    sockaddr_in client_addr;
    socklen_t client_addr_len;

    memset(&client_addr,0,sizeof(client_addr));

    if (FD_ISSET(listenSocketDescriptor, &watchedSockets)){

        if ((new_socket = accept(listenSocketDescriptor, (struct sockaddr *)&client_addr, &client_addr_len)) < 0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        cout<<"New <client "<<new_socket<<"> connected! IP: "<<inet_ntoa(client_addr.sin_addr)<<" port: "<<ntohs(client_addr.sin_port)<<endl;

        const char *message="Welcome to chat project\n";
        if (send(new_socket, message, strlen(message), 0) != strlen(message))
        {
            cerr<<"send() error! errno: "<<errno<<endl;
            exit(EXIT_FAILURE);
        }

        clients.push_back(new_socket);
        cout<<"Total clients: "<<clients.size()<<endl;
    }
}

void checkConnectedClients()
{
    sockaddr_in client_addr;
    memset(&client_addr, 0, sizeof(client_addr));
    socklen_t client_addr_len;
    char messageBuffer[256];
    memset(messageBuffer,0, sizeof(messageBuffer));

    for(auto element : clients)
    {
        if(FD_ISSET(element,&watchedSockets)) //sprawdza czy dany klient jest teraz gotowy (dane uzyskane przez select)
        {
            if(read(element, messageBuffer, sizeof(messageBuffer))==0) //host sie odlaczyl
            {
                getpeername(element, (struct sockaddr*)&client_addr, &client_addr_len);
                cout<<"<client "<<element<<"> disconnected! IP: "<<inet_ntoa(client_addr.sin_addr)<<" port: "<<ntohs(client_addr.sin_port)<<endl;

                close(element);
                FD_CLR(element, &watchedSockets);
                clients.erase(std::find(clients.begin(),clients.end(), element));
            }
            else
            {
                cout<<"Message received from socket: "<<element<<" Content:    "<<messageBuffer;

                //preparing and sending message to other clients
                std::stringstream message;
                message<<"<client "<<element<<">: "<<messageBuffer;

                for(auto el : clients)
                {
                    if(el!=element)
                        send(el, message.str().c_str(), message.str().length(),0);
                }
            }
        }
    }
}


int main(int argc, char* argv[])
{
    int maxSocketDescriptor;

    if (argc != 2)
    {
        fprintf(stderr, "Invocation: %s <PORT>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if ((listenSocketDescriptor = socket(PF_INET, SOCK_STREAM, 0)) == -1)
    {
        cerr<<"listen socket creating error! errno: "<<errno<<endl;
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family          =       AF_INET;
    server_addr.sin_addr.s_addr     =       htonl(INADDR_ANY);
    server_addr.sin_port            =       htons(atoi(argv[1]));


    if (bind(listenSocketDescriptor, (struct sockaddr*) &server_addr, sizeof(server_addr)) == -1)
    {
        cerr<<"bind() error! errno: "<<errno<<endl;
        exit(EXIT_FAILURE);
    }

    if (listen(listenSocketDescriptor, 10) == -1)
    {
        cerr<<"listen() error! errno: "<<errno<<endl;
        exit(EXIT_FAILURE);
    }
    cout<<"Listening for connections..."<<endl;



    while(true)
    {
        FD_ZERO(&watchedSockets);
        FD_SET(listenSocketDescriptor, &watchedSockets);
        maxSocketDescriptor = listenSocketDescriptor;

        for(auto element : clients)
        {
            FD_SET(element, &watchedSockets);
            if (element > maxSocketDescriptor)
                maxSocketDescriptor = element;
        }

        if(select(maxSocketDescriptor + 1, &watchedSockets, NULL, NULL, NULL)<0 && errno!=EINTR)
        {
            cerr<<"select() error! errno: "<<errno<<endl;
            exit(EXIT_FAILURE);
        }

        checkForNewConnection();
        checkConnectedClients();
    }
}
