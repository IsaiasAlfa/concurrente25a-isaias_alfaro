// Copyright 2024 ECCI-UCR CC-BY 4.0
#include <iostream>

#include <omp.h>

void heavy_task() {
}


int main(int argc, char* argv[]) {
  const int thread_count = argc == 2 ? ::atoi(argv[1]) : omp_get_max_threads();

  #pragma omp parallel num_threads(thread_count)
  {
  heavy_task();
  #pragma omp critical(stdout)
  std::cout << "Hello from secondary thread " << omp_get_thread_num()
    << " of " << omp_get_num_threads() << std::endl;
  }

  std::cout << "Hello from main thread " << std::endl;
}
