#include "Connection.h"

Connection::Connection(EventLoop *loop, Socket *clientsock):loop_(loop), clientsock_(clientsock)
{
    clientchannel_ = new Channel(loop_, clientsock_->fd());
    clientchannel_->setreadcallback(std::bind(&Connection::onmessage, this));
    clientchannel_->setclosecallback(std::bind(&Connection::closecallback, this));
    clientchannel_->seterrorcallback(std::bind(&Connection::errorcallback, this));
    clientchannel_->useet();
    clientchannel_->enablereading();
}

Connection::~Connection()
{
    delete clientsock_;
    delete clientchannel_;
}

int Connection::fd() const
{
    return clientsock_->fd();
}

std::string Connection::ip() const
{
    return clientsock_->ip();
}

uint16_t Connection::port() const
{
    return clientsock_->port();
}

void Connection::closecallback()
{
    //printf("client(eventfd=%d) disconnected.\n", fd());
    //close(fd());
    closecallback_(this);
}

void Connection::errorcallback()
{
    //printf("client(eventfd=%d) error.\n", fd());
    //close(fd());
    errorcallback_(this);
}

void Connection::setclosecallback(std::function<void(Connection *)> fn)
{
    closecallback_ = fn;
}

void Connection::seterrorcallback(std::function<void(Connection *)> fn)
{
    errorcallback_ = fn;
}

void Connection::onmessage()
{
    char buffer[1024];
    while(true)
    {
        bzero(&buffer, sizeof(buffer));
        ssize_t nread = read(fd(), buffer, sizeof(buffer));

        if (nread > 0)
        {
            printf("recv(eventfd=%d):%s\n", fd(), buffer);
            send(fd(), buffer, strlen(buffer), 0);
        }
        else if (nread == -1 && errno == EINTR)
        {
            continue;
        }
        else if (nread == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK) ))
        {
            break;
        }
        else if (nread == 0)
        {
            //printf("client(eventfd=%d) disconnected.\n", fd_);
            //close(fd_);
            closecallback();
            break;
        }
    }
}