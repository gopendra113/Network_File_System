#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include "server_functionalities.h"

int CCSSportNo;
int NMSSportNo;
#define MAX_AX_PATHS 100
#define PATH_LENGTH 1024
#define max_clients 2000
#define SIZEOFNAME 128
char HomeDirectory[128] = "";

typedef struct response
{
    int responsevalue; // 1 : Success , 0 : Failure , 2 : FileNotFound , 3 : FileFound , 4 : Being Written Onto
} Response;
typedef struct client_Arg
{
    int ServerSock;
    struct sockaddr_in ServerAddress;
} client_Arg;

typedef struct storage_server_info
{
    char ip[15];
    int NMSSport;
    int SSCLport;
    int sizeoffile_paths;
    char file_paths[MAX_AX_PATHS][PATH_LENGTH];
} storage_server_info;
typedef struct input
{
    char oper[50];
    char path1[SIZEOFNAME];
    char path2[SIZEOFNAME];
    int FileorDirec;
} request;

void *client(void *arg)
{
    int client_sock = *(int *)arg;
    char buffer[1024];
    bzero(buffer, 1024);
    request received_request;
    recv(client_sock, buffer, BUFFER_SIZE, 0);
    memcpy(&received_request, buffer, sizeof(received_request));
    char path[128];
    strcpy(path, HomeDirectory);
    strcat(path, "/");
    strcat(path, received_request.path1);
    strcpy(received_request.path1, path);
    printf("Received struct from client:\n");
    printf("Name: %s\n", received_request.oper);
    printf("Address1: %s\n", received_request.path1);
    printf("Address1: %s\n", received_request.path2);
    // create New Thread
    // Handle the request
    // send response
    if (strcmp(received_request.oper, "read") == 0)
    {
        sc_read(client_sock, received_request.path1);
    }
    else if (strcmp(received_request.oper, "write") == 0)
    {
        sc_write(client_sock, received_request.path1);
    }
    else if (strcmp(received_request.oper, "get") == 0)
    {
        sc_permission(client_sock, received_request.path1);
    }
    printf("socket: %d\n", client_sock);
    close(client_sock);
}

