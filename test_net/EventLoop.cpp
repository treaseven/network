#include "EventLoop.h"
#include <unistd.h>
#include <sys/syscall.h>

int createtimerfd(int sec=30)
{
    int tfd = timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC | TFD_NONBLOCK);
    struct itimerspec timeout;
    memset(&timeout, 0, sizeof(struct itimerspec));
    timeout.it_value.tv_sec = 5;
    timeout.it_value.tv_nsec = 0;
    timerfd_settime(tfd, 0, &timeout, 0);
    return tfd;
}

EventLoop::EventLoop(bool mainloop_)
            :ep_(new Epoll), wakeupfd_(eventfd(0, EFD_NONBLOCK)), wakechannel_(new Channel(this, wakeupfd_)),
            timerfd_(createtimerfd()), timerchannel_(new Channel(this, timerfd_))
{
    wakechannel_->setreadcallback(std::bind(&EventLoop::handlewakeup, this));
    wakechannel_->enablereading();

    timerchannel_->setreadcallback(std::bind(&EventLoop::handletimer, this));
    timerchannel_->enablereading();
}

EventLoop::~EventLoop()
{
    // delete ep_;
}

void EventLoop::run()
{
    threadid_ = syscall(SYS_gettid);
    while(true)
    {
        std::vector<Channel *> channels = ep_->loop(10 * 1000);

        if (channels.size() == 0)
        {
            epolltimeoutcallback_(this);
        }
        else
        {
            for(auto &ch:channels)
            {
                ch->handleevent();
            }
        }
    }
}

void EventLoop::updatechannel(Channel *ch)
{
    ep_->updatechannel(ch);
}

void EventLoop::removechannel(Channel *ch)
{
    ep_->removechannel(ch);
}

void EventLoop::setepolltimeoutcallback(std::function<void(EventLoop *)> fn)
{
    epolltimeoutcallback_ = fn;
}

bool EventLoop::isinloopthread()
{
    return threadid_ == syscall(SYS_gettid);
}

void EventLoop::queueinloop(std::function<void()> fn)
{
    {
        std::lock_guard<std::mutex> gd(mutex_);
        taskqueue_.push(fn);
    }

    wakeup();
}

void EventLoop::wakeup()
{
    uint64_t val = 1;
    write(wakeupfd_, &val, sizeof(val));
}

void EventLoop::handlewakeup()
{
    printf("handlewakeup() thread id is %ld.\n", syscall(SYS_gettid));
    uint64_t val;
    read(wakeupfd_, &val, sizeof(val));

    std::function<void()> fn;

    std::lock_guard<std::mutex> gd(mutex_);

    while(taskqueue_.size() > 0)
    {
        fn = std::move(taskqueue_.front());
        taskqueue_.pop();
        fn();
    }
}

void EventLoop::handletimer()
{
    struct itimerspec timeout;
    memset(&timeout, 0, sizeof(struct itimerspec));
    timeout.it_value.tv_sec = 5;
    timeout.it_value.tv_nsec = 0;
    timerfd_settime(timerfd_, 0, &timeout, 0);

    if (mainloop_)
        printf("闹钟时间到了.\n");
    else
        printf("从事件循环的闹钟时间到了.\n");
}