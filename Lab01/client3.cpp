//Autor: Witold Karaś
//na podstawie client2.c

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h> /* socket() */
#include <netinet/in.h> /* struct sockaddr_in */
#include <arpa/inet.h>  /* inet_pton() */
#include <unistd.h>     /* close() */
#include <string.h>
#include <errno.h>


#include <iostream>
#include <cstdlib>

using namespace std;

int main(int argc, char** argv) {

    int             sockfd;                 /* Desktryptor gniazda. */
    int             retval;                 /* Wartosc zwracana przez funkcje. */
    struct          sockaddr_in remote_addr;/* Gniazdowa struktura adresowa. */
    socklen_t       addr_len;               /* Rozmiar struktury w bajtach. */
    char            buff[256];              /* Bufor dla funkcji recvfrom(). */


    if (argc != 3) {
        fprintf(
            stderr,
            "Invocation: %s <IPv4 ADDRESS> <PORT>\n", argv[0]
        );
        exit(EXIT_FAILURE);
    }

    /* Utworzenie gniazda dla protokolu UDP: */
    sockfd = socket(PF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        perror("socket()");
        exit(EXIT_FAILURE);
    }

    /* Wyzerowanie struktury adresowej dla adresu zdalnego (serwera): */
    memset(&remote_addr, 0, sizeof(remote_addr));
    /* Domena komunikacyjna (rodzina protokolow): */
    remote_addr.sin_family = AF_INET;

    /* Konwersja adresu IP z postaci kropkowo-dziesietnej: */
    retval = inet_pton(AF_INET, argv[1], &remote_addr.sin_addr);
    if (retval == 0) {
        fprintf(stderr, "inet_pton(): invalid network address!\n");
        exit(EXIT_FAILURE);
    } else if (retval == -1) {
        perror("inet_pton()");
        exit(EXIT_FAILURE);
    }

    remote_addr.sin_port = htons(atoi(argv[2])); /* Numer portu. */
    addr_len = sizeof(remote_addr); /* Rozmiar struktury adresowej w bajtach. */

		
	if(connect(sockfd, (sockaddr*)&remote_addr, addr_len)==-1)
    {
        cerr<<"Socket connect error!"<<endl;
        perror("");
        cerr<<"errno: "<<errno;
        exit(EXIT_FAILURE);
    }
	
	while(true)
	{
		cerr<<"Enter message to send"<<endl;
		
		char message[256];
        //scanf("%s",message);
		//cin>>message;
        cin.getline(message, 256);

		
		if(send(sockfd, message, strlen(message), 0)==-1)
		{
			cerr<<"Socket send error!"<<endl;
			cerr<<"errno: "<<errno;
			exit(EXIT_FAILURE);
		}

        if(message[0]=='\000')
        {
            break;
        }
		
		//waiting for response
		if (recvfrom(sockfd, buff, sizeof(buff), 0, NULL, NULL) == -1) 
		{
			cerr<<"Socket receive error!"<<endl;
			cerr<<"errno: "<<errno;
			exit(EXIT_FAILURE);
		}

		//buff[retval] = '\0';

		cerr<<"Response from server: "<<buff<<endl;
	}

    close(sockfd);
	cerr<<"No message provided. Shutting down...";
    exit(EXIT_SUCCESS);
}

