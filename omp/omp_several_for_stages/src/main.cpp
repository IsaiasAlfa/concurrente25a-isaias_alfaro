// Copyright 2024 ECCI-UCR CC-BY 4.0
#include <omp.h>
#include <iostream>

int main(int argc, char* argv[]) {
  const int thread_count = argc >= 2 ? ::atoi(argv[1]) : omp_get_num_procs();
  const int iteration_count = argc >= 3 ? ::atoi(argv[2]) : thread_count;
  // const int stage_count = argc >= 4 ? ::atoi(argv[3]) : thread_count;

  #pragma omp parallel num_threads(thread_count) \
    default(none) shared(std::cout, iteration_count) firstprivate(thread_count)
  {  // NOLINT(whitespace/braces)
    #pragma omp for schedule(runtime)
    for (int iteration = 0; iteration < iteration_count; ++iteration) {
      #pragma omp critical(stdout)
      std::cout << "stage 1: " << omp_get_thread_num() << '/'
        << omp_get_num_threads() << ": iteration " << iteration << '/'
        << iteration_count << std::endl;
    }  // Tiene barreras implicitas al final

    #pragma omp single
    std::cout << /*omp_get_thread_num() <<*/ std::endl;

    #pragma omp for schedule(runtime)
    for (int iteration = 0; iteration < iteration_count; ++iteration) {
      #pragma omp critical(stdout)
      std::cout << "stage 2: " << omp_get_thread_num() << '/'
        << omp_get_num_threads() << ": iteration " << iteration << '/'
        << iteration_count << std::endl;
    }

    #pragma omp single
    std::cout << /*omp_get_thread_num() <<*/ std::endl;

    #pragma omp for schedule(runtime)
    for (int iteration = 0; iteration < iteration_count; ++iteration) {
      #pragma omp critical(stdout)
      std::cout << "stage 3: " << omp_get_thread_num() << '/'
        << omp_get_num_threads() << ": iteration " << iteration << '/'
        << iteration_count << std::endl;
    }
  }
}

// void stages3(const int thread_count, const int iteration_count) {
// }
