#ifndef UDP_SERVER_H
#define UDP_SERVER_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

class UdpServer {
public:
    char recv_buf[512];
    char send_buf[512];
    int recv_len;

public:
    UdpServer(int _port);
    ~UdpServer();

    int init();
    int receiveData();

private:
    int server_fd;
    int server_port;

    struct sockaddr_in addr_serv;
    socklen_t serv_len;
    struct sockaddr_in addr_client;
    socklen_t client_len;
};

#endif