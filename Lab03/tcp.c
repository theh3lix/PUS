#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <error.h>
#include <errno.h>
#include "checksum.h"

#define SOURCE_PORT 6666
#define SOURCE_ADDRESS "192.168.2.2"

struct phdr 
{
    struct in_addr ip_src, ip_dst;
    unsigned char unused;
    unsigned char protocol;
    unsigned short length;

    struct tcphdr tcp;
};

int sockfd;
int socket_option;
int retval; 
unsigned short checksum;

struct addrinfo hints;
struct ip *ip_header;
struct tcphdr *tcp_header;
struct addrinfo*rp, *result;
struct phdr pseudo_header;


void ip_create()
{
	ip_header->ip_hl = 5; 
    ip_header->ip_v = 4; 
    ip_header->ip_tos = 0; 
    ip_header->ip_len = sizeof(struct ip) + sizeof(struct tcphdr);
    ip_header->ip_id = 0;
    ip_header->ip_off = 0;
    ip_header->ip_ttl = 255;
    ip_header->ip_p = IPPROTO_TCP;
    ip_header->ip_src.s_addr = inet_addr(SOURCE_ADDRESS);
    ip_header->ip_dst.s_addr = ((struct sockaddr_in*)rp->ai_addr)->sin_addr.s_addr;
    ip_header->ip_sum = internet_checksum((unsigned short *)ip_header,sizeof(struct ip));
}

void tcp_create(int port)
{
	tcp_header->source = htons(SOURCE_PORT);
    tcp_header->dest = htons(port); 
    tcp_header->seq = 0;
    tcp_header->ack_seq = 0;
    tcp_header->doff = 5;
    tcp_header->fin = 0;
    tcp_header->syn = 1;
    tcp_header->rst = 0;
    tcp_header->psh = 0;
    tcp_header->ack = 0;
    tcp_header->urg = 0;
    tcp_header->window = htons(5840);
    tcp_header->check = 0;
    tcp_header->urg_ptr	= 0;
}

void pseudo_create()
{
	pseudo_header.ip_src.s_addr = ip_header->ip_src.s_addr; 
    pseudo_header.ip_dst.s_addr = ip_header->ip_dst.s_addr; 
    pseudo_header.unused = 0;
    pseudo_header.protocol = IPPROTO_TCP;
    pseudo_header.length = htons(20);
}

int main(int argc, char** argv) 
{	
    unsigned char datagram[sizeof(struct ip) + sizeof(struct tcphdr) + sizeof(struct phdr)] = {0};
    ip_header = (struct ip *)datagram;
    tcp_header = (struct tcphdr *)(datagram + sizeof(struct ip));

    if (argc != 3) 
    {
        printf("Bad number of arguments\n\t<IP ADDR> <PORT>\n");
        exit(EXIT_FAILURE);
    }

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = PF_INET; 
    hints.ai_socktype = SOCK_RAW;
    hints.ai_protocol = IPPROTO_TCP;
    retval = getaddrinfo(argv[1], NULL, &hints, &result);

    if (retval < 0) 
    {
        printf("Getaddinfo error\n\terrno: %d\n", errno);
        perror("");
        exit(EXIT_FAILURE);
    }

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

        retval = setsockopt(sockfd,IPPROTO_IP, IP_HDRINCL,&socket_option, sizeof(int));
        
        if (retval == -1) 
        {
            printf("Setsockopt error\n\terrno: %d\n", errno);
            perror("");
            exit(EXIT_FAILURE);
        } 
        else 
            break;
    }
	
    if (rp == NULL) 
    {
        printf("Error while creating socket\n\terrno: %d\n", errno);
        perror("");
        exit(EXIT_FAILURE);
    }

	ip_create();
	tcp_create(atoi(argv[2]));
	pseudo_create();

    memcpy(&pseudo_header.tcp , tcp_header , sizeof (struct tcphdr));

    printf("Sending...\n\n");

    tcp_header->check = internet_checksum((unsigned short *) &pseudo_header, sizeof(struct phdr));

    while(1)
    {
        retval = sendto(sockfd, datagram,ip_header->ip_len,0,rp->ai_addr, rp->ai_addrlen);
        
        if (retval == -1) 
        {
            printf("Sendto error\n\terrno: %d\n", errno);
            perror("");
        }        
        else
            printf ("Packet sent\n");
        

        sleep(1);
    }

    exit(EXIT_SUCCESS);
}
