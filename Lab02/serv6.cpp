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
	if(argc!=2)
	{
		cerr<<"Wrong arguments number!"<<endl;
		cerr<<"Propper invocation: \n./serv6 port_number "<<endl;
        exit(EXIT_FAILURE);
	}
	
	cerr<<"IPv6 Server - PUS Lab02-3"<<endl;
	cerr<<"Witold Karaś, Łukasz Maj"<<endl;
	cerr<<"Port number:"<<argv[1]<<endl;
	
	
	int portNumber=atoi(argv[1]);
	
	sockaddr_in6 serverSocketAddress;
	
	
	//creating new socket
	//AF_INET6 - communications domain = IPv6
	//SOCK_STREAM - socket type = TCP
	//IPPROTO_TCP - TCP (see netinet/in.h)
	int socketDescriptor=socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP); 

	if(socketDescriptor==-1)
	{
		cerr<<"Socket creation error!"<<endl;
		cerr<<"errno: "<<errno;
		exit(EXIT_FAILURE);
	}
	
	
	//preparing serverSocketAddress structure
	memset(&serverSocketAddress, 0, sizeof(serverSocketAddress));
    serverSocketAddress.sin6_family=AF_INET6;     
    serverSocketAddress.sin6_port=htons(portNumber);
    serverSocketAddress.sin6_addr=in6addr_any;
    
    if (bind(socketDescriptor, (struct sockaddr*) &serverSocketAddress, sizeof(serverSocketAddress)) ==-1)
    {
		cerr<<"Socket binding error!"<<endl;
		cerr<<"errno: "<<errno;
		exit(EXIT_FAILURE);
    }
    
	const int queueLength=20;
	
	//mark socket passive (prepare for receive)
	if(listen(socketDescriptor, queueLength)==-1)
	{
		cerr<<"Socket listen error!"<<endl;
		cerr<<"errno: "<<errno;
		exit(EXIT_FAILURE);
	}

	while(true)
	{
		int clientSocketDescriptor;
		sockaddr_in6 clientSocketAddress;
		socklen_t clientSocketAddressLength = sizeof(clientSocketAddress);

		if(clientSocketDescriptor=accept(socketDescriptor, (sockaddr *)&clientSocketAddress, &clientSocketAddressLength )==-1)
		{
			cerr<<"Socket accept error!"<<endl;
			cerr<<"errno: "<<errno;
			exit(EXIT_FAILURE);
		}
		else
		{
			char* ipAddressCString;
			inet_ntop(AF_INET6, &clientSocketAddress.sin6_addr, ipAddressCString, INET6_ADDRSTRLEN );
			cerr<<"Client IP address: "<<ipAddressCString<<" Port number: "<< ntohs(clientSocketAddress.sin6_port)<<endl;


			if(IN6_IS_ADDR_V4MAPPED(&clientSocketAddress.sin6_addr))
			{
				cout<<"Client: IP ver. 4"<<endl;
			}
			else
			{
				cout<<"Client: IP ver. 6"<<endl;
			}
			cout<<flush;

			char* mess= (char *) "Laboratorium PUS";
			if(write(clientSocketDescriptor, mess, strlen(mess))<=0)
            {
                cerr<<"Socket write error!"<<endl;
                cerr<<"errno: "<<errno;
                exit(EXIT_FAILURE);
            }
			close(clientSocketDescriptor);
		}
	}
}



