#include <cmath>
#include <future>
#include <functional>
#include <iostream>
#include <memory>
#include <thread>

#include "ThreadSafeQueue.h"

int power(int x, int y) {
  return static_cast<int>(std::pow(x, y));
}

int main() {
  ThreadSafeQueue<std::function<void()>> q;

  std::thread worker([&] {
    auto job = q.pop();
    job();
  });

  std::packaged_task<int()> task([] { return power(3, 8); });
  std::future<int> fut = task.get_future();

  auto taskPtr = std::make_shared<std::packaged_task<int()>>(
    std::move(task));

  q.push([taskPtr]() mutable {
    (*taskPtr)();
  });

  worker.join();
  std::cout << "Packaged Power:\t" << fut.get() << "\n";
}
