

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h> /* socket() */
#include <netinet/in.h> /* struct sockaddr_in */
#include <arpa/inet.h>  /* inet_ntop() */
#include <unistd.h>     /* close() */
#include <string.h>
#include <errno.h>
#include <iostream>
#include <cstdlib>
#include <vector
#include <algorithm>

using namespace std;


std::vector<int>clients(30);
int listenfd, connfd; /* Deskryptory dla gniazda nasluchujacego i polaczonego: */
int max_sd;
fd_set readfds;
struct sockaddr_in client_addr, server_addr;     /* Gniazdowe struktury adresowe (dla klienta i serwera): */

void checkForNewConnection()
{
    int new_socket;

    socklen_t client_addr_len= sizeof(client_addr);

    //mozna sie pozbyc tego sprawdzania
    if (FD_ISSET(listenfd, &readfds)){
        /* Z pewnoscia to nowe polaczenie, trzeba je dodac */
        if ((new_socket = accept(listenfd, (struct sockaddr *)&client_addr, (socklen_t*)&client_addr_len)) < 0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        /* Informacja o nowym polaczeniu */
        printf("New connection , socket fd is %d , ip is : %s , port : %d. Connected users: %d.\n", new_socket, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port),clients.size()); //size()-+1 bo od 0 ale +1 bo socket serwera

        char* message="Welcome to chat project\n";
        /* Wysylamy wiadomosc powitalna */
        if (send(new_socket, message, strlen(message), 0) != strlen(message)){
            perror("send");
        }

        /* Potwierdzamy na konsli wyslanie wiadomosci */
        puts("Welcome message sent successfully");

        /* Dodajemy polaczenie do tablicy */
        clients.push_back(new_socket);
    }
}

void checkConnectedClients()
{
    /* Sprawdzamy inne sockety */
    for (i = 0; i < 30; i++){
        sd = clients[i];
        /* Jesli na danym sockecie przyszla wiadomosc */
        if (FD_ISSET(sd, &readfds))	{
            /* Jesli byla to informacja o zamknieciu polaczenia */
            if ((valread = read(sd, buff, sizeof(buff))) == 0){
                /* Host sie odlaczyl, wypisujemy informacje */
                getpeername(sd, (struct sockaddr*)&client_addr, (socklen_t*)&client_addr_len);
                printf("Host disconnected , ip %s , port %d \n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

                /* Zamykamy socket i usuwamy klienta z tablicy oraz zmniejszamy licznik podlaczanych klientow */
                close(sd);
                clients[i] = 0;
                clients_num--;
            }
                /* Jesli uzytkownik sie nie odlaczyl, napisal wiadomosc */
            else{
                /* Wypisujemy informacje na serwerze */
                printf("Got message: %s from socket %d\n", buff, sd);
                /* Konczymy string znakiem NULL */
                buff[valread] = '\0';
                char str[15];
                /* Wyposazamy wiadomosc w informacje o tym, kto ja przeslal */
                sprintf(str, "%d", sd);
                strcat(str, ": ");
                strcat(str, buff);
                int j;
                /* I przesylamy do innych klientow (wykluczamy klienta, ktory przeslal wiadomosc) */
                for (j = 0; j < 30; j++)
                    if(clients[j]!=sd && clients[j]!=0)
                        send(clients[j], str, strlen(str), 0);
            }
        }
    }



}



int main(int argc, char** argv) {


    int  retval; /* Wartosc zwracana przez funkcje. */

    socklen_t server_addr_len;


    int sd, valread;



    char buff[256];     /* Bufor wykorzystywany przez write() i read(): */
    char addr_buff[256]; /* Bufor dla adresu IP klienta w postaci kropkowo-dziesietnej: */



    if (argc != 2) {
        fprintf(stderr, "Invocation: %s <PORT>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* Utworzenie gniazda dla protokolu TCP: */
    listenfd = socket(PF_INET, SOCK_STREAM, 0);
    if (listenfd == -1) {
        perror("socket()");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family          =       AF_INET;
    server_addr.sin_addr.s_addr     =       htonl(INADDR_ANY);
    server_addr.sin_port            =       htons(atoi(argv[1]));
    server_addr_len                 =       sizeof(server_addr);





    if (bind(listenfd, (struct sockaddr*) &server_addr, server_addr_len) == -1)
    {
        perror("bind()");
        exit(EXIT_FAILURE);
    }

    if (listen(listenfd, 2) == -1)
    {
        perror("listen()");
        exit(EXIT_FAILURE);
    }


    fprintf(stdout, "Server is listening for incoming connection...\n");





    while(true)
    {
        FD_ZERO(&readfds); 	/*  Wyzerowanie struktury przechowujacej sockety (tymczasowo) */
        FD_SET(listenfd, &readfds); /* Dodanie glownego socketu do struktury */
        max_sd = listenfd; 	/* Chwilowo maksymalnym identyfikatorem jest jedyny umieszczony w strukturze */


        for(auto element : clients)
        {
            //to tez chyba(czy na pewno?) mozna usunac tego ifa
            /* Jesli jest podlaczony klient */
            if(element>0)
            {
                /*Dodajemy go do struktury i doliczamy, jesli identyfikator jego socketa jest najwyzszy */
                FD_SET(element, &readfds);
                if (element > max_sd)
                    max_sd = element;
            }
        }

        int retVal = select(max_sd + 1, &readfds, NULL, NULL, NULL);

        if ((retVal < 0) && (errno != EINTR))	{
            printf("select error");
        }

        checkForNewConnection();
        checkConnectedClients();

    }
}
