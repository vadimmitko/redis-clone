#ifndef REDIS_CLONE_CONCURRENCY_THREAD_POOL_H_
#define REDIS_CLONE_CONCURRENCY_THREAD_POOL_H_

#include <cstddef>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class ThreadPool {
 public:
  explicit ThreadPool(size_t num_workers = std::thread::hardware_concurrency());
  ~ThreadPool();

  ThreadPool(const ThreadPool&) = delete;
  ThreadPool& operator=(const ThreadPool&) = delete;

  void enqueue(std::function<void()> task);

 private:
  void worker_thread();

  std::vector<std::thread> workers_;
  std::queue<std::function<void()>> task_queue_;
  std::mutex m_;
  std::condition_variable cv_;
  bool stop_ = false;
};
#endif
