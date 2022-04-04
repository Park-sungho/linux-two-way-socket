
#include <iostream>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <pthread.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

#include "server_main.h"

using namespace std;

#define SOCKET_DATA_MAX 1024
#define SERVER_SOCKET_PORT 7200

/**
 * @brief Two-way communication 
 * 
 */
void *loop_receive(void *socket_fd)
{
    cout << "Server Ready !!!" << endl;

    int sockfd = *(int *)socket_fd;

    do
    {
        char buffer[SOCKET_DATA_MAX];
        int recv_len = 0;

        memset(buffer , 0 , SOCKET_DATA_MAX);
        recv_len = recv(sockfd , buffer , SOCKET_DATA_MAX , 0);

        buffer[recv_len] = '\0';
        if (recv_len < 1)
        {
            printf("Client closed.\n");
            close(sockfd);
            exit(1);
        }

        cout << "From Client : " << buffer << endl;

    } while(1);

    return nullptr;
}

int HelloNetServer::startReceiveLoopback()
{
    int fd;
    int socket_option = 1;
    char ip_address[64] = "\n";
    pthread_t recv_tid;
    struct sockaddr_in socket_client;
    socklen_t socket_length = sizeof(socket_client);

    //fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if((fd = socket(AF_INET , SOCK_STREAM , 0)) == -1)
    {
        cout << "Socket create failed." << endl;
        return -1;
    }

    // Set Socket Option
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *) &socket_option, sizeof(socket_option));

    int result = [](int fd) {
        struct sockaddr_in server_address;
        memset(&server_address, 0, sizeof(server_address));
        server_address.sin_family = AF_INET;
        server_address.sin_addr.s_addr = htonl(INADDR_ANY);
        server_address.sin_port = htons(SERVER_SOCKET_PORT);
        return (bind(fd , (sockaddr *)&server_address , sizeof(server_address)) < 0) ? -1: 0;
    }(fd);

    if (result < 0)
    {
        cout << "Socket bind() falied." << endl;
        return -1;
    }

    // Flow: socket() -> bind() -> listen() -> accept() -> read() | write() -> close()
    int backlog = 5;
    if(listen(fd , backlog) < 0)
    {
        cout << "Socket listen() failed." << endl;
        return -1;
    }

    if((net_connect_fd = accept(fd , (sockaddr *)&socket_client , &socket_length)) < 0)
    {
        cout << "Socket accept() failed." << endl;
        return -1;
    }

    sprintf(ip_address, "%s", inet_ntoa(socket_client.sin_addr));

    if(pthread_create(&recv_tid , NULL , loop_receive, &net_connect_fd) < 0)
    {
        cout << "Server thread creation failed." << endl;
        return -1;
    }

    cout << "SYSTEM: Connected IP address : " << ip_address << endl;

    return 0;
}

int HelloNetServer::startSendMessageServer()
{
    char msg_buff[SOCKET_DATA_MAX] = "\n";

    do
    {    
        cin >> msg_buff;

        if(send(net_connect_fd , msg_buff , strlen(msg_buff) , 0) == -1)
        {
            perror("send() failed.\n");
            exit(1);
        }

        if(strcmp(msg_buff , "quit") == 0)
        {
            memset(msg_buff , 0 , SOCKET_DATA_MAX);
            strncpy(msg_buff , "\nInfo:Server want to quit. Bye.", sizeof(char)*SOCKET_DATA_MAX);
            send(net_connect_fd , msg_buff , strlen(msg_buff) , 0);
            close(net_connect_fd);
            exit(0);
        }

    } while(1);

    return 0;
}

int HelloNetServer::disconenct()
{
    close(net_connect_fd);

    return 0;
}

int main()
{
    HelloNetServer hello;

    cout << "################################" << endl;
    cout << "Start two way connection Server." << endl;
    cout << "################################" << endl;

    if (hello.startReceiveLoopback() >= 0)
    {
        hello.startSendMessageServer();
    }

    cout << "App stop." << endl;

    return 0;
}
