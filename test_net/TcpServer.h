#pragma once
#include "EventLoop.h"
#include "Socket.h"
#include "Channel.h"
#include "Acceptor.h"
#include <map>

class TcpServer
{
private:
    EventLoop loop_;
    Acceptor *acceptor_;
    std::map<int, Connection*> conns_;
public:
    TcpServer(const std::string &ip, const uint16_t port);
    ~TcpServer();

    void start();
    void newconnection(Socket *clientsock);
    void closeconnection(Connection *conn);
    void errorconnection(Connection *conn);
    void onmessage(Connection *conn, std::string message);
    void sendcomplete(Connection *conn);
    void epolltimeout(EventLoop *loop);
};