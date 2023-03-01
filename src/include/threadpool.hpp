#pragma once
#include <cassert>
#include <condition_variable>
#include <functional>
#include <memory>
#include <queue>
#include <thread>
#include <type_traits>

//线程池
class ThreadPool {
 public:
  explicit ThreadPool(size_t threadCount) : pool_(std::make_shared<Pool>()) {
    // 处理线程
    assert(threadCount > 0);
    for (size_t i = 0; i < threadCount; i++) {
      std::thread([pool = pool_] {
        std::unique_lock<std::mutex> locker(pool->mtx_);
        while (true) {
          if (!pool->tasks_.empty()) {
            auto current = std::move(pool->tasks_.front());
            pool->tasks_.pop();
            locker.unlock();
            current();
            locker.lock();
          } else if (pool->is_closed_) {
            break;
          } else {
            pool->cond_.wait(locker);
          }
        }
      }).detach();
    }
  }

  ThreadPool() = default;
  ThreadPool(ThreadPool &&) = default;
  ~ThreadPool() {
    if (static_cast<bool>(pool_)) {
      {
        std::lock_guard<std::mutex> locker(pool_->mtx_);
        pool_->is_closed_ = true;
      }
      pool_->cond_.notify_all();
    }
  }

  template <class F>
  void AddTask(F &&task) {
    {
      std::lock_guard<std::mutex> locker(pool_->mtx_);
      pool_->tasks_.emplace(std::forward<F>(task));
    }
    pool_->cond_.notify_one();
  }

 private:
  struct Pool {

    //锁
    std::mutex mtx_;
    //条件变量，用于阻塞线程
    std::condition_variable cond_;
    bool is_closed_;
    std::queue<std::function<void()>> tasks_;
  };
  std::shared_ptr<Pool> pool_;
};