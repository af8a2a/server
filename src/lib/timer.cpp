/*
 * @Author       : mark
 * @Date         : 2020-06-17
 * @copyleft Apache 2.0
 */
#include "timer.hh"
#include "Channel.hh"
void HeapTimer::Siftup(size_t i) {
  assert(i >= 0 && i < heap_.size());
  size_t j = (i - 1) / 2;
  while (j >= 0) {
    if (heap_[j] < heap_[i]) {
      break;
    }
    SwapNode(i, j);
    i = j;
    j = (i - 1) / 2;
  }
}

void HeapTimer::SwapNode(size_t i, size_t j) {
  assert(i >= 0 && i < heap_.size());
  assert(j >= 0 && j < heap_.size());
  std::swap(heap_[i], heap_[j]);
  ref_[heap_[i].id_] = i;
  ref_[heap_[j].id_] = j;
}

bool HeapTimer::Siftdown(size_t index, size_t n) {
  assert(index >= 0 && index < heap_.size());
  assert(n >= 0 && n <= heap_.size());
  size_t i = index;
  size_t j = i * 2 + 1;
  while (j < n) {
    if (j + 1 < n && heap_[j + 1] < heap_[j]) {
      j++;
    }
    if (heap_[i] < heap_[j]) {
      break;
    }
    SwapNode(i, j);
    i = j;
    j = i * 2 + 1;
  }
  return i > index;
}

void HeapTimer::Add(int id, int timeout, const TimeoutCallBack &callback) {
  // printf("add success");
  assert(id >= 0);
  size_t i;
  if (!ref_.contains(id)) {
    /* 新节点：堆尾插入，调整堆 */
    i = heap_.size();
    ref_[id] = i;
    heap_.push_back({id, Clock::now() + MS(timeout), callback});
    Siftup(i);
  } else {
    /* 已有结点：调整堆 */
    i = ref_[id];
    heap_[i].expires_ = Clock::now() + MS(timeout);
    heap_[i].callback_ = callback;
    if (!Siftdown(i, heap_.size())) {
      Siftup(i);
    }
  }
}

void HeapTimer::DoWork(int id) {
  /* 删除指定id结点，并触发回调函数 */
  if (heap_.empty() || !ref_.contains(id)) {
    return;
  }
  size_t index = ref_[id];
  TimerNode node = heap_[index];
  //node.callback_();
  Del(index);
}

void HeapTimer::Del(size_t index) {
  /* 删除指定位置的结点 */
  assert(!heap_.empty() && index >= 0 && index < heap_.size());
  /* 将要删除的结点换到队尾，然后调整堆 */
  size_t i = index;
  size_t n = heap_.size() - 1;
  assert(i <= n);
  if (i < n) {
    SwapNode(i, n);
    if (!Siftdown(i, n)) {
      Siftup(i);
    }
  }
  /* 队尾元素删除 */
  ref_.erase(heap_.back().id_);
  heap_.pop_back();
}

void HeapTimer::Adjust(int id, int timeout) {
  /* 调整指定id的结点 */
  assert(!heap_.empty() && ref_.contains(id));
  heap_[ref_[id]].expires_ = Clock::now() + MS(timeout);
  ;
  Siftdown(ref_[id], heap_.size());
}

void HeapTimer::Tick() {
  /* 清除超时结点 */
  if (heap_.empty()) {
    return;
  }
  //printf("time is%ld",std::chrono::duration_cast<MS>(Clock::now().time_since_epoch()).count());
  while (!heap_.empty()) {
    TimerNode node = heap_.front();
    //printf("time is%ld\n", std::chrono::duration_cast<MS>(node.expires_ - Clock::now()).count());
    if (std::chrono::duration_cast<MS>(node.expires_ - Clock::now()).count() > 0) {  // 没超时
      break;
    }
    // 超时执行
    printf("timeout!\n");

    node.callback_();
    Pop();
  }
}

void HeapTimer::Pop() {
  assert(!heap_.empty());
  Del(0);
}

void HeapTimer::Clear() {
  ref_.clear();
  heap_.clear();
}

int HeapTimer::GetNextTick() {
  Tick();
  size_t res = -1;
  if (!heap_.empty()) {
    res = std::chrono::duration_cast<MS>(heap_.front().expires_ - Clock::now()).count();
    if (res < 0) {
      res = 0;
    }
  }
  return static_cast<int>(res);
}