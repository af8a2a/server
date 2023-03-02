
#include "Channel.hh"
#include <unistd.h>
#include <utility>
#include "Eventloop.hh"
#include "epoll.hh"

Channel::Channel(EventLoop *_loop, int _fd)
    : loop(_loop), fd(_fd), events(0), ready(0), inEpoll(false){};
void Channel::handleEvent() {
    if(ready & (EPOLLIN | EPOLLPRI)){
        readCallback();
    }
    if(ready & (EPOLLOUT)){
        writeCallback();
    }
}

void Channel::enableReading(){
    events |= EPOLLIN | EPOLLPRI;
    loop->UpdateChannel(this);
}
Channel::~Channel() {
        if(fd != -1){
        close(fd);
        fd = -1;
    }
}
void Channel::useET(){
    events |= EPOLLET;
    loop->UpdateChannel(this);
}
auto Channel::getFd() -> int{
    return fd;
}

auto Channel::getEvents() -> uint32_t{
    return events;
}
auto Channel::getReady() -> uint32_t{
    return ready;
}

auto Channel::getInEpoll() -> bool{
    return inEpoll;
}

void Channel::setInEpoll(bool _in){
    inEpoll = _in;
}

void Channel::setReady(uint32_t _ev){
    ready = _ev;
}

void Channel::setReadCallback(std::function<void()> _cb){
    readCallback = std::move(_cb);
}