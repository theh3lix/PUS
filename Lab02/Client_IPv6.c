#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h> 
#include <netinet/in.h>
#include <arpa/inet.h>  
#include <unistd.h>    
#include <string.h>
#include <errno.h>
#include <net/if.h>
#include <sys/types.h>

#define MAX_SIZE_OF_MESSAGE 256

char msg[MAX_SIZE_OF_MESSAGE];
int socket_server;                 	
struct sockaddr_in6 server_ip;	

char ip[16];
int port;
char interf[3];

void check_args(int);
void create_socket();
void msg_recieve();
void server_connect();
void fill_struct();
int interface_number();

int main(int argc, char** argv) 
{
    check_args(argc);
	create_socket();

	sscanf(argv[1], "%s", ip);
	sscanf(argv[2], "%d", &port);
	sscanf(argv[3], "%s", interf);
	
    fill_struct();
	server_connect();
    msg_recieve();
    
    close(socket_server);
    exit(EXIT_SUCCESS);
}

void create_socket()
{
	socket_server = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
    if (socket_server == -1) 
    {
		printf("CLIENT: Socket error\n");
        perror("");
        exit(EXIT_FAILURE);
    }
}

void msg_recieve()
{
	memset(msg, 0, MAX_SIZE_OF_MESSAGE);
    recv(socket_server, msg, MAX_SIZE_OF_MESSAGE, 0);
    printf("CLIENT: Server msg: %s\n", msg);
}

void server_connect()
{
	if (connect(socket_server, (struct sockaddr*) &server_ip, sizeof(server_ip)) == -1) 
    {
		printf("CLIENT: Connect error\n");
        perror("");
        exit(EXIT_FAILURE);
    }
}

void check_args(int argc)
{
	if (argc != 4) 
    {
		printf("CLIENT: Bad number of arguments\n");
        printf("CLIENT: Correct <IPv6 ADDRESS> <PORT> <INTERFACE>\n");
        exit(EXIT_FAILURE);
    }
}

void fill_struct()
{
	memset(&server_ip, 0, sizeof(server_ip)); 
    server_ip.sin6_family = AF_INET6; 
    server_ip.sin6_port = htons(port); 
    server_ip.sin6_scope_id = interface_number();
    inet_pton(AF_INET6, ip, &server_ip.sin6_addr);
}

int interface_number()
{
	if (if_nametoindex(interf) == 0)
    {
		printf("CLIENT: Name to index error\n");
		perror("");
		exit(EXIT_FAILURE);
	}
	
	return if_nametoindex(interf);
}
