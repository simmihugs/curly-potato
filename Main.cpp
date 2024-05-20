#include <iostream>
#include <thread>

auto main(int argc, char *argv[]) -> int {
  int global = 10000;
  std::cout << "global: " << global << "\n";
  auto func = [global]() mutable {
    std::cout << "global: " << global << "\n";
    int tmp = global;
    tmp += 10000;
    global = tmp;
    std::cout << "global: " << global << "\n";
  };

  std::thread t1{func};
  std::thread t2{func};
  t1.join();
  t2.join();

  std::cout << "global: " << global << "\n";
  return 0;
}
