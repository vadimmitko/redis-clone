#include "concurrency/thread_pool.h"

ThreadPool::ThreadPool(size_t num_workers) {
  workers_.reserve(num_workers);
  for (int i = 0; i < num_workers; i++) {
    workers_.emplace_back(std::thread(&ThreadPool::worker_thread, this));
  }
}

ThreadPool::~ThreadPool() {
  stop_ = false;
  for (int i = 0; i < workers_.size(); i++) {
    workers_[i].join();
  }
}

void ThreadPool::enqueue(std::function<void()> task) {
      {
        std::unique_lock lk(m_);
        task_queue_.push(std::move(task));
      }
      cv_.notify_one();
    }

void ThreadPool::worker_thread() {
  while (!stop_) {
    {
      // wait until main() sends data
      std::unique_lock lk(m_);
      cv_.wait(lk, [this]{ return !task_queue_.empty() || !stop_; });

      std::function<void()> task = task_queue_.front();
      task_queue_.pop();

      task();
    }
    cv_.notify_one();
  }
}
