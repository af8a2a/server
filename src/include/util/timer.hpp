/*
 * @Author       : mark
 * @Date         : 2020-06-17
 * @copyleft GPL 2.0
 */

#include <arpa/inet.h>
#include <algorithm>
#include <cassert>
#include <ctime>
#include <functional>
#include <queue>
#include <unordered_map>
#include <utility>

using TimerCb = std::function<void()>;

class HeapTimer {
 public:
  HeapTimer() { heap_.reserve(64); }

  ~HeapTimer() { Clear(); }

  struct TimerNode {
    //定时器生效的绝对时间
    time_t expires_;
    //回调函数
    TimerCb cbfunc_;
    //下标
    size_t index_;
    //id
    int id_;
    auto operator<(const TimerNode &t) -> bool { return expires_ < t.expires_; }
  };

  auto Action(int id) -> bool;

  auto Adjust(int id, time_t newExpires) -> bool;
    //添加目标定时器
  auto Add(time_t timeSlot, TimerCb cbfunc) -> int;

  auto Del(int id) -> bool;

  void Clear();

  void Tick();

 private:
  void Siftup(size_t i);

  void Siftdown(size_t i, size_t n);

  void Del(TimerNode *node);

  void Action(TimerNode *node);

  auto NextCount() -> int;

  std::vector<TimerNode *> heap_;

  std::unordered_map<int, TimerNode *> ref_;

  int id_count_;
};
void HeapTimer::Siftup(size_t i) {
  size_t j = (i - 1) / 2;
  while (i > 0 && heap_[i] < heap_[j]) {
    std::swap(heap_[i], heap_[j]);
    heap_[i]->index_ = i;
    heap_[j]->index_ = j;
    i = j;
    j = (i - 1) / 2;
  }
}

void HeapTimer::Siftdown(size_t i, size_t n) {
  size_t j = i * 2 + 1;
  while (j < n) {
    if (j + 1 < n && heap_[j + 1] < heap_[j]) {
      j++;
    }
    if (heap_[i] < heap_[j]) {
      break;
    }
    std::swap(heap_[i], heap_[j]);
    heap_[i]->index_ = i;
    heap_[j]->index_ = j;
    i = j;
    j = i * 2 + 1;
  }
}

auto HeapTimer::Action(int id) -> bool {
  if (ref_.count(id) != 1) {
    return false;
  }
  auto cb = std::move(ref_[id]->cbfunc_);
  if (cb) {
    cb();
  };
  return Del(id);
}

void HeapTimer::Action(TimerNode *node) {
  auto cb = std::move(node->cbfunc_);
  if (cb) {
    cb();
  };
  Del(node);
}

auto HeapTimer::Add(time_t timeSlot, TimerCb cbfunc) -> int {
  time_t expire = time(nullptr) + timeSlot;
  size_t index = heap_.size();
  int id = NextCount();
  auto *node = new TimerNode({expire, std::move(cbfunc), index, id});
  heap_.push_back(node);
  Siftup(heap_.size() - 1);
  ref_[id] = node;
  return id;
}

auto HeapTimer::Del(int id) -> bool {
  if (!ref_.contains(id)) {
    return false;
  }
  Del(ref_[id]);
  return true;
}

void HeapTimer::Del(TimerNode *node) {
  assert(node != nullptr && !heap_.empty());
  size_t i = node->index_;
  size_t n = heap_.size() - 1;
  assert(i <= n);
  if (i != n) {
    std::swap(heap_[i], heap_[n]);
    heap_[i]->index_ = i;
    Siftdown(i, n);
    Siftup(i);
  }
  heap_.pop_back();
  ref_.erase(node->id_);
  delete node;
}

auto HeapTimer::Adjust(int id, time_t newExpires) -> bool {
  if (ref_.count(id) != 1) {
    return false;
  }
  ref_[id]->expires_ = newExpires;
  Siftdown(ref_[id]->index_, heap_.size());
  return true;
}

void HeapTimer::Tick() {
  if (heap_.empty()) {
    return;
  }
  time_t now = time(nullptr);
  while (!heap_.empty()) {
    auto node = heap_.front();
    if (now < node->expires_) {
      break;
    }
    Action(node);
  }
}

void HeapTimer::Clear() {
  ref_.clear();
  for (auto &item : heap_) {
    delete item;
  }
  heap_.clear();
}

auto HeapTimer::NextCount() -> int { return ++id_count_; }