#include <fstream>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

std::mutex fileMutex;

void writeLog(std::fstream& writer)
{
  std::lock_guard<std::mutex> lock(fileMutex);
  writer.seekp(0, std::ios::end);
  writer << "Log: " << std::this_thread::get_id() << "\n";
}

int main()
{
  std::fstream writer("log.txt", std::ios::out | std::ios::app);

  const int threadCount = 100;
  std::vector<std::thread> threads;

  for (int i = 0; i < threadCount; i++) {
    threads.emplace_back(write_log, std::ref(writer));
  }

  for (auto& t : threads) {
    t.join();
  }

  writer.close();
  return 0;
}
