#include "epoll/Channel.h"
#include "epoll/epoll.hpp"

Channel::Channel(Epoll *_ep, int _fd) : ep(_ep), fd(_fd), events(0), revents(0), inEpoll(false){

}
void Channel::enableReading(){
    events = EPOLLIN | EPOLLET;
    ep->UpdateChannel(this);
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
void Channel::handleEvent() {
    
}
// void Channel::setEvents(uint32_t _ev){
//     events = _ev;
// }

void Channel::setRevents(uint32_t _ev){
    revents = _ev;
}