#include <chrono>
#include <iostream>
#include <pqrs/thread_wait.hpp>
#include <thread>

int main(void) {
  int i = 0;
  auto w = pqrs::make_thread_wait();

  auto thread = std::thread([&i, w] {
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    i = 100;
    w->notify();
  });

  std::cout << i << std::endl; // 0

  w->wait_notice();

  std::cout << i << std::endl; // 500

  thread.join();

  return 0;
}
