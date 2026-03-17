#include <algorithm>
#include <cstdint>
#include <iostream>
#include <numeric>
#include <random>
#include <thread>
#include <vector>

std::vector<int> generate_vector()
{
  std::random_device rnd_device;
  std::mt19937 mersenne_engine{rnd_device()};
  std::uniform_int_distribution<int> dist{1, 52};

  auto gen = [&]() { return dist(mersenne_engine); };

  std::vector<int> vec(50);
  std::generate(vec.begin(), vec.end(), gen);
  return vec;
}

uint16_t generate_thread_count(const std::vector<int>& vec)
{
  uint16_t thread_count = std::thread::hardware_concurrency();
  if (thread_count == 0) {
    thread_count = 2;
  }

  if (thread_count > vec.size()) {
    thread_count = static_cast<uint16_t>(vec.size());
  }

  return thread_count;
}

void partial_sum(const std::vector<int>& vec, std::size_t start, std::size_t end, uint64_t &result)
{
  result = 0;
  for (std::size_t i = start; i < end; ++i) {
    result += vec[i];
  }
}

int main()
{
  std::vector<int> vec = generate_vector();
  uint16_t thread_count = generate_thread_count(vec);

  std::vector<std::thread> threads;
  std::vector<uint64_t> partial_results(thread_count, 0);

  std::size_t block_size = vec.size() / thread_count;
  std::size_t remainder = vec.size() % thread_count;

  std::size_t start = 0;

  for (uint16_t i = 0; i < thread_count; ++i) {
    std::size_t end = start + block_size + (i < remainder ? 1 : 0);

    threads.emplace_back(partial_sum, std::cref(vec), start, end, std::ref(partial_results[i]));

    start = end;
  }

  for (auto& t : threads) {
    t.join();
  }

  uint64_t parallel_total = 0;
  for (const auto& part : partial_results) {
    parallel_total += part;
  }

  std::cout << "Parallel sum:        " << parallel_total << '\n';
  return 0;
}
