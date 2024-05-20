#include <atomic>
#include <iomanip>
#include <iostream>
#include <thread>
#include <tuple>
#include <vector>

// naive
struct NaiveQNode {
  bool wait;
  NaiveQNode *next;
  NaiveQNode() : wait(false), next(nullptr) {}
};

class NaiveMCSLock {
private:
  NaiveQNode *tail = nullptr;

  NaiveQNode *swap(NaiveQNode **a, NaiveQNode *b) {
    NaiveQNode *old = *a;
    *a = b;
    return old;
  }

  bool cas(NaiveQNode **a, NaiveQNode *expected, NaiveQNode *desired) {
    bool t = *a == expected;
    if (t) {
      *a = desired;
    }
    return t;
  }

public:
  void acquire(NaiveQNode *p) {
    NaiveQNode *prev = this->swap(&tail, p);
    if (prev != NULL) {
      prev->next = p;
      while (p->wait) {
        // spin
      }
    }
    std::cout << "Aquired\n";
  }

  void release(NaiveQNode *p) {
    NaiveQNode *succ = p->next;
    while (succ == NULL) {
      if (this->cas(&tail, p, NULL)) {
        return;
      } else {
        succ = p->next;
      }
    }
    succ->wait = false;
    std::cout << "Released\n";
  }
};

struct AtomicQNode {
  std::atomic<bool> wait;
  std::atomic<AtomicQNode *> next;
};

class AtomicMCSLock {
private:
  std::atomic<AtomicQNode *> tail;

  AtomicQNode *swap(std::atomic<AtomicQNode *> &_tail, AtomicQNode *_p) {
    return _tail.exchange(_p, std::memory_order_acq_rel);
  }

  bool cas(std::atomic<AtomicQNode *> &_tail, AtomicQNode *_expected,
           AtomicQNode *_desired) {
    return _tail.compare_exchange_weak(_expected, _desired,
                                       std::memory_order_acq_rel);
  }

public:
  void acquire(AtomicQNode *p) {
    p->next.store(nullptr);
    p->wait.store(true);

    AtomicQNode *prev = swap(tail, p);

    if (prev) {
      prev->next.store(p, std::memory_order_release);
      while (p->wait.load(std::memory_order_acquire)) {
        //
      }
    }
    std::cout << "acquired\n";
  }

  void release(AtomicQNode *p) {
    AtomicQNode *succ = p->next.load(std::memory_order_acquire);

    if (!succ) {
      if (cas(tail, p, nullptr)) {
        return;
      }
      do {
        succ = p->next.load(std::memory_order_acquire);
      } while (succ == nullptr);
    }
    succ->wait.store(false, std::memory_order_release);
    std::cout << "released\n";
  }
};

auto main() -> int {
  // Naive
  {
    std::cout << "Naive\n";
    std::thread t1;
    std::thread t2;

    NaiveMCSLock lock = NaiveMCSLock();
    int global = 0;

    auto func = [&lock, &global]() {
      NaiveQNode node = NaiveQNode();
      lock.acquire(&node);
      for (auto i = 0; i < 100000000; ++i) {
        global += 1;
      }
      lock.release(&node);
    };

    std::cout << "global = " << global << "\n";
    t1 = std::thread{func};
    t2 = std::thread{func};
    t1.join();
    t2.join();
    std::cout << "global = " << global << "\n";
    std::cout << "\n\n\n";
  }

  // Atomic
  {
    std::cout << "Atomic\n";
    std::thread t1;
    std::thread t2;

    AtomicMCSLock mcs = AtomicMCSLock();
    AtomicQNode node;
    int global = 0;

    auto func = [&mcs, &global, &node]() {
      mcs.acquire(&node);
      for (auto i = 0; i < 100000000; ++i) {
        global += 1;
      }
      mcs.release(&node);
    };

    std::cout << "global = " << global << "\n";
    t1 = std::thread{func};
    t2 = std::thread{func};
    t1.join();
    t2.join();
    std::cout << "global = " << global << "\n";
    std::cout << "\n\n\n";
  }

  return 0;
}
