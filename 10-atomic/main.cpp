#include <atomic>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

std::mutex counterMutex;

void counter(int& count, int iterations)
{
  for (int i = 0; i < iterations; i++) {
    count++;
  }
}

void lockedCounter(int& count, int iterations)
{
  std::lock_guard<std::mutex> lock(counterMutex);
  for (int i = 0; i < iterations; i++) {
    count++;
  }
}

void atomicCounter(std::atomic<int>& count, int iterations)
{
  for (int i = 0; i < iterations; i++) {
    count.fetch_add(1, std::memory_order_relaxed);
  }
}

int main()
{
  constexpr int threads = 4;
  constexpr int iterations = 250000;

  {
    int c = 0;
    std::vector<std::thread> ts;
    ts.reserve(threads);

    for (int t = 0; t < threads; ++t) {
      ts.emplace_back(counter, std::ref(c), iterations);
    }
    for (auto& th : ts) th.join();

    std::cout << "[no sync] expected " << (threads * iterations)
              << ", got " << c << "\n";
  }

  {
    int c = 0;
    std::vector<std::thread> ts;
    ts.reserve(threads);

    for (int t = 0; t < threads; ++t) {
      ts.emplace_back(lockedCounter, std::ref(c), iterations);
    }
    for (auto& th : ts) th.join();

    std::cout << "[mutex] expected " << (threads * iterations)
              << ", got " << c << "\n";
  }

  {
    std::atomic<int> c{0};
    std::vector<std::thread> ts;
    ts.reserve(threads);

    for (int t = 0; t < threads; ++t) {
      ts.emplace_back(atomicCounter, std::ref(c), iterations);
    }
    for (auto& th : ts) th.join();

    std::cout << "[atomic] expected " << (threads * iterations)
              << ", got " << c.load() << "\n";
  }

  return 0;
}
