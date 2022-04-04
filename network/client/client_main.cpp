
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

using namespace std;

#define SOCKET_DATA_MAX 1024
#define SERVER_SOCKET_PORT 7200
#define IP_ADDRESS_LEN_MAX 64
#define LOOPBACK_IP_ADDRESS "127.0.0.1"

/**
 * @brief Two-way communication 
 * 
 */
class HelloNetClient
{
public:
    int startSendMessageClient();
    int startReceiveLoopback(char* ip_address);
    int disconenct();

private:
    int net_connect_fd;
};

void *loop_receive(void *socket_fd)
{
    cout << "Client Ready !!!" << endl;

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

int HelloNetClient::startReceiveLoopback(char* ip_address)
{
    pthread_t recv_tid;
    struct sockaddr_in server_address;

    //net_connect_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if ((net_connect_fd = socket(AF_INET , SOCK_STREAM , 0)) == -1)
    {
        cout << "Socket create failed." << endl;
        return -1;
    }

    memset(server_address , 0 , sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(SERVER_SOCKET_PORT);
    if (inet_pton(AF_INET, ip_address, &server_address.sin_addr) < 0)
    {
        cout << "Socket inet_pton() failed." << endl;
        return -1;
    }

    if (connect(net_connect_fd, (sockaddr *)&server_address), sizeof(server_address) < 0)
    {
        cout << "Socket connecting failed." << endl;
        return -1;
    }

    if(pthread_create(&recv_tid , NULL , loop_receive, &net_connect_fd) < 0)
    {
        cout << "Client thread creation failed." << endl;
        return -1;
    }

    cout << "SYSTEM: Connecting IP address : " << ip_address << endl;

    return 0;
}

int HelloNetClient::startSendMessageClient()
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
            strncpy(msg_buff , "\nInfo:Client want to quit. Bye.", sizeof(char)*SOCKET_DATA_MAX);
            send(net_connect_fd , msg_buff , strlen(msg_buff) , 0);
            close(net_connect_fd);
            exit(0);
        }

    } while(1);

    return 0;
}

int HelloNetClient::disconenct()
{
    close(net_connect_fd);

    return 0;
}

int main()
{
    char ip_address[IP_ADDRESS_LEN_MAX] = "\n";
    HelloNetClient hello;
    string input_str;

    cout << "################################" << endl;
    cout << "Start two way connection Client." << endl;
    cout << "################################" << endl;
    cout << "Please, input Server IP address : ";

    cin >> input_str;
    sprintf(ip_address, "%s", input_str.c_str());
    if (input_str.length() < 7)
    {
        cout << "Your IP address is not invalid, set Default IP:" << LOOPBACK_IP_ADDRESS << endl;
        sprintf(ip_address, "%s", LOOPBACK_IP_ADDRESS);
    }

    cout << "Set Server IP address : " << ip_address << endl;

    if (hello.startReceiveLoopback(ip_address) >= 0)
    {
        hello.startSendMessageClient();
    }

    cout << "App stop." << endl;

    return 0;
}
