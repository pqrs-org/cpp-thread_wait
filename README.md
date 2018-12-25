[![Build Status](https://travis-ci.org/pqrs-org/cpp-thread_wait.svg?branch=master)](https://travis-ci.org/pqrs-org/cpp-thread_wait)
[![License](https://img.shields.io/badge/license-Boost%20Software%20License-blue.svg)](https://github.com/pqrs-org/cpp-thread_wait/blob/master/LICENSE.md)

# cpp-thread_wait

A wrapper of `std::condition_variable::wait`.

You can wait another thread by using the following methods.

- `pqrs::thread_wait::wait_notice`
- `pqrs::thread_wait::notify`

## Install

### Using package manager

You can install `include/pqrs` by using [cget](https://github.com/pfultz2/cget).

```shell
cget install pqrs-org/cget-recipes
cget install pqrs-org/cpp-thread_wait
```

### Manual install

Copy `include/pqrs` directory into your include directory.

## Usage

```cpp
#include <chrono>
#include <iostream>
#include <pqrs/thread_wait.hpp>
#include <thread>

int main(void) {
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

  std::cout << i << std::endl; // 500

  thread.join();

  return 0;
}
```
