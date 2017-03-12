#include <cstdlib>
#include <netdb.h>
#include <cstdio>
#include <cstring>
#include <arpa/inet.h>
#include <cerrno>
#include <zconf.h>
#include <pthread.h>
#include <malloc.h>
#include <dirent.h>

#pragma GCC diagnostic ignored "-Wwrite-strings"

#define MAX_CLIENT_MESSAGE 2000
#define MAX_BUFFOR_SIZE 1000

void* connection_handler(void* socket_descriptor)
{
    // Get the socket descriptor
    int sock = *(int*)socket_descriptor;
    char* client_message[MAX_CLIENT_MESSAGE];
    DIR* dir;
    struct dirent* ent;
    char buff_send[MAX_BUFFOR_SIZE];
    int read_size;

    printf("SERVER: Thread function\n");

    // Reading images
    if ((dir = opendir("/home/qhoros/CLionProjects/server5/img/")) != NULL)
    {
        strcpy(buff_send, "<HTML>\n\t<BODY>\n\t\t<CENTER>\n");

        while((ent = readdir(dir)) != NULL)
        {
            if (!strcmp(ent->d_name, ".") || !strcmp(ent->d_name, ".."))
                continue;
            else if (strstr(ent->d_name, ".gif") != NULL || strstr(ent->d_name, ".png") != NULL || strstr(ent->d_name, ".jpg") != NULL || strstr(ent->d_name, ".jpeg") != NULL)
            {
                strcat(buff_send, "\t\t\t<IMG SRC='");
                strcat(buff_send, ent->d_name);
                strcat(buff_send, "'/></IMG><BR />\n");
            }
            else
                continue;
        }
        strcat(buff_send, "\t\t</CENTER>\n\t</BODY>\n</HTML>\n");

        closedir(dir);
    }
    else
    {
        printf("SERVER: Could not open 'img' directory\n\terrno: %d\n", errno);
        perror("");
        exit(EXIT_FAILURE);
    }

    // Sending message
    if (write(sock, buff_send, strlen(buff_send)) < 0)
    {
        printf("SERVER: Write message error\n\terrno: %d\n", errno);
        perror("");
        exit(EXIT_FAILURE);
    }

    //Receive a message from client
    read_size = recv(sock, client_message, MAX_CLIENT_MESSAGE, 0);

    if(read_size == 0)
    {
        printf("SERVER: Client disconnected");
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        printf("SERVER: Recv failed\n\terrno: %d\n", errno);
        perror("");
        exit(EXIT_FAILURE);
    }

    free(socket_descriptor);
    return 0;
}

int main(int argc, char** argv)
{
    int socket_descriptor, new_socket, *new_sock, port_number;
    struct sockaddr_in server, client;
    char* message;

    if (argc != 2)
    {
        printf("SERVER: Need to specify port number\n");
        exit(EXIT_FAILURE);
    }

    sscanf(argv[1], "%d", &port_number);

    // Create socket
    socket_descriptor = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);

    if (socket_descriptor < 0)
    {
        printf("SERVER: Could not create socket\n\terrno: %d\n", errno);
        perror("");
        exit(EXIT_FAILURE);
    }

    // Prepare the sockaddr_in structure
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port_number);

    // Bind
    if (bind(socket_descriptor, (struct sockaddr*)&server, sizeof(server)) < 0)
    {
        printf("SERVER: Bind failed\n\terrno: %d\n", errno);
        perror("");
        exit(EXIT_FAILURE);
    }

    printf("SERVER: Bind done\n");

    // Listen
    listen(socket_descriptor, 3);

    // Accept an incoming connection
    printf("SERVER: Waiting...\n");
    int size = sizeof(struct sockaddr_in);

    while((new_socket = accept(socket_descriptor, (struct sockaddr*)&client, (socklen_t*)&size)))
    {
        if (new_socket < 0)
        {
            printf("SERVER: Accept failed\n\terrno: %d\n", errno);
            perror("");
            exit(EXIT_FAILURE);
        }

        printf("SERVER: Connection accepted\n");

        // Reply to the client
        //message = "Hello client...\n";
        //write(new_socket, message, strlen(message));

        // Creating clinet thread
        pthread_t new_thread;
        new_sock = (int*)malloc(1);
        *new_sock= new_socket;

        if (pthread_create(&new_thread, NULL, connection_handler, (void*)new_sock) < 0)
        {
            printf("SERVER: Could not create thread\n\terrno: %d\n", errno);
            perror("");
            exit(EXIT_FAILURE);
        }

        pthread_join(new_thread, NULL);
        printf("SERVER: Thread handler assigned\n");
    }

    close(socket_descriptor);
    exit(EXIT_SUCCESS);
}
