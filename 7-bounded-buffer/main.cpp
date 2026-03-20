#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

#include "Buffer.h"

int main() {
  const std::size_t capacity = 10;
  const int producerCount = 4;
  const int consumerCount = 4;

  const int itemsPerProducer = 250000;
  const int totalItems = producerCount * itemsPerProducer;

  Buffer<int> buffer(capacity);

  std::vector<int> consumerQuota(consumerCount, totalItems / consumerCount);
  int rem = totalItems % consumerCount;
  for (int i = 0; i < rem; ++i) {
    consumerQuota[i]++;
  }

  auto start = std::chrono::high_resolution_clock::now();

  std::vector<std::thread> producers;
  producers.reserve(producerCount);

  for (int p = 0; p < producerCount; ++p) {
    int startValue = p * itemsPerProducer;
    int endValue = startValue + itemsPerProducer;

    producers.emplace_back([&, startValue, endValue] {
      for (int v = startValue; v < endValue; ++v) {
        buffer.push(v);
      }
    });
  }

  std::vector<std::thread> consumers;
  consumers.reserve(consumerCount);

  for (int c = 0; c < consumerCount; ++c) {
    int quota = consumerQuota[c];

    consumers.emplace_back([&, quota] {
      for (int i = 0; i < quota; ++i) {
        (void)buffer.pop();
      }
    });
  }

  for (auto& t : producers) t.join();
  for (auto& t : consumers) t.join();

  auto end = std::chrono::high_resolution_clock::now();
  auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

  double throughput = totalItems / (ms / 1000.0);
  std::cout << "capacity=" << capacity << "\n"
            << "producerCount=" << producerCount << " consumerCount=" << consumerCount
            << "\n"
            << "totalItems=" << totalItems
            << " timeMs=" << ms
            << " throughput(items/s)=" << throughput << "\n";

  return 0;
}
