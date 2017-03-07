//author: Witold Karaś
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h> 
#include <string.h>
#include <errno.h>


#include <iostream>

using namespace std;

int main(int argc, char* argv[])
{
    if (argc != 3) {
        cerr << "Wrong arguments number!" << endl;
        cerr << "Propper invocation: \n./client4 port_number ";
        exit(EXIT_FAILURE);
    }

    cerr << "IPv4/v6 client - PUS Lab02-5" << endl;
    cerr << "Witold Karaś, Łukasz Maj" << endl;
    cerr << "Port number:" << argv[2]<<endl;


    int portNumber=atoi(argv[2]);


    addrinfo hints;
    addrinfo *serverSocketAddressList;

    //AF_UNSPEC - any communications domain = IPv4 or IPv6
    //SOCK_STREAM - socket type = TCP
    //IPPROTO_TCP - TCP (see netinet/in.h)
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = 0;
    hints.ai_protocol = IPPROTO_TCP;


    if(getaddrinfo(argv[1], argv[2], &hints, &serverSocketAddressList)!=0)
    {
        cerr<<"Socket creation error!"<<endl;
        cerr<<"errno: "<<errno;
        exit(EXIT_FAILURE);
    }


    switch(serverSocketAddressList->ai_family)
    {
        case AF_INET:
            cout<<"IPv4 socket family"<<endl;
            break;
        case AF_INET6:
            cout<<"IPv6 socket family"<<endl;
            break;
        default:
            cerr<<"Unknown socket family"<<endl;
            exit(EXIT_FAILURE);
            break;
    }

    //creating socket corresponding to server IP version and other options
    int socketDescriptor=socket(serverSocketAddressList->ai_family,
                            serverSocketAddressList->ai_socktype,
                            serverSocketAddressList->ai_protocol);

    
    if(connect(socketDescriptor, serverSocketAddressList->ai_addr, serverSocketAddressList->ai_addrlen)==-1)
    {
        cerr<<"Socket connect error!"<<endl;
        perror("");
        cerr<<"errno: "<<errno;
        exit(EXIT_FAILURE);
    }
    else
    {
        char IPAddress[NI_MAXHOST];
        char portNumber[NI_MAXSERV];

        sockaddr_storage serverSocketAddressStorage;
        serverSocketAddressStorage.ss_family=serverSocketAddressList->ai_family;
        socklen_t serverSocketAddressStorageLength=sizeof(serverSocketAddressStorage);

        getsockname(socketDescriptor, (struct sockaddr*)&serverSocketAddressStorage, &serverSocketAddressStorageLength);
        getnameinfo((struct sockaddr*)&serverSocketAddressStorage,
                    serverSocketAddressStorageLength,
                    IPAddress,
                    NI_MAXHOST,
                    portNumber,
                    NI_MAXSERV,
                    NI_NUMERICHOST | NI_NUMERICSERV);

        cout<<"Server IP Address: "<<IPAddress<<" server port number: "<<portNumber;


        char message[512];
        memset(message, 0, 512);
        recv(socketDescriptor, message, sizeof(message), 0);
        putchar('\n');
        cout<< "Message from server: "<<message<<endl;
        putchar('\n');

        close(socketDescriptor);
        exit(EXIT_SUCCESS);
    }
}