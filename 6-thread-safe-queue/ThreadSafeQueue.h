#pragma once

#include <condition_variable>
#include <mutex>
#include <queue>

template <typename T>
class ThreadSafeQueue 
{
public:
  ThreadSafeQueue() = default;
  ~ThreadSafeQueue() = default;

  ThreadSafeQueue(const ThreadSafeQueue&) = delete;
  ThreadSafeQueue& operator=(const ThreadSafeQueue&) = delete;

  void push(const T& value) 
  {
    {
      std::lock_guard<std::mutex> lock(m_mutex);
      m_queue.push(value);
    }
    m_condVar.notify_one();
  }

  void push(T&& value) 
  {
    {
      std::lock_guard<std::mutex> lock(m_mutex);
      m_queue.push(std::move(value));
    }
    m_condVar.notify_one();
  }

  T pop() 
  {
    std::unique_lock<std::mutex> lock(m_mutex);
    m_condVar.wait(lock, [this] { return !m_queue.empty(); });
    T value = std::move(m_queue.front());
    m_queue.pop();
    return value;
  }

  bool tryPop(T& value) 
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_queue.empty()) {
      return false;
    }
    value = std::move(m_queue.front());
    m_queue.pop();
    return true;
  }

  bool empty() const 
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_queue.empty();
  }
private:
  std::queue<T> m_queue;
  mutable std::mutex m_mutex;
  std::condition_variable m_condVar;
};
