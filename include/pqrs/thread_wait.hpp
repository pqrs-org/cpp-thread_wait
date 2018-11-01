#pragma once

// (C) Copyright Takayama Fumihiko 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See http://www.boost.org/LICENSE_1_0.txt)

// `pqrs::thread_wait` can be used safely in a multi-threaded environment.

#include <memory>
#include <mutex>

namespace pqrs {
class thread_wait {
public:
  // We have to use shared_ptr to avoid SEGV from a spuriously wake.
  //
  // Note:
  //   wait::notify rarely causes SEGV unless we use shared_ptr in the following case.
  //
  //   1. `notify` set notify_ = true.
  //   2. `wait_notice` exits by spuriously wake.
  //   3. `wait` is destructed.
  //   4. `notify` calls `cv_.notify_one` with released `cv_`. (SEGV)
  //
  //   A bad example:
  //     ----------------------------------------
  //     {
  //       wait w;
  //       std::thread th([&w] {
  //         w.notify(); // `notify` rarely causes SEGV.
  //       })
  //       w.wait_notice();
  //     }
  //     ----------------------------------------
  //
  //   A good example:
  //     ----------------------------------------
  //     {
  //       auto w = make_wait();
  //       std::thread th([w] {
  //         w->notify();
  //       })
  //       w->wait_notice();
  //     }
  //     ----------------------------------------

  static std::shared_ptr<thread_wait> make_thread_wait(void) {
    struct impl : thread_wait {
      impl(void) : thread_wait() {}
    };
    return std::make_shared<impl>();
  }

  virtual ~thread_wait(void) {
  }

  void wait_notice(void) {
    std::unique_lock<std::mutex> lock(mutex_);

    cv_.wait(lock, [this] {
      return notify_;
    });
  }

  void notify(void) {
    {
      std::lock_guard<std::mutex> lock(mutex_);

      notify_ = true;
    }

    cv_.notify_one();
  }

private:
  thread_wait(void) : notify_(false) {
  }

  bool notify_;
  std::mutex mutex_;
  std::condition_variable cv_;
};

inline std::shared_ptr<thread_wait> make_thread_wait(void) {
  return thread_wait::make_thread_wait();
}
} // namespace pqrs
