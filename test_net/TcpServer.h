#pragma once
#include "EventLoop.h"
#include "Socket.h"
#include "Channel.h"
#include "Acceptor.h"
#include "ThreadPool.h"
#include <map>

class TcpServer
{
private:
    EventLoop *mainloop_;
    std::vector<EventLoop *> subloops_;
    Acceptor *acceptor_;
    ThreadPool *threadpool_;
    int threadnum_;
    std::map<int, Connection*> conns_;
    std::function<void(Connection *)> newconnectioncb_;
    std::function<void(Connection *)> closeconnectioncb_;
    std::function<void(Connection *)> errorconnectioncb_;
    std::function<void(Connection *, std::string &message)> onmessagecb_;
    std::function<void(Connection *)> sendcompletecb_;
    std::function<void(EventLoop *)> timeoutcb_;
public:
    TcpServer(const std::string &ip, const uint16_t port, int threadnum=3);
    ~TcpServer();

    void start();

    void newconnection(Socket *clientsock);
    void closeconnection(Connection *conn);
    void errorconnection(Connection *conn);
    void onmessage(Connection *conn, std::string& message);
    void sendcomplete(Connection *conn);
    void epolltimeout(EventLoop *loop);

    void setnewconnectioncb(std::function<void(Connection *)> fn);
    void setcloseconnectioncb(std::function<void(Connection *)> fn);
    void seterrorconnectioncb(std::function<void(Connection *)> fn);
    void setonmessagecb(std::function<void(Connection *, std::string &message)> fn);
    void setsendcompletecb(std::function<void(Connection *)> fn);
    void settimeoutcb(std::function<void(EventLoop *)> fn);
};