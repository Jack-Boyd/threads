
#include <iostream>
#include <mutex>
#include <thread>

std::mutex counterMutex;
void count(int& count, int interations)
{
  std::lock_guard<std::mutex> lock(counterMutex);
  for (int i = 0; i < interations; i++) {
    count++;
  }
}

int main()
{
  int counter = 0;
  const int threadCount = 4;
  int iterations = 100000;
  std::vector<std::thread> threads;

  for (int i = 0; i < threadCount; i++) {
    threads.emplace_back(count, std::ref(counter), iterations);
  }

  for (auto& t : threads) {
    t.join();
  } 
  std::cout << "Expected: " << threadCount * iterations << '\n';
  std::cout << "Actual:   " << counter << '\n';
  return 0;
}
