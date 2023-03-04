#include "timer.hh"
void HeapTimer::Siftup(size_t i) {  // NOLINT
  size_t j = (i - 1) / 2;           // NOLINT
  while (i > 0 && heap_[i] < heap_[j]) {
    std::swap(heap_[i], heap_[j]);
    heap_[i]->index_ = i;
    heap_[j]->index_ = j;
    i = j;
    j = (i - 1) / 2;
  }
}

void HeapTimer::Siftdown(size_t index, size_t nums) {  // NOLINT
  size_t j = index * 2 + 1;                            // NOLINT
  while (j < nums) {
    if (j + 1 < nums && heap_[j + 1] < heap_[j]) {
      j++;
    }
    if (heap_[index] < heap_[j]) {
      break;
    }
    std::swap(heap_[index], heap_[j]);
    heap_[index]->index_ = index;
    heap_[j]->index_ = j;
    index = j;
    j = index * 2 + 1;
  }
}

auto HeapTimer::Action(int _id) -> bool {
  if (ref_.count(_id) != 1) {
    return false;
  }
  auto callback = std::move(ref_[_id]->cbfunc_);
  if (callback) {
    callback();
  };
  return Del(_id);
}

void HeapTimer::Action(TimerNode *node) {
  auto callback = std::move(node->cbfunc_);
  if (callback) {
    callback();
  };
  Del(node);
}

auto HeapTimer::Add(time_t timeSlot, TimerCb cbfunc) -> int {
  time_t expire = time(nullptr) + timeSlot;
  size_t index = heap_.size();
  int timer_id = NextCount();
  auto *node = new TimerNode({expire, std::move(cbfunc), index, timer_id});
  heap_.push_back(node);
  Siftup(heap_.size() - 1);
  ref_[timer_id] = node;
  return timer_id;
}

auto HeapTimer::Del(int _id) -> bool {
  if (!ref_.contains(_id)) {
    return false;
  }
  Del(ref_[_id]);
  return true;
}

void HeapTimer::Del(TimerNode *node) {
  assert(node != nullptr && !heap_.empty());
  size_t index = node->index_;
  size_t size = heap_.size() - 1;
  assert(index <= size);
  if (index != size) {
    std::swap(heap_[index], heap_[size]);
    heap_[index]->index_ = index;
    Siftdown(index, size);
    Siftup(index);
  }
  heap_.pop_back();
  ref_.erase(node->id_);
  delete node;
}

auto HeapTimer::Adjust(int _id, time_t newExpires) -> bool {
  if (ref_.count(_id) != 1) {
    return false;
  }
  ref_[_id]->expires_ = newExpires;
  Siftdown(ref_[_id]->index_, heap_.size());
  return true;
}

void HeapTimer::Tick() {
  if (heap_.empty()) {
    return;
  }
  time_t now = time(nullptr);
  while (!heap_.empty()) {
    auto *node = heap_.front();
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
HeapTimer::HeapTimer() { heap_.reserve(64); }
HeapTimer::~HeapTimer() { Clear(); }