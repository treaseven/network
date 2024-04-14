#include "TcpServer.h"

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("usage:./tcpepoll ip port\n");
        printf("example:./tcpepoll 192.168.31.176 5085\n");
        return -1;
    }

    /*Socket servsock(createnonblocking());
    servsock.setreuseaddr(true);
    servsock.settcpnodelay(true);
    servsock.setreuseport(true);
    servsock.setkeepalive(true); 

    InetAddress servaddr(argv[1], atoi(argv[2]));

    servsock.bind(servaddr);
    servsock.listen();

    EventLoop loop;
    Channel *servchannel = new Channel(&loop, servsock.fd());
    servchannel->setreadcallback(std::bind(&Channel::newconnection, servchannel, &servsock));
    servchannel->enablereading();

    loop.run();*/
    TcpServer tcpserver(argv[1], atoi(argv[2]));
    tcpserver.start();
    return 0;
}
