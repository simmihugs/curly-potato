#include <atomic>
#include <iomanip>
#include <iostream>
#include <thread>
#include <vector>

class AngabePetersonLock {
private:
  int turn = 0;
  bool interested[2] = {false, false};

public:
  void aquire(int thread_id) {
    int other = 1 - thread_id;
    interested[thread_id] = true;
    turn = other;

    int spins = 0;
    while (interested[other] && turn == other) {
      if (spins++ % 10000000 == 0) {
        std::cout << "thread" << thread_id << " spined for " << spins
                  << " spins\n";
      }
    }
    std::cout << "aqurired by thread" << thread_id << "\n";
  }

  void release(int thread_id) {
    std::cout << "released by thread" << thread_id << "\n";
    interested[thread_id] = false;
  }
};

class AtomicPetersonLock {
private:
  std::atomic<int> turn;
  std::atomic<bool> interested[2];

public:
  AtomicPetersonLock() {
    turn.store(0, std::memory_order_release);
    interested[0].store(false, std::memory_order_release);
    interested[1].store(false, std::memory_order_release);
  }

  ~AtomicPetersonLock() {}

  void aquire(int thread_id) {
    int other = 1 - thread_id;
    interested[thread_id].store(true, std::memory_order_acquire);
    turn = other;

    int spins = 0;
    while (interested[other].load(std::memory_order_acquire) &&
           turn.load(std::memory_order_acquire) == other) {
      if (spins++ % 10000000 == 0) {
        // std::cout << "thread" << thread_id << " spined for " << spins << "
        // spins\n";
      }
    }
    // std::cout << "aqurired by thread" << thread_id << "\n";
  }

  void release(int thread_id) {
    // std::cout << "released by thread" << thread_id << "\n";
    interested[thread_id].store(false, std::memory_order_release);
  }
};

void test_angabe_peterson_lock(void) {
  std::cout << "\n\nAngabe PetersonLock: \n";
  int global_int = 0;
  auto lock = AngabePetersonLock();
  auto func = [&global_int, &lock](int thread_id) {
    lock.aquire(thread_id);
    for (auto i = 0; i < 100000000; ++i) {
      global_int += 1;
    }
    lock.release(thread_id);
  };

  std::cout << "global_int = " << global_int << "\n";
  std::thread t1;
  std::thread t2;
  t1 = std::thread{func, 0};
  t2 = std::thread{func, 1};
  t1.join();
  t2.join();
  std::cout << "global_int = " << global_int << "\n\n\n";
}

void test_atomic_peterson_lock(void) {
  int global_int = 0;
  AtomicPetersonLock lock = AtomicPetersonLock();

  std::cout << "\n\nAtomic PetersonLock: \n";
  auto func = [&global_int, &lock](int thread_id) {
    lock.aquire(thread_id);
    for (auto i = 0; i < 100000000; ++i) {
      global_int += 1;
    }
    lock.release(thread_id);
  };
  auto func_without_lock = [&global_int](int thread_id) {
    for (auto i = 0; i < 100000000; ++i) {
      global_int += 1;
    }
  };

  std::cout << "Compare with lock vs without vs should be:\n";
  std::cout << "| Atomic Peterson Lock |  No Lock              | Expected "
               "value        |\n";
  for (auto i = 0; i < 10; ++i) {
    global_int = 0;
    {
      std::thread t1;
      std::thread t2;
      t1 = std::thread{func, 0};
      t2 = std::thread{func, 1};
      t1.join();
      t2.join();
    }
    int result = global_int;
    global_int = 0;
    {
      std::thread t1;
      std::thread t2;
      t1 = std::thread{func_without_lock, 0};
      t2 = std::thread{func_without_lock, 1};
      t1.join();
      t2.join();
    }
    std::cout << "| " << result << std::setw(14) << " | ";
    std::cout << global_int << std::setw(15) << " | ";
    std::cout << 200000000 << std::setw(15) << " |\n";
  }
}

void test_without_peterson_lock(void) {
  std::cout << "\n\nWithout (any) PetersonLock: \n";
  int global_int = 0;

  auto func = [&global_int](int thread_id) {
    for (auto i = 0; i < 100000000; ++i) {
      global_int += 1;
    }
  };

  std::cout << "global_int = " << global_int << "\n";
  std::thread t1;
  std::thread t2;
  t1 = std::thread{func, 0};
  t2 = std::thread{func, 1};
  t1.join();
  t2.join();
  std::cout << "global_int = " << global_int << "\n\n\n";
}

auto main(int argc, char *argv[]) -> int {
  // test_angabe_peterson_lock();
  test_atomic_peterson_lock();
  // test_without_peterson_lock();
  return 0;
}
