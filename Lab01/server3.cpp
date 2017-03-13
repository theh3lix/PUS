/*
 * Data:                2009-02-10
 * Autor:               Jakub Gasior <quebes@mars.iti.pk.edu.pl>
 * Kompilacja:          $ gcc server2.c -o server2
 * Uruchamianie:        $ ./server2 <numer portu>
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h> /* socket() */
#include <netinet/in.h> /* struct sockaddr_in */
#include <arpa/inet.h>  /* inet_ntop() */
#include <unistd.h>     /* close() */
#include <string.h>
#include <errno.h>

#include "libpalindrome.c"

#include <iostream>



using namespace std;

int main(int argc, char** argv) {

    int             sockfd; /* Deskryptor gniazda. */
    int             retval; /* Wartosc zwracana przez funkcje. */

    /* Gniazdowe struktury adresowe (dla klienta i serwera): */
    struct          sockaddr_in client_addr, server_addr;

    /* Rozmiar struktur w bajtach: */
    socklen_t       client_addr_len, server_addr_len;

    /* Bufor wykorzystywany przez recvfrom() i sendto(): */
    char            buff[256];
    memset(&buff, 0, sizeof(buff));

    /* Bufor dla adresu IP klienta w postaci kropkowo-dziesietnej: */
    char            addr_buff[256];


    if (argc != 2) {
        fprintf(stderr, "Invocation: %s <PORT>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* Utworzenie gniazda dla protokolu UDP: */
    sockfd = socket(PF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        perror("socket()");
        exit(EXIT_FAILURE);
    }

    /* Wyzerowanie struktury adresowej serwera: */
    memset(&server_addr, 0, sizeof(server_addr));
    /* Domena komunikacyjna (rodzina protokolow): */
    server_addr.sin_family          =       AF_INET;
    /* Adres nieokreslony (ang. wildcard address): */
    server_addr.sin_addr.s_addr     =       htonl(INADDR_ANY);
    /* Numer portu: */
    server_addr.sin_port            =       htons(atoi(argv[1]));
    /* Rozmiar struktury adresowej serwera w bajtach: */
    server_addr_len                 =       sizeof(server_addr);

    /* Powiazanie "nazwy" (adresu IP i numeru portu) z gniazdem: */
    if (bind(sockfd, (struct sockaddr*) &server_addr, server_addr_len) == -1) {
        perror("bind()");
        exit(EXIT_FAILURE);
    }

    

	while(true)
	{
		fprintf(stdout, "Server is listening for incoming connection...\n");
		
		
		client_addr_len = sizeof(client_addr);

		int recvfromRetval = recvfrom(sockfd,buff, sizeof(buff),0,(struct sockaddr*)&client_addr, &client_addr_len);
		
		
		if(recvfromRetval==-1)
		{
			cerr<<"Socket receive error!"<<endl;
			cerr<<"errno: "<<errno;
			exit(EXIT_FAILURE);
		}
		
		if(recvfromRetval==0)
		{
			break;
		}
		

		
		fprintf(stdout, "UDP datagram received from %s:%d. Echoing message...\n", inet_ntop(AF_INET, &client_addr.sin_addr, addr_buff, sizeof(addr_buff)), ntohs(client_addr.sin_port));
			
		//czy na pewno wysylany jest \0 ?
		
		char currentChar;
		size_t i=0;
		
		
		do
		{
			//DOPISAC TO DO UWAG DO SPRAWKA - bez sensu 2x sprawdzanie
			if(!isdigit(buff[i]))
			{
				cout<<"String is not na number!";
				close(sockfd);
				exit(EXIT_FAILURE);
			}
			i++;
		} while(currentChar!='\0');


        char retVal;
        switch(is_palindrome(buff, strlen(buff)))
        {
            case 1:
                retVal='t';
                break;
            case 0:
                retVal='n';
                break;
            default:
                retVal='x';
                break;
        }
		

        //uwagi: blad w kodzie pliku server2.c
		//sending response back to client
        //wtf? - zwracane -1 a errno jest 0
		if(sendto(sockfd,&retVal,sizeof(retVal),0,(struct sockaddr*)&client_addr, client_addr_len)==-1);
		{
			cerr<<"Error sending message back. errno: "<<errno<<endl;
            perror("");
			close(sockfd);
			exit(EXIT_FAILURE);
		}		
	}
	
	

    close(sockfd);
	cout<<"No message provided. Shutting down...";
    exit(EXIT_SUCCESS);
}
