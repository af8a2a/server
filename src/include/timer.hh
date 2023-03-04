/*
 * @Author       : mark
 * @Date         : 2020-06-17
 * @copyleft GPL 2.0
 */
#pragma once
#include <arpa/inet.h>
#include <algorithm>
#include <cassert>
#include <ctime>
#include <functional>
#include <queue>
#include <unordered_map>
#include <utility>
#include "Macros.h"

using TimerCb = std::function<void()>;

class HeapTimer {
 public:
  HeapTimer();
  ~HeapTimer();
  DISALLOW_COPY_AND_MOVE(HeapTimer);

  struct TimerNode {
    //定时器生效的绝对时间
    time_t expires_;
    //回调函数
    TimerCb cbfunc_;
    //下标
    size_t index_;
    //id
    int id_;
    auto operator<(const TimerNode &time) -> bool { return expires_ < time.expires_; }
  };

  auto Action(int _id) -> bool;

  auto Adjust(int _id, time_t newExpires) -> bool;
    //添加目标定时器
  auto Add(time_t timeSlot, TimerCb cbfunc) -> int;

  auto Del(int _id) -> bool;

  void Clear();

  void Tick();

 private:
  void Siftup(size_t i);//NOLINT

  void Siftdown(size_t i, size_t n);//NOLINT

  void Del(TimerNode *node);

  void Action(TimerNode *node);

  auto NextCount() -> int;

  std::vector<TimerNode *> heap_;

  std::unordered_map<int, TimerNode *> ref_;

  int id_count_{};
};
