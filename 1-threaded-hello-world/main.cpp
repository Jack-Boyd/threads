#include <iostream>
#include <thread>

void printNumber(int number) 
{
  std::cout << "Hello from number thread! " 
            << number 
            << " - " 
            << std::this_thread::get_id() 
            << "\n";
}

void worker() 
{
  std::cout << "Hello from a thread! - " 
            << std::this_thread::get_id() 
            << "\n";
}

int main() 
{
  std::thread t(worker);
  std::thread t2(worker);
  std::thread t3(print_number, 5);
  std::thread t4(print_number, 20);
  std::thread t5([]() {
    std::cout << "Hello from a lambda thread! - "
              << std::this_thread::get_id()
              << "\n";
  });

  std::cout << "Hello from main!" << "\n";

  t.join();
  t2.join();
  t3.join();
  t4.join();
  t5.join();
  std::cout << "Done!" << "\n";
  return 0;
}
