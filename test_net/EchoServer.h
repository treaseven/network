#pragma once
#include "TcpServer.h"
#include "EventLoop.h"
#include "Connection.h"
#include "ThreadPool.h"

class EchoServer
{
private:
    TcpServer tcpserver_;
    ThreadPool threadpool_;
public:
    EchoServer(const std::string &ip, const uint16_t port, int threadnum=3, int workthreadnum=5);
    ~EchoServer();

    void Start();
    void HandleNewConnection(Connection *conn);
    void HandleClose(Connection *conn);
    void HandleError(Connection *conn);
    void HandleMessage(Connection *conn, std::string& message);
    void HandleSendComplete(Connection *conn);

    void OnMessage(Connection *conn, std::string& message);
};