#include <boost/ut.hpp>
#include <chrono>
#include <pqrs/thread_wait.hpp>
#include <thread>

int main(void) {
  using namespace boost::ut;
  using namespace boost::ut::literals;

  "thread_wait"_test = [] {
    {
      int count = 0;
      auto w = pqrs::make_thread_wait();
      auto t = std::thread([&count, w] {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        ++count;
        w->notify();
      });
      expect(count == 0);

      w->wait_notice();
      expect(count == 1);

      t.join();
    }
  };

  "stress testing"_test = [] {
    for (int i = 0; i < 10000; ++i) {
      auto w = pqrs::make_thread_wait();
      auto t = std::thread([w] {
        w->notify();
      });
      t.detach();
      w->wait_notice();
    }
  };

  return 0;
}
