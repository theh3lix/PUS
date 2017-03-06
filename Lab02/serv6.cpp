
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h> /* socket() */
#include <netinet/in.h> /* struct sockaddr_in */
#include <arpa/inet.h>  /* inet_ntop() */
#include <unistd.h>     /* close() */
#include <string.h>
#include <errno.h>

int main(int argc, char** argv) {

    int             sockfd; /* Deskryptor gniazda. */
    int             retval; /* Wartosc zwracana przez funkcje. */
    int new_socket;
    struct          sockaddr_in6 client_addr, server_addr;     /* Gniazdowe struktury adresowe (dla klienta i serwera): */
    socklen_t       client_addr_len, server_addr_len;     /* Rozmiar struktur w bajtach: */

    /* Bufor wykorzystywany przez recvfrom() i sendto(): */
    char*           wiadomosc = "Laboratorium PUS02";
    char            adresIPv6[INET6_ADDRSTRLEN];

    if (argc != 2) {
        fprintf(stderr, "Invocation: %s <PORT>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    sockfd = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd == -1) {
        perror("socket()");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));     /* Wyzerowanie struktury adresowej serwera: */
    server_addr.sin6_family          =       AF_INET6;     /* Domena komunikacyjna (rodzina protokolow): */
    server_addr.sin6_port            =       htons(atoi(argv[1]));     /* Numer portu: */
    server_addr.sin6_addr            =       in6addr_any;    /* Adres nieokreslony (ang. wildcard address): */
    server_addr_len                  =       sizeof(server_addr);     /* Rozmiar struktury adresowej serwera w bajtach: */

    /* Powiazanie "nazwy" (adresu IP i numeru portu) z gniazdem: */
    retval = bind(sockfd, (struct sockaddr*) &server_addr, server_addr_len);
    if ( retval == -1) {
        perror("bind()");
        exit(EXIT_FAILURE);
    }

    fprintf(stdout, "Server is listening for incoming connection...\n");
    client_addr_len = sizeof(client_addr);

    /* Oczekiwanie na dane od klienta: */
    if (listen(sockfd, SOMAXCONN) == -1) { //SOMAXCONN ~128
        perror("listen()");
        exit(EXIT_FAILURE);
    }

    while(1){

        if ((new_socket = accept(sockfd, (struct sockaddr *)&client_addr, (socklen_t*)&client_addr_len)) < 0){
				perror("accept");
				exit(EXIT_FAILURE);
			}else{
			    if(!IN6_IS_ADDR_V4MAPPED(&client_addr.sin6_addr))
                    printf("Client is v6\n");
                  else
                    printf("Client is v4\n");
			    inet_ntop(AF_INET6, &client_addr.sin6_addr, adresIPv6, INET6_ADDRSTRLEN );
                printf("---ADRES: %s \n---PORT: %d \n *****\n",adresIPv6, ntohs(client_addr.sin6_port));
                fflush(stdout);

                send( new_socket, wiadomosc, strlen(wiadomosc), 0);
			}
        close(new_socket);
    }

    exit(EXIT_SUCCESS);
}

