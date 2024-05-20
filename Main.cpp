#include <atomic>
#include <iostream>
#include <thread>

class AtomicPetersonLock {
private:
  int turn = 0;
  bool interested[2] = {false, false};

public:
  void aquire(int thread_id) {
    int other = 1 - thread_id;
    interested[thread_id] = true;
    turn = other;
    while (interested[other] && turn == other) {
      std::cout << "thread: " << thread_id << " spin\n";
    }
  }

  void release(int thread_id) { interested[thread_id] = false; }
};

auto main(int argc, char *argv[]) -> int {

  return 0;
}
