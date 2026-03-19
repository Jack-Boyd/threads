#include <iostream>
#include <mutex>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

struct BankAccount
{
  std::string id;
  double balance;
  std::mutex mtx;
  BankAccount(std::string _id, double _balance) : id(_id), balance(_balance) {}
};

std::mutex cout_mtx;

void transfer(BankAccount& from, BankAccount& to, float amount)
{
  if (&from == &to) {
    std::lock_guard<std::mutex> out_lock(cout_mtx);
    std::cout << "Skipping self-transfer for " << from.id << '\n';
    return;
  }
  
  try {
    std::scoped_lock lock(from.mtx, to.mtx);

    {
      std::lock_guard<std::mutex> out_lock(cout_mtx);
      std::cout << from.id << "($" << from.balance << ") paying $" << amount << " to " << to.id << "($" << to.balance << ")\n";
    }

    if (from.balance < amount) {
      throw std::runtime_error("Insufficient funds");
    }

    from.balance -= amount;
    to.balance += amount;

    {
      std::lock_guard<std::mutex> out_lock(cout_mtx);
      std::cout << from.id << "($" << from.balance << ") paid $" << amount << " to " << to.id << "($" << to.balance << ") successfully\n";
    }
  } catch (const std::exception& e) {
    std::lock_guard<std::mutex> out_lock(cout_mtx);
    std::cout << "Transfer from " << from.id << " to " << to.id << " failed: " << e.what() << '\n';
  }
}

int main() 
{
  BankAccount account1("Alan", 140.41), account2("Ben", 134.21), account3("Josh", 249302.44), account4("Zach", 33219.01);

  float initial_total = account1.balance + account2.balance + account3.balance + account4.balance;

  std::vector<std::thread> threads;
  threads.emplace_back(transfer, std::ref(account1), std::ref(account2), 100.45);
  threads.emplace_back(transfer, std::ref(account3), std::ref(account1), 4450.95);
  threads.emplace_back(transfer, std::ref(account2), std::ref(account1), 20.0);
  threads.emplace_back(transfer, std::ref(account1), std::ref(account3), 14.24);
  threads.emplace_back(transfer, std::ref(account4), std::ref(account3), 2409.04);
  threads.emplace_back(transfer, std::ref(account3), std::ref(account4), 8042.02);
  threads.emplace_back(transfer, std::ref(account3), std::ref(account1), 33.52);
  threads.emplace_back(transfer, std::ref(account3), std::ref(account1), 1241.10);

  for (auto& t : threads) {
    t.join();
  }

  {
    std::scoped_lock lock(account1.mtx, account2.mtx, account3.mtx,
                          account4.mtx);
    std::cout << "\nFinal balances:\n";
    std::cout << account1.id << ": $" << account1.balance << '\n';
    std::cout << account2.id << ": $" << account2.balance << '\n';
    std::cout << account3.id << ": $" << account3.balance << '\n';
    std::cout << account4.id << ": $" << account4.balance << '\n';
  }

  float final_total = account1.balance + account2.balance + account3.balance + account4.balance;

  std::cout << "\nInitial total: $" << initial_total << '\n';
  std::cout << "Final total:   $" << final_total << '\n';

  return 0;
}
