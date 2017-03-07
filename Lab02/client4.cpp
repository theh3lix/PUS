#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h> 
#include <netinet/in.h>
#include <arpa/inet.h>
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

    cerr << "IPv4 client - PUS Lab02-3" << endl;
    cerr << "Witold Karaś, Łukasz Maj" << endl;
    cerr << "Port number:" << argv[2]<<endl;


    int portNumber=atoi(argv[2]);


    sockaddr_in serverSocketAddress;
    socklen_t serverSocketAddressLength;


    //creating new socket
    //AF_INET - communications domain = IPv4
    //SOCK_STREAM - socket type = TCP
    //IPPROTO_TCP - TCP (see netinet/in.h)
    int socketDescriptor=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if(socketDescriptor==-1)
    {
        cerr<<"Socket creation error!"<<endl;
        cerr<<"errno: "<<errno;
        exit(EXIT_FAILURE);
    }


    //preparing serverSocketAddress structure
    memset(&serverSocketAddress, 0, sizeof(serverSocketAddress));
    serverSocketAddress.sin_family=AF_INET;
    serverSocketAddress.sin_port=htons(portNumber);
    inet_pton(AF_INET, argv[1], &serverSocketAddress.sin_addr);

    serverSocketAddressLength=sizeof(serverSocketAddress);

    if(connect(socketDescriptor, (sockaddr *)&serverSocketAddress, serverSocketAddressLength)==-1)
    {
        cerr<<"Socket connect error!"<<endl;
        perror("");
        cerr<<"errno: "<<errno;
        exit(EXIT_FAILURE);
    }
    else
    {
        char message[512];
        memset(message, 0, 512);
        recv(socketDescriptor, message, sizeof(message), 0);
        cout<< "Message from server: "<<message<<endl;

        close(socketDescriptor);
        exit(EXIT_SUCCESS);
    }

}