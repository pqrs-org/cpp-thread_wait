#include <chrono>
#include <iostream>
#include <pqrs/thread_wait.hpp>
#include <thread>

int main() {
  int i = 0;

  // Create `pqrs::thread_wait`.
  auto w = pqrs::make_thread_wait();

  // Note: You must capture `pqrs::thread_wait` by copy.
  // (Do not capture by reference like [&w].)
  auto thread = std::thread([&i, w] {
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    i = 100;

    w->notify();
  });

  std::cout << i << std::endl; // 0

  // Wait until another thread calls `pqrs::thread_wait::notify`.
  w->wait_notice();

  std::cout << i << std::endl; // 100

  // You can also wait with a timeout.
  auto w2 = pqrs::make_thread_wait();
  auto notified = w2->wait_notice_for(std::chrono::seconds(1));
  std::cout << "notified: " << std::boolalpha << notified << std::endl; // false

  thread.join();

  return 0;
}
