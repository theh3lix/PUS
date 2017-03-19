#include <netinet/in.h>
#include <error.h>
#include "checksum.h"
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <errno.h>

int	sockfd;
int	socket_option;
int	retval;

struct phdr 
{
    struct in_addr ip_dst;
    unsigned char unused;
    unsigned char protocol;
    unsigned short length;
};

unsigned short checksum;
unsigned char datagram[sizeof(struct udphdr) + sizeof(struct phdr)] = {0};

struct udphdr *udp_header = (struct udphdr *)(datagram);
struct phdr *pseudo_header  = (struct phdr *)(datagram + sizeof(struct udphdr));
struct addrinfo hints;
struct addrinfo *rp, *result;

void hints_init();
void sending_empty_datagram();
void creating_and_setting_socket();

int main(int argc, char** argv) 
{	
    if (argc != 3) 
    {
        printf("Bad number of arguments\n\t<IPv6> <PORT>\n");
        exit(EXIT_FAILURE);
    }

	hints_init();

    retval = getaddrinfo(argv[1], NULL, &hints, &result);
    
    if (retval != 0) 
    {
        printf("Getaddrinfo error\n\terrno: %d\n", errno);
        perror("");
        exit(EXIT_FAILURE);
    }

	creating_and_setting_socket();
	
    udp_header->uh_dport = htons(atoi(argv[2]));
    udp_header->uh_ulen = htons(sizeof(struct udphdr));
    pseudo_header->ip_dst.s_addr = ((struct sockaddr_in*)rp->ai_addr)->sin_addr.s_addr;
    pseudo_header->unused = 0;
    pseudo_header->protocol = IPPROTO_UDP;
    pseudo_header->length = udp_header->uh_ulen; 
    udp_header->uh_sum = 0;
    checksum = internet_checksum((unsigned short *)udp_header,sizeof(struct udphdr) + sizeof(struct phdr));
	
	if(checksum==0)
		udp_header->uh_sum = 0xffff;
	else
		udp_header->uh_sum = checksum;
	
	sending_empty_datagram();
    exit(EXIT_SUCCESS);
}

void hints_init()
{
	memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET6;
    hints.ai_socktype = SOCK_RAW;
    hints.ai_protocol = IPPROTO_UDP;
}

void sending_empty_datagram()
{
    printf("Sending empty packet...\n");
	
    for (;;) 
    {
        retval = sendto(sockfd,datagram, sizeof(struct udphdr),0,rp->ai_addr, rp->ai_addrlen);

        if (retval == -1) 
        {
            printf("Sendto error\n\terrno: %d\n", errno);
            perror("");
        }
        else
            printf ("Packet sent\n");

        sleep(1);
    }
}

void creating_and_setting_socket()
{
	socket_option = 1;
		
	for (rp = result; rp != NULL; rp = rp->ai_next) 
    {
        sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        
        if (sockfd == -1) 
        {
            printf("Socket error\n\terrno: %d\n", errno);
            perror("");
            continue;
        }
		
        int offset = 6;

        retval = setsockopt(sockfd, IPPROTO_IPV6, IPV6_CHECKSUM, &offset, sizeof(offset));
        
        if (retval == -1) 
        {
            printf("Setsocketopt error\n\terrno: %d\n", errno);
            perror("");
            exit(EXIT_FAILURE);
        } 
        else 
        {
            printf("Socket created\n");
            break;
        }
    }

    if (rp == NULL) 
    {
        printf("Socket error\n\terrno: %d\n", errno);
        perror("");
        exit(EXIT_FAILURE);
    }
}
