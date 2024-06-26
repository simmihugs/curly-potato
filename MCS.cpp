#include <atomic>
#include <iostream>
#include <thread>
#include <vector>

struct QNode {
  std::atomic<bool> wait;
  std::atomic<QNode *> next;
};

class AtomicMCSLock {
private:
  std::atomic<QNode *> tail;

  QNode *swap(std::atomic<QNode *> &_tail, QNode *_p) {
    return _tail.exchange(_p, std::memory_order_acq_rel);
  }

  bool cas(std::atomic<QNode *> &_tail, QNode *_expected, QNode *_desired) {
    return _tail.compare_exchange_weak(_expected, _desired,
                                       std::memory_order_acq_rel);
  }

public:
  AtomicMCSLock() {}
  AtomicMCSLock(const AtomicMCSLock &other,
                std::memory_order order = std::memory_order_seq_cst) {}

  void acquire(QNode *p) {
    p->next.store(nullptr);
    p->wait.store(true);

    QNode *prev = swap(tail, p);

    if (prev) {
      prev->next.store(p, std::memory_order_release);
      while (p->wait.load(std::memory_order_acquire)) {
        //
      }
    }
  }

  void release(QNode *p) {
    QNode *succ = p->next.load(std::memory_order_acquire);

    if (!succ) {
      auto desired = p;
      if (cas(tail, desired, nullptr)) {
        return;
      }
      do {
        succ = p->next.load(std::memory_order_acquire);
      } while (succ == nullptr);
    }
    succ->wait.store(false, std::memory_order_release);
  }
};

auto main() -> int {
  std::thread t1;
  std::thread t2;

  AtomicMCSLock mcs = AtomicMCSLock();
  QNode node;
  int global = 0;

  auto func = [&mcs, &global, &node]() {
    mcs.acquire(&node);
    for (auto i = 0; i < 100000000; ++i) {
      global += 1;
    }
    mcs.release(&node);
  };


  t1 = std::thread{func};
  t2 = std::thread{func};
  t1.join();
  t2.join();


  if (global == 2 * 100000000) {
      std::cout << "SUCCESS\n";      
  } else {
      std::cout << "FAILURE\n";      
  }

  return 0;
}
