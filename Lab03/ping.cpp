#include <iostream>
#include <stdlib.h>
#include <cstdlib>
#include <unistd.h>
#include <random>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netdb.h>
#include <cstring>

#include "checksum.h"


void mainProcess();
void childProcess();

using namespace std;

addrinfo hints;
addrinfo *result, *rp;
int socketDescriptor;

int main(int argc, char* argv[])
{
    int ttl=128;


    if(argc!=2)
    {
        cerr<<"Propper invocation: ./ping <targetHostName/IpAdrress>";
    }


    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_RAW;
    hints.ai_protocol = IPPROTO_ICMP;


    if(getaddrinfo(argv[1], NULL, &hints, &result)!=0)
    {
        cerr<<"Fork error! "<<endl;
        perror("");
        exit(EXIT_FAILURE);
    }

    for (rp = result; rp != NULL; rp = rp->ai_next)
    {
        if ((socketDescriptor = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol)) == -1)
        {
            perror("socket()");
            continue;
        }

        if (setsockopt(socketDescriptor,IPPROTO_IP, IP_TTL, &ttl, sizeof(int)) == -1)
        {
            perror("setsockopt()");
            exit(EXIT_FAILURE);
        }
        else
            break;
    }

    if (rp == nullptr)
    {
        fprintf(stderr, "Client failure: could not create socket.\n");
        exit(EXIT_FAILURE);
    }

    
    auto ppid=fork();

    if(ppid==0)
        mainProcess();
    else if(ppid>0)
        childProcess();
    else
    {
        cerr<<"Fork error! "<<endl;
        perror("");
        exit(EXIT_FAILURE);
    }
}


void mainProcess() {

    unsigned char data[32];
    unsigned char datagram[sizeof(struct icmphdr) + sizeof(data)];
    struct icmphdr *icmp_header = (struct icmphdr *) (datagram);


    for (size_t i = 1; i <= 4; i++)
    {

    }
}


void childProcess()
{


}
