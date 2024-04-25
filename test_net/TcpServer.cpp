#include "TcpServer.h"

TcpServer::TcpServer(const std::string &ip, const uint16_t port, int threadnum)
                    :threadnum_(threadnum), mainloop_(new EventLoop), acceptor_(mainloop_.get(), ip, port), threadpool_(threadnum_, "IO")
{
    mainloop_->setepolltimeoutcallback(std::bind(&TcpServer::epolltimeout, this ,std::placeholders::_1));
    acceptor_.setnewconnectioncb(std::bind(&TcpServer::newconnection, this, std::placeholders::_1));
    
    // threadpool_ = new ThreadPool(threadnum_, "IO");

    for(int ii = 0; ii < threadnum_; ii++)
    {
        subloops_.emplace_back(new EventLoop);
        subloops_[ii]->setepolltimeoutcallback(std::bind(&TcpServer::epolltimeout, this, std::placeholders::_1));
        threadpool_.addtask(std::bind(&EventLoop::run, subloops_[ii].get()));
    }
}

TcpServer::~TcpServer()
{
    //delete acceptor_;
    //delete mainloop_;
    /*for (auto &aa:conns_)
    {
        delete aa.second;
    }*/

    //for (auto &aa:subloops_)
    //    delete aa;
    
    //delete threadpool_;
}

void TcpServer::start()
{
    mainloop_->run();
}

void TcpServer::newconnection(std::unique_ptr<Socket> clientsock)
{
    spConnection conn(new Connection(subloops_[clientsock->fd()%threadnum_].get(), std::move(clientsock)));

    conn->setclosecallback(std::bind(&TcpServer::closeconnection, this, std::placeholders::_1));
    conn->seterrorcallback(std::bind(&TcpServer::errorconnection, this, std::placeholders::_1));
    conn->setonmessagecallback(std::bind(&TcpServer::onmessage, this, std::placeholders::_1, std::placeholders::_2));
    conn->setsendcompletecallback(std::bind(&TcpServer::sendcomplete, this, std::placeholders::_1));
    printf("new connection(fd=%d, ip=%s, port=%d) ok.\n", conn->fd(), conn->ip().c_str(), conn->port());
    conns_[conn->fd()] = conn;

    if(newconnectioncb_) newconnectioncb_(conn);
}

void TcpServer::closeconnection(spConnection conn)
{
    printf("client(eventfd=%d) disconnected.\n", conn->fd());
    //close(fd());
    if(closeconnectioncb_) closeconnectioncb_(conn);
    conns_.erase(conn->fd());
    //delete conn;
}

void TcpServer::errorconnection(spConnection conn)
{
    printf("client(eventfd=%d) error.\n", conn->fd());
    //close(fd());
    if(errorconnectioncb_) errorconnectioncb_(conn);
    conns_.erase(conn->fd());
    //delete conn;
}

void TcpServer::onmessage(spConnection conn, std::string& message)
{
    if(onmessagecb_) onmessagecb_(conn, message);
}

void TcpServer::sendcomplete(spConnection conn)
{
    // printf("send complete.\n");

    if (sendcompletecb_) sendcompletecb_(conn);
}

void TcpServer::epolltimeout(EventLoop *loop)
{
    if(timeoutcb_) timeoutcb_(loop);
}

void TcpServer::setnewconnectioncb(std::function<void(spConnection)> fn)
{
    newconnectioncb_ = fn;
}

void TcpServer::setcloseconnectioncb(std::function<void(spConnection)> fn)
{
    closeconnectioncb_ = fn;
}

void TcpServer::seterrorconnectioncb(std::function<void(spConnection)> fn)
{
    errorconnectioncb_ = fn;
}

void TcpServer::setonmessagecb(std::function<void(spConnection, std::string &message)> fn)
{
    onmessagecb_ = fn;
}

void TcpServer::setsendcompletecb(std::function<void(spConnection)> fn)
{
    sendcompletecb_ = fn;
}

void TcpServer::settimeoutcb(std::function<void(EventLoop *)> fn)
{
    timeoutcb_ = fn;
}