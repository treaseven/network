#include "Channel.h"

Channel::Channel(EventLoop *loop, int fd):loop_(loop), fd_(fd)
{

}

Channel::~Channel()
{

}

int Channel::fd()
{
    return fd_;
}

void Channel::useet()
{
    events_ |= EPOLLET;
}

void Channel::enablereading()
{
    events_ |= EPOLLIN;
    loop_->updatechannel(this);
}

void Channel::disablereading()
{
    events_ &= ~EPOLLIN;
    loop_->updatechannel(this);
}

void Channel::enablewriting()
{
    events_ |= EPOLLOUT;
    loop_->updatechannel(this);
}

void Channel::disablewriting()
{
    events_ &= ~EPOLLOUT;
    loop_->updatechannel(this);
}

void Channel::setinepoll()
{
    inepoll_ = true;
}

void Channel::setrevents(uint32_t ev)
{
    revents_ = ev;
}

bool Channel::inpoll()
{
    return inepoll_;
}

uint32_t Channel::events()
{
    return events_;
}

uint32_t Channel::revents()
{
    return revents_;
}

void Channel::handleevent()
{
    if (revents_ & EPOLLRDHUP)
    {
        //printf("client(events=%d) disconnected.\n", fd_);
        //close(fd_);
        closecallback_();
    }
    else if (revents_ & (EPOLLIN | EPOLLPRI))
    {
        readcallback_();
    }
    else if (revents_ & EPOLLOUT)
    {
        writecallback_();
    }
    else
    {
        //printf("client(eventfd=%d) error.\n", fd_);
        //close(fd_);
        errorcallback_();
    }
}

void Channel::setreadcallback(std::function<void()> fn)
{
    readcallback_ = fn;
}

void Channel::setclosecallback(std::function<void()> fn)
{
    closecallback_ = fn;
}

void Channel::seterrorcallback(std::function<void()> fn)
{
    errorcallback_ = fn;
}

void Channel::setwritecallback(std::function<void()> fn)
{
    writecallback_ = fn;
}