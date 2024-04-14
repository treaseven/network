#include "TcpServer.h"

TcpServer::TcpServer(const std::string &ip, const uint16_t port)
{
    Socket *servsock = new Socket(createnonblocking());
    servsock->setreuseaddr(true);
    servsock->settcpnodelay(true);
    servsock->setreuseport(true);
    servsock->setkeepalive(true); 

    InetAddress servaddr(ip, port);

    servsock->bind(servaddr);
    servsock->listen();

    Channel *servchannel = new Channel(&loop_, servsock->fd());
    servchannel->setreadcallback(std::bind(&Channel::newconnection, servchannel, servsock));
    servchannel->enablereading();
}

TcpServer::~TcpServer()
{

}

void TcpServer::start()
{
    loop_.run();
}