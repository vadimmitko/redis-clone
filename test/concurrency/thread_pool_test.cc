#include "concurrency/thread_pool.h"

#include <atomic>
#include <chrono>
#include <gtest/gtest.h>

template <typename F, typename T, typename... Args>
auto curry(F func, T* ptr, Args... args) {
    return [func, ptr, args...]() { func(*ptr, args...); };
}

void process(std::atomic<char>& value) {
  value.fetch_add(1);
}

class ThreadPoolTest : public testing::Test {
};

TEST_F(ThreadPoolTest, ThreadTasksUpdateCounter) {
  std::atomic<char> counter = {0};
  {
    ThreadPool tp = ThreadPool();
    tp.enqueue(curry(process, &counter));
    tp.enqueue(curry(process, &counter));
    tp.enqueue(curry(process, &counter));
  }

  EXPECT_EQ(counter.load(), 3);
}

TEST_F(ThreadPoolTest, GracefulShutdownDrainsQueuedTasks) {
  std::atomic<uint16_t> counter = {0};

  {
    ThreadPool tp = ThreadPool(2);

    for (int i = 0; i < 10; i++) {
      tp.enqueue([&counter]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        counter.fetch_add(1);
      });
    }
  }

  EXPECT_EQ(counter.load(), 10);
}

TEST_F(ThreadPoolTest, DestructsCleanlyWithNoTasksEnqueued) {
  ThreadPool tp = ThreadPool();
}
