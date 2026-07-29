#include <condition_variable>
#include <thread>
#include <mutex>
#include <functional>
#include <queue>

class ThreadPool {

  public:
    ThreadPool(size_t num_workers = std::thread::hardware_concurrency()) {
      workers_.reserve(num_workers);
      for (int i = 0; i < num_workers; i++) {
        workers_.emplace_back(std::thread(&ThreadPool::worker_thread, this));
      }
    }
    ~ThreadPool() {
      stop_ = false;
      for (int i = 0; i < workers_.size(); i++) {
        workers_[i].join();
      }
    }
    ThreadPool(ThreadPool&&) = delete;
    ThreadPool& operator=(ThreadPool&&) = delete;

    void enqueue(std::function<void()>&& f) {
      {
        std::unique_lock lk(m_);
        task_queue_.push(std::move(f));
      }
      cv_.notify_one();
    }

  private:
    std::queue<std::function<void()>> task_queue_ = {};
    std::vector<std::thread> workers_ =  {};

    std::mutex m_;
    std::condition_variable cv_;
    bool stop_ = false;

    void worker_thread() {
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
}; 
