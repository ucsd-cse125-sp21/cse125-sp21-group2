#pragma once

#include <cassert>
#include <condition_variable>
#include <mutex>
#include <thread>

// https://stackoverflow.com/questions/11666610/how-to-give-priority-to-privileged-thread-in-mutex-locking
class PriorityMutex {
  std::condition_variable cv_;
  std::mutex gate_;
  bool locked_;
  std::thread::id pr_tid_;  // priority thread
 public:
  PriorityMutex() : locked_(false) {}
  ~PriorityMutex() { assert(!locked_); }
  PriorityMutex(PriorityMutex&) = delete;
  PriorityMutex operator=(PriorityMutex&) = delete;

  void lock(bool privileged = false) {
    const std::thread::id tid = std::this_thread::get_id();
    std::unique_lock<decltype(gate_)> lk(gate_);
    if (privileged) pr_tid_ = tid;
    cv_.wait(lk, [&] {
      return !locked_ && (pr_tid_ == std::thread::id() || pr_tid_ == tid);
    });
    locked_ = true;
  }

  void unlock() {
    std::lock_guard<decltype(gate_)> lk(gate_);
    if (pr_tid_ == std::this_thread::get_id()) pr_tid_ = std::thread::id();
    locked_ = false;
    cv_.notify_all();
  }

  bool isLocked() { return locked_; }
};
