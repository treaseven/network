#include "EchoServer.h"

EchoServer::EchoServer(const std::string &ip, const uint16_t port, int threadnum, int workthreadnum):tcpserver_(ip, port, threadnum), threadpool_(workthreadnum, "WORKS")
{
    tcpserver_.setnewconnectioncb(std::bind(&EchoServer::HandleNewConnection, this, std::placeholders::_1));
    tcpserver_.setcloseconnectioncb(std::bind(&EchoServer::HandleClose, this, std::placeholders::_1));
    tcpserver_.seterrorconnectioncb(std::bind(&EchoServer::HandleError, this, std::placeholders::_1));
    tcpserver_.setonmessagecb(std::bind(&EchoServer::HandleMessage, this, std::placeholders::_1, std::placeholders::_2));
    tcpserver_.setsendcompletecb(std::bind(&EchoServer::HandleSendComplete, this, std::placeholders::_1));
}

EchoServer::~EchoServer()
{

}

void EchoServer::Start()
{
    tcpserver_.start();
}

void EchoServer::Stop()
{
    threadpool_.stop();
    printf("工作线程已停止.\n");

    tcpserver_.stop();
}

void EchoServer::HandleNewConnection(spConnection conn)
{
    //std::cout << "New Connection Come in" << std::endl;
    //printf("void EchoServer::HandleNewConnection() thread is %ld.\n", syscall(SYS_gettid));
    printf("new connection(fd=%d, ip=%s, port=%d) ok.\n", conn->fd(), conn->ip().c_str(), conn->port());
    
}

void EchoServer::HandleClose(spConnection conn)
{
    //std::cout << "EchoServer conn close" << std::endl;
    printf("connection closed(fd=%d, ip=%s, port=%d).\n", conn->fd(), conn->ip().c_str(), conn->port());
}

void EchoServer::HandleError(spConnection conn)
{
    //std::cout << "EchoServer conn error" << std::endl;
}

void EchoServer::HandleMessage(spConnection conn, std::string& message)
{
    if (threadpool_.size() == 0)
    {
        OnMessage(conn, message);
    }
    else
    {
        threadpool_.addtask(std::bind(&EchoServer::OnMessage, this, conn, message));
    }
}

void EchoServer::HandleSendComplete(spConnection conn)
{
    //std::cout << "Message send complete" << std::endl;
}

void EchoServer::OnMessage(spConnection conn, std::string& message)
{
    message = "reply:" + message;
    conn->send(message.data(), message.size());
}