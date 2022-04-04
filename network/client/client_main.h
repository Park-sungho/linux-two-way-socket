#ifndef CLIENT_MAIN_H_
#define CLIENT_MAIN_H_

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

#endif  // CLIENT_MAIN_H_