void *client_handler(void *arg)
{
    int socket_for_client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (socket_for_client < 0)
    {
        perror("Socket Failed");
        exit(1);
    }
    int client_sock;
    struct sockaddr_in server_addr, client_addr;
    memset(&server_addr, '\0', sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(CCSSportNo);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    char buffer[1024];
    if (bind(socket_for_client, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Bind Failed");
        exit(1);
    }
    listen(socket_for_client, 5);
    printf("Listening for Clients...\n");

    pthread_t threads[max_clients];
    int i = 0;
    while (i < max_clients)
    {
        int ClientLength = sizeof(client_addr);
        bzero((char *)&client_addr, ClientLength);
        int client_sock = accept(socket_for_client, (struct sockaddr *)&client_addr, &ClientLength);
        printf("[+]SS connected.\n");

        if (pthread_create(&threads[i], NULL, client, &client_sock) != 0)
        {
            fprintf(stderr, "Error creating thread\n");
            // return 1;
        }

        // bzero(buffer, 1024);
        // request received_request;
        // read(client_sock, buffer, sizeof(received_request));
        // memcpy(&received_request, buffer, sizeof(received_request));

        // printf("Received struct from client:\n");
        // printf("Name: %s\n", received_request.oper);
        // printf("Address1: %s\n", received_request.path1);
        // printf("Address1: %s\n", received_request.path2);

        // create New Thread
        // Handle the request
        // send response
        // close(client_sock);
        i++;
    }
    for (int j = 0; j < max_clients; j++)
    {
        if (pthread_join(threads[j], NULL) != 0)
        {
            fprintf(stderr, "Error joining thread\n");
            // return ;
        }
    }
}
void *NaminHandler(void *arg)
{
    int client_sock = *(int *)arg;
    request Request;
    if (recv(client_sock, &Request, sizeof(Request), 0) == -1)
    {
        perror("recv failed");
        exit(1);
    }
    if (!strcmp(Request.oper, "create"))
    {
        char path[128];
        strcpy(path, HomeDirectory);
        strcat(path, "/");
        strcat(path, Request.path1);
        strcpy(Request.path1, path);
        strcat(Request.path1, "/");
        strcat(Request.path1, Request.path2);
        int answer = -1;
        if (Request.FileorDirec == 1)
        {
            answer = make_file(Request.path1);
        }
        else if (Request.FileorDirec == 2)
        {
            answer = make_directory(Request.path1);
        }
        Response Resp;
        Resp.responsevalue = answer;
        if (send(client_sock, &Resp, sizeof(Resp), 0) == -1)
        {
            perror("Sending Response Failed");
            exit(1);
        }
    }
    else if (!strcmp(Request.oper, "delete"))
    {
        char path[128];
        strcpy(path, HomeDirectory);
        strcat(path, "/");
        strcat(path, Request.path1);
        strcpy(Request.path1, path);
        int answer = -1;
        if (Request.FileorDirec == 1)
        {
            answer = delete_file(Request.path1);
        }
        else if (Request.FileorDirec == 2)
        {
            answer = delete_directory(Request.path1);
        }
        Response Resp;
        Resp.responsevalue = answer;
        if (send(client_sock, &Resp, sizeof(Resp), 0) == -1)
        {
            perror("Sending Response Failed");
            exit(1);
        }
    }
    close(client_sock);
    return NULL;
}
void *NamingServerListener()
{
    int NamingSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (NamingSock < 0)
    {
        perror("Naming Server Listener : Socket Creation Failed");
        exit(1);
    }

    struct sockaddr_in Storageserver_addr, client_addr;
    char buffer[1024];
    int n;

    memset(&Storageserver_addr, '\0', sizeof(Storageserver_addr));
    Storageserver_addr.sin_family = AF_INET;
    Storageserver_addr.sin_port = htons(NMSSportNo);
    Storageserver_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (bind(NamingSock, (struct sockaddr *)&Storageserver_addr, sizeof(Storageserver_addr)) < 0)
    {
        perror("[-]Bind error");
        exit(1);
    }
    listen(NamingSock, 5);
    printf("Listening...\n");
    while (1)
    {
        int ClientLength = sizeof(client_addr);
        bzero((char *)&client_addr, ClientLength);
        int client_sock = accept(NamingSock, (struct sockaddr *)&client_addr, &ClientLength);
        printf("[+]SS connected. zoya\n");
        pthread_t Thread_ID;
        if (pthread_create(&Thread_ID, NULL, NaminHandler, &client_sock) != 0)
        {
            perror("Thread Creation Error");
            exit(1);
        }
    }
    return NULL;
}
int main(int argc, char *argv[])
{
    CCSSportNo = atoi(argv[1]);
    NMSSportNo = atoi(argv[2]);
    getcwd(HomeDirectory, 128);
    char *ip = "127.0.0.1";
    int port = 5566;
    int sock;
    struct sockaddr_in Naming_Server_Addr;
    socklen_t addr_size;
    int n;
    char **AccessiblePaths = (char **)malloc(sizeof(char *) * 2);
    for (int i = 0; i < 2; i++)
    {
        AccessiblePaths[i] = (char *)malloc(sizeof(char) * 1024);
    }
    int currentsize = 2;
    char path[1024];
    int i = 0;
    while (1)
    {
        fgets(path, sizeof(path), stdin);
        if (path[0] == '\n')
        {
            break;
        }
        else
        {
            path[strlen(path) - 1] = '\0';
            strcpy(AccessiblePaths[i++], path);
            if (i >= currentsize)
            {
                currentsize = 2 * currentsize;
                AccessiblePaths = (char **)realloc(AccessiblePaths, sizeof(char *) * currentsize);
                for (int j = i; j < currentsize; j++)
                {
                    AccessiblePaths[j] = (char *)malloc(sizeof(char) * 1024);
                }
            }
        }
    }
    AccessiblePaths = (char **)realloc(AccessiblePaths, sizeof(char *) * i);
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock < 0)
    {
        perror("Socket Error");
        exit(1);
    }

    memset(&Naming_Server_Addr, '\0', sizeof(Naming_Server_Addr));
    Naming_Server_Addr.sin_family = AF_INET;
    Naming_Server_Addr.sin_port = htons(port);
    Naming_Server_Addr.sin_addr.s_addr = inet_addr(ip);

    if (connect(sock, (struct sockaddr *)&Naming_Server_Addr, sizeof(Naming_Server_Addr)) == -1)
    {
        perror("Connect Error");
        exit(1);
    }

    storage_server_info details;
    strcpy(details.ip, ip);
    details.NMSSport = NMSSportNo;
    details.SSCLport = CCSSportNo;
    details.sizeoffile_paths = i;
    for (int k = 0; k < i; k++)
    {
        strcpy(details.file_paths[k], AccessiblePaths[k]);
    }
    if (send(sock, &details, sizeof(details), 0) == -1)
    {
        perror("Send Error");
        exit(1);
    }
    close(sock);

    pthread_t NamingServerThread;
    if (pthread_create(&NamingServerThread, NULL, (void *)&NamingServerListener, NULL) != 0)
    {
        perror("Pthread Error");
        exit(1);
    }
    pthread_t ClientsListeningThread;
    if (pthread_create(&ClientsListeningThread, NULL, (void *)client_handler, NULL) != 0)
    {
        perror("Pthread Error : Client Thread");
        exit(1);
    }

    if (pthread_join(NamingServerThread, NULL) != 0)
    {
        perror("pthread join");
        exit(EXIT_FAILURE);
    }
    if (pthread_join(ClientsListeningThread, NULL) != 0)
    {
        perror("pthread join");
        exit(EXIT_FAILURE);
    }
    printf("Disconnected from the naming server.\n");
    return 0;
}