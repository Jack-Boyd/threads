#include <future>
#include <iostream>
#include <thread>
#include <vector>

enum class Operation { Add, Sub, Mul, Div };

struct Calculation 
{
  Operation op;
  int a;
  int b;
  std::promise<int> promise;

  Calculation(Operation op_, int a_, int b_) : op(op_), a(a_), b(b_), promise() {}
};

void calculate(Calculation calc)
{
  try {
    int result = 0;
    switch (calc.op) {
      case Operation::Add:
        result = calc.a + calc.b;
        break;
      case Operation::Sub:
        result = calc.a - calc.b;
        break;
      case Operation::Mul:
        result = calc.a * calc.b;
        break;
      case Operation::Div:
        if (calc.b == 0) throw std::runtime_error("division by zero");
        result = calc.a / calc.b;
        break;
    }

    calc.promise.set_value(result);
  } catch (...) {
    calc.promise.set_exception(std::current_exception());
  }
}

int main() 
{
  std::vector<std::thread> threads;
  std::vector<std::future<int>> futures;

  {
    Calculation calc(Operation::Add, 1, 2);
    std::future<int> future = calc.promise.get_future();
    futures.push_back(std::move(future));

    threads.push_back(std::thread(calculate, std::move(calc)));
  }
  {
    Calculation calc(Operation::Mul, 11, 8);
    std::future<int> future = calc.promise.get_future();
    futures.push_back(std::move(future));

    threads.push_back(std::thread(calculate, std::move(calc)));
  }
  {
    Calculation calc(Operation::Div, 24, 6);
    std::future<int> future = calc.promise.get_future();
    futures.push_back(std::move(future));

    threads.push_back(std::thread(calculate, std::move(calc)));
  }
  {
    Calculation calc(Operation::Mul, 13, 92);
    std::future<int> future = calc.promise.get_future();
    futures.push_back(std::move(future));

    threads.push_back(std::thread(calculate, std::move(calc)));
  }
  {
    Calculation calc(Operation::Sub, 49, 39);
    std::future<int> future = calc.promise.get_future();
    futures.push_back(std::move(future));

    threads.push_back(std::thread(calculate, std::move(calc)));
  }
  {
    Calculation calc(Operation::Add, 249, 698);
    std::future<int> future = calc.promise.get_future();
    futures.push_back(std::move(future));

    threads.push_back(std::thread(calculate, std::move(calc)));
  }

  for (auto& f : futures) {
    try {
      std::cout << "Result = " << f.get() << "\n";
    } catch (const std::exception& e) {
      std::cout << "Calculation failed: " << e.what() << "\n";
    }
  }

  for (auto& t : threads) t.join();

  return 0;
}
