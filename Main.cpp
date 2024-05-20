#include <iostream>
#include <thread>

auto main(int argc, char *argv[]) -> int {
  int sum = 0;
  std::cout << "sum = " << sum << "\n";

  auto f = [&sum]() {
    for (int i = 0; i < 100000; i++) {
      sum += 1;
    }
  };

  std::thread t1(f);
  std::thread t2(f);
  t1.join();
  t2.join();

  std::cout << "sum = " << sum << "\n";

  return 0;
}
