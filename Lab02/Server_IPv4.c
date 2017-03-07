#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> 
#include <unistd.h>     
#include <string.h>
#include <errno.h>

int socket_server; 
int new_socket;
struct sockaddr_in client_ip, server_ip;        

char* msg = "Laboratorium PUS";
char addr_IPv4[INET_ADDRSTRLEN];
int port;

void check_args(int);
void fill_struct();
void create_socket();
void binding();
void listening();
void sending_response();

int main(int argc, char** argv) 
{
	check_args(argc);
	create_socket();
	
	sscanf(argv[1], "%d", &port);
	
	fill_struct();
	binding();

    printf("SERVER: Waiting...\n");
    
    listening();
    sending_response();

    exit(EXIT_SUCCESS);
}

void check_args(int argc)
{
	if (argc != 2) 
    {
		printf("SERVER: Bad number of arguments\n");
        printf("SERVER: Correct: <PORT>\n");
        exit(EXIT_FAILURE);
    }
}

void fill_struct()
{
	memset(&server_ip, 0, sizeof(server_ip));    
    server_ip.sin_family = AF_INET;    
    server_ip.sin_port = htons(port);     
    server_ip.sin_addr.s_addr = htonl(INADDR_ANY);     
}

void create_socket()
{
	socket_server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	
    if (socket_server == -1) 
    {
		printf("SERVER: Socket error\n");
        perror("");
        
        exit(EXIT_FAILURE);
    }
}

void binding()
{
    if ((bind(socket_server, (struct sockaddr*) &server_ip, sizeof(server_ip))) == -1) 
    {
		printf("SERVER: Binding error\n");
        perror("");
        exit(EXIT_FAILURE);
    }
}

void listening()
{	
	if (listen(socket_server, SOMAXCONN) == -1) 
    { 
		printf("SERVER: Listening error\n");
        perror("");
        exit(EXIT_FAILURE);
    }
}
	
void sending_response()
{
	socklen_t client_len = sizeof(client_ip);
	
	while(1)
    {
        if ((new_socket = accept(socket_server, (struct sockaddr *)&client_ip, (socklen_t*)&client_len)) < 0)
        {
			printf("SERVER: Accept error\n");
			perror("");
			exit(EXIT_FAILURE);
		}
		else
		{
			inet_ntop(AF_INET, &client_ip.sin_addr, addr_IPv4, INET_ADDRSTRLEN );
            printf("SERVER: Connection established\nADRES IPv4: %s\nCLIENT PORT: %d\n", addr_IPv4, ntohs(client_ip.sin_port));
            fflush(stdout);
			send(new_socket, msg, strlen(msg), 0);
		}
        close(new_socket);
    }
}
