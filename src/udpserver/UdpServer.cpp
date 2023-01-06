#include "udpServer.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

UdpServer::UdpServer(int _port)
    : server_port(_port)
{
    init();
}

UdpServer::~UdpServer()
{
    close(server_fd);
}

int UdpServer::init()
{ 
    server_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (server_fd < 0) {
        perror("socket()");
        return -1;
    }

    bzero(&addr_serv, sizeof(sockaddr_in));
    addr_serv.sin_family = AF_INET;
    addr_serv.sin_port = htons(server_port);
    addr_serv.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_len = sizeof(addr_serv);

    if (bind(server_fd, (sockaddr *)&addr_serv, serv_len) < 0) {
        perror("bind()");
        return -1;
    }
    return 0;
}

int UdpServer::receiveData()
{
    recv_len = recvfrom(server_fd, recv_buf, sizeof(recv_buf), 0, (sockaddr *)&addr_client, &client_len);
    if (recv_len < 0) {
        perror("recvfrom()");
        return -1;
    }

    recv_buf[recv_len] = '\0';
    return recv_len;
}


