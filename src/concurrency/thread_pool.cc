#include "concurrency/thread_pool.h"

ThreadPool::ThreadPool(size_t num_workers) {
  workers_.reserve(num_workers);
  for (int i = 0; i < num_workers; i++) {
    workers_.emplace_back(std::thread(&ThreadPool::worker_thread, this));
  }
}

ThreadPool::~ThreadPool() {
  {
    std::unique_lock lk(m_);
    stop_ = true;
  }
  cv_.notify_all();
  for (auto& worker: workers_) {
    worker.join();
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
  while (true) {
    std::function<void()> task;
    {
      // wait until main() sends data
      std::unique_lock lk(m_);
      cv_.wait(lk, [this]{ return !task_queue_.empty() || stop_; });

      if  (stop_ && task_queue_.empty()) {
        return;
      }

      task = std::move(task_queue_.front());
      task_queue_.pop();
    }
    task();
  }
}
