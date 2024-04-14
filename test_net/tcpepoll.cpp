#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/fcntl.h>
#include <netinet/tcp.h>
#include <sys/epoll.h>
#include "InetAddress.h"
#include "Socket.h"
#include "Epoll.h"
#include "Channel.h"
#include "EventLoop.h"

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("usage:./tcpepoll ip port\n");
        printf("example:./tcpepoll 192.168.31.176 5085\n");
        return -1;
    }

    Socket servsock(createnonblocking());
    servsock.setreuseaddr(true);
    servsock.settcpnodelay(true);
    servsock.setreuseport(true);
    servsock.setkeepalive(true); 

    InetAddress servaddr(argv[1], atoi(argv[2]));

    servsock.bind(servaddr);
    servsock.listen();

    EventLoop loop;
    Channel *servchannel = new Channel(loop.ep(), servsock.fd());
    servchannel->setreadcallback(std::bind(&Channel::newconnection, servchannel, &servsock));
    servchannel->enablereading();

    loop.run();
    return 0;
}
