#pragma once

#include <atomic>
#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>
class ThreadPool {
  using uLock = std::unique_lock<std::mutex>;

 public:
  ThreadPool(int threadNum);
  template <typename F, typename... Args>
  auto enqueue(F&& f, Args&&... args);
  ~ThreadPool();

 private:
  void addThread();
  std::vector<std::thread> workers;
  std::queue<std::function<void()>> tasks;
  std::atomic<bool> stop;
  // 保护tasks用
  std::mutex mtx;
  std::condition_variable cv;
};

inline void ThreadPool::addThread() {
  workers.emplace_back([this] {
    // 每次for循环:
    // 等待可以拿
    // 拿任务
    // 执行
    while (true) {
      std::function<void()> task;
      {
        // 进入临界区
        uLock locker(this->mtx);
        cv.wait(locker, [this] { return this->stop || !this->tasks.empty(); });
        if (this->stop && this->tasks.empty()) {
          locker.unlock();
          return;
        }
        task = std::move(this->tasks.front());
        this->tasks.pop();
      }
      task();
    }
  });
}

inline ThreadPool::ThreadPool(int threadNum) : stop(false) {
  for (int i = 0; i < threadNum; ++i) {
    addThread();
  }
}

// 异步
template <typename F, typename... Args>
auto ThreadPool::enqueue(F&& f, Args&&... args) {
  using returnType = decltype(f(args...));
  // 将packaged_task入对
  using packageTask = std::packaged_task<returnType()>;
  auto task = std::make_shared<packageTask>(
      std::bind(std::forward<F>(f), std::forward<Args>(args)...));
  auto ret = task->get_future();
  {
    uLock locker(mtx);
    tasks.emplace([task] { (*task)(); });
  }
  cv.notify_one();
  return ret;
}

inline ThreadPool::~ThreadPool() {
  stop = true;
  cv.notify_all();
  for (auto& worker : workers) {
    worker.join();
  }
}
