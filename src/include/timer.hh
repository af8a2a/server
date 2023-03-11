/*
 * @Author       : mark
 * @Date         : 2020-06-17
 * @copyleft Apache 2.0
 */ 
#ifndef HEAP_TIMER_H
#define HEAP_TIMER_H

#include <queue>
#include <unordered_map>
#include <ctime>
#include <algorithm>
#include <arpa/inet.h> 
#include <functional> 
#include <cassert> 
#include <chrono>
class Channel;
using TimeoutCallBack = std::function<void()>;
using Clock = std::chrono::high_resolution_clock;
using MS = std::chrono::milliseconds;
using TimeStamp = Clock::time_point;

struct TimerNode {
    int id_;
    TimeStamp expires_;
    TimeoutCallBack callback_;
    bool operator<(const TimerNode& t) {//NOLINT
        return expires_ < t.expires_;
    }
};
class HeapTimer {//NOLINT
public:
    HeapTimer() { heap_.reserve(64); }

    ~HeapTimer() { Clear(); }
    
    void Adjust(int id_, int newExpires);

    void Add(int id_, int timeOut, const TimeoutCallBack& callback);

    void DoWork(int id_);

    void Clear();

    void Tick();

    void Pop();

    int GetNextTick();

private:
    void Del(size_t i);
    
    void Siftup(size_t i);

    bool Siftdown(size_t index, size_t n);

    void SwapNode(size_t i, size_t j);

    std::vector<TimerNode> heap_;

    std::unordered_map<int, size_t> ref_;
};

#endif //HEAP_TIMER_H