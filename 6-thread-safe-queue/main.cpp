#include <iostream>
#include <thread>
#include <vector>
#include <mutex>

#include "ThreadSafeQueue.h"

int main() {
  ThreadSafeQueue<int> q;

  constexpr int producerCount = 2;
  constexpr int itemsPerProducer = 50;
  const int totalItems = producerCount * itemsPerProducer;

  int consumed = 0;
  std::mutex consumedMutex;

  std::vector<std::thread> producers;
  producers.reserve(producerCount);

  for (int p = 0; p < producerCount; ++p) {
    producers.emplace_back([&, p]() {
      for (int i = 0; i < itemsPerProducer; ++i) {
        int value = p * 1 + i;
        q.push(value);
      }
    });
  }

  std::thread consumer([&]() {
    while (true) {
      int value = q.pop();

      int nowConsumed = 0;
      {
        std::lock_guard<std::mutex> lock(consumedMutex);
        consumed++;
        nowConsumed = consumed;
      }

      if (nowConsumed <= 10) {
        std::cout << "Popped: " << value << "\n";
      }

      if (nowConsumed >= totalItems) {
        break;
      }
    }
  });

  for (auto& t : producers) t.join();
  consumer.join();

  {
    std::lock_guard<std::mutex> lock(consumedMutex);
    std::cout << "Total consumed: " << consumed << "\n";
  }

  std::cout << "Queue empty: " << std::boolalpha << q.empty() << "\n";

  int leftover = 0;
  bool hasLeftover = q.tryPop(leftover);
  std::cout << "tryPop after test: " << std::boolalpha << hasLeftover << "\n";
  if (hasLeftover) {
    std::cout << "Leftover value: " << leftover << "\n";
  }

  return 0;
}
