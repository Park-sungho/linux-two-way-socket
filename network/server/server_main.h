#ifndef SERVER_MAIN_H_
#define SERVER_MAIN_H_

/**
 * @brief Two-way communication 
 * 
 */
class HelloNetServer
{
public:
    int startSendMessageServer();
    int startReceiveLoopback();
    int disconenct();

private:
    int net_connect_fd;
};

#endif  // SERVER_MAIN_H_
