#include <iostream>
#include <unistd.h>
#include <random>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <cstring>

#include "checksum.h"


void mainProcess();
void childProcess();

using namespace std;

const size_t dataSize=32; //32B

addrinfo *rp;

int socketDescriptor;

int main(int argc, char* argv[])
{
    int ttl=128;
    addrinfo hints, *result;


    if(argc!=2)
    {
        cerr<<"Propper invocation: ./ping <targetHostName/IpAdrress>\n";
        exit(EXIT_FAILURE);
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


void mainProcess()
{
    cout<<"Main process working..."<<endl<<endl;

    std::random_device randomSeed;
    std::mt19937 randomGenerator(randomSeed());
    std::uniform_int_distribution<int> distribution(65,90);


    unsigned char datagram[sizeof(icmphdr) + dataSize];
    icmphdr *icmp_header = (icmphdr *)datagram;


    for (size_t i = 1; i <= 4; i++)
    {
        for(int j=0; j<dataSize-1; j++)
            datagram[sizeof(icmp_header) + j]=distribution(randomGenerator);

        datagram[sizeof(icmp_header) + 31] = '\0';

        icmp_header->type = ICMP_ECHO;
        icmp_header->code = 0;
        icmp_header->un.echo.id = htons(getpid());
        icmp_header->un.echo.sequence = htons(i);
        icmp_header->checksum = 0;
        icmp_header->checksum = internet_checksum((unsigned short*)datagram, sizeof(datagram));


        if (sendto(socketDescriptor, (const char*) icmp_header, sizeof(datagram), 0, rp->ai_addr, rp->ai_addrlen) < 0)
        {
            perror("sendto() ");
            exit(EXIT_FAILURE);
        }
        cout<<"Packet has been sent \n";

        sleep(1);
    }
    exit(EXIT_SUCCESS);
}


void childProcess()
{
    cout<<"Child process working..."<<endl<<endl;

    unsigned char datagram[sizeof(icmphdr) + dataSize];
    sockaddr_in addresStruct;

    ip *ipheader = (ip*) datagram;
    icmphdr *icmp_header = (icmphdr *) (datagram + sizeof(ip));

    socklen_t addresStructSize = sizeof(addresStruct);

    for(int i=0;i<4;i++)
    {
        recvfrom(socketDescriptor, datagram, sizeof(datagram), 0, (sockaddr *) &addresStruct, &addresStructSize);

        cout<<"--IP details--"<<endl;
        cout<<"Source address: "<<inet_ntoa(ipheader->ip_src)<<endl;
        cout<<"TTL: "<<ipheader->ip_ttl<<endl;
        cout<<"Header length: "<<ipheader->ip_hl<<endl;
        cout<<"Destination address: "<< inet_ntoa(ipheader->ip_dst) <<endl;
        cout<<"--ICMP details--"<<endl;
        cout<<"Type: "<< (int)icmp_header->type <<endl;
        cout<<"Code: "<< (int)icmp_header->code <<endl;
        cout<<"ID: "<< icmp_header->un.echo.id <<endl;
        cout<<"Sequence number: "<< icmp_header->un.echo.sequence <<endl;
        cout<<endl<<endl;
    }
    exit(EXIT_SUCCESS);
}
