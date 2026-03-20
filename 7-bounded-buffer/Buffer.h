#pragma once

#include <condition_variable>
#include <mutex>
#include <queue>

template <typename T>
class Buffer {
public:
  explicit Buffer(std::size_t capacity) : m_capacity(capacity) {}

  void push(const T& value) {
    std::unique_lock<std::mutex> lock(m_mutex);
    m_condVarNotFull.wait(lock, [this] {
      return m_queue.size() != m_capacity;
    });

    m_queue.push(value);
    lock.unlock();
    m_condVarNotEmpty.notify_one();
  }

  void push(T&& value) {
    std::unique_lock<std::mutex> lock(m_mutex);
    m_condVarNotFull.wait(lock, [this] {
      return m_queue.size() != m_capacity;
    });

    m_queue.push(std::move(value));
    lock.unlock();
    m_condVarNotEmpty.notify_one();
  }

  T pop() {
    std::unique_lock<std::mutex> lock(m_mutex);
    m_condVarNotEmpty.wait(lock, [this] {
      return !m_queue.empty();
    });

    T value = std::move(m_queue.front());
    m_queue.pop();
    lock.unlock();
    m_condVarNotFull.notify_one();
    return value;
  }

  bool empty() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_queue.empty();
  }

private:
  std::queue<T> m_queue;
  std::size_t m_capacity;

  mutable std::mutex m_mutex;
  std::condition_variable m_condVarNotEmpty;
  std::condition_variable m_condVarNotFull;
};
