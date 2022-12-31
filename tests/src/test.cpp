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

  "thread_wait (multiple thread)"_test = [] {
    {
      auto w = pqrs::make_thread_wait();
      auto t1 = std::thread([w] {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        w->notify();
      });

      auto t2 = std::thread([w] {
        std::cout << "waiting (t2)" << std::endl;
        w->wait_notice();
        std::cout << "done (t2)" << std::endl;
      });

      auto t3 = std::thread([w] {
        std::cout << "waiting (t3)" << std::endl;
        w->wait_notice();
        std::cout << "done (t3)" << std::endl;
      });

      t1.join();
      t2.join();
      t3.join();
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
