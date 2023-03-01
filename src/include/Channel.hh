#pragma once
#include <sys/epoll.h>
#include <functional>
class EventLoop;
class Channel
{
private:
    EventLoop *loop_;
    int fd;
    uint32_t events{0};
    uint32_t revents{0};
    bool inEpoll{false};
    std::function<void()> callback;
public:
    Channel(EventLoop *_loop, int _fd);
    ~Channel();
    void handleEvent();
    void enableReading();

    auto getFd() -> int;
    auto getEvents() -> uint32_t;
    auto getRevents() -> uint32_t;
    auto getInEpoll() -> bool;
    void setInEpoll();

    // void setEvents(uint32_t);
    void setRevents(uint32_t);
    void setCallback(std::function<void()>);
};

