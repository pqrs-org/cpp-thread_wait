#include <array>
#include <atomic>
#include <boost/ut.hpp>
#include <chrono>
#include <pqrs/thread_wait.hpp>
#include <thread>
#include <type_traits>

static_assert(std::is_final_v<pqrs::thread_wait>);
static_assert(!std::is_default_constructible_v<pqrs::thread_wait>);
static_assert(!std::is_copy_constructible_v<pqrs::thread_wait>);
static_assert(!std::is_move_constructible_v<pqrs::thread_wait>);
static_assert(!std::is_copy_assignable_v<pqrs::thread_wait>);
static_assert(!std::is_move_assignable_v<pqrs::thread_wait>);

int main() {
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

  "thread_wait (notify before wait)"_test = [] {
    auto w = pqrs::make_thread_wait();

    w->notify();
    w->wait_notice();
  };

  "thread_wait (multiple waiters)"_test = [] {
    {
      auto w = pqrs::make_thread_wait();
      std::array<std::atomic_bool, 2> completed{};

      auto notifier = std::thread([w] {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        w->notify();
      });

      auto waiter1 = std::thread([&completed, w] {
        w->wait_notice();
        completed[0].store(true, std::memory_order_release);
      });

      auto waiter2 = std::thread([&completed, w] {
        w->wait_notice();
        completed[1].store(true, std::memory_order_release);
      });

      notifier.join();
      waiter1.join();
      waiter2.join();

      expect(completed[0].load(std::memory_order_acquire));
      expect(completed[1].load(std::memory_order_acquire));
    }
  };

  "thread_wait (noexcept)"_test = [] {
    auto w = pqrs::make_thread_wait();

    expect(noexcept(w->notify()));
    expect(noexcept(w->wait_notice()));
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
