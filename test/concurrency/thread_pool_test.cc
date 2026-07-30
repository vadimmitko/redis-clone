#include "concurrency/thread_pool.h"

#include <atomic>
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
