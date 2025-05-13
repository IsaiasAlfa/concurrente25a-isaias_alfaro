// Copyright 2024 ECCI-UCR CC-BY 4.0
#include <iostream>

int main(int argc, char* argv[]) {
  const int thread_count = argc == 2 ? ::atoi(argv[1]) : 1;

  std::cout << "Hello from secondary thread " << 0
    << " of " << thread_count << std::endl;
}
