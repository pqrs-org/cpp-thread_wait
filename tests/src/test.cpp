#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include <chrono>
#include <pqrs/thread_wait.hpp>
#include <thread>

TEST_CASE("thread_wait") {
  {
    int count = 0;
    auto w = pqrs::make_thread_wait();
    auto t = std::thread([&count, w] {
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
      ++count;
      w->notify();
    });
    REQUIRE(count == 0);

    w->wait_notice();
    REQUIRE(count == 1);

    t.join();
  }
}

TEST_CASE("stress testing") {
  for (int i = 0; i < 10000; ++i) {
    auto w = pqrs::make_thread_wait();
    auto t = std::thread([w] {
      w->notify();
    });
    t.detach();
    w->wait_notice();
  }
}
