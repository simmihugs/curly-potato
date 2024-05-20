#include <atomic>
#include <iostream>
#include <thread>
#include <vector>

class PetersonLock {
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

auto main(int argc, char *argv[]) -> int {
  int global_int = 0;
  auto lock = PetersonLock();
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
  std::cout << "global_int = " << global_int << "\n";

  return 0;
}
