#pragma once
#include <sys/epoll.h>

class Epoll;
class Channel
{
private:
    Epoll *ep;
    int fd;
    uint32_t events;
    uint32_t revents;
    bool inEpoll;
public:
    Channel(Epoll *_ep, int _fd);
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
};

