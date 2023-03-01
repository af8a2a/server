
#include <utility>
#include "Channel.hh"
#include "epoll.hh"
#include "Eventloop.hh"

Channel::Channel(EventLoop *_loop, int _fd): loop_(_loop), fd(_fd){};
void Channel::handleEvent(){
    callback();
}

void Channel::enableReading(){
    events = EPOLLIN | EPOLLET;
    loop_->UpdateChannel(this);
}

auto Channel::getFd() -> int{
    return fd;
}

auto Channel::getEvents() -> uint32_t{
    return events;
}
auto Channel::getRevents() -> uint32_t{
    return revents;
}

auto Channel::getInEpoll() -> bool{
    return inEpoll;
}

void Channel::setInEpoll(){
    inEpoll = true;
}
Channel::~Channel() {
    delete loop_;
}

void Channel::setRevents(uint32_t _ev){
    revents = _ev;
}

void Channel::setCallback(std::function<void()> _cb){
    callback = std::move(_cb);
}