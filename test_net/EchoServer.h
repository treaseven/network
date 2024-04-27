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
    void Stop();
    
    void HandleNewConnection(spConnection conn);
    void HandleClose(spConnection conn);
    void HandleError(spConnection conn);
    void HandleMessage(spConnection conn, std::string& message);
    void HandleSendComplete(spConnection conn);

    void OnMessage(spConnection conn, std::string& message);
};