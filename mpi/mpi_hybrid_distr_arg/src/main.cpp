// Copyright 2024 ECCI-UCR CC-BY-4
#include <iostream>

#include <omp.h>
#include <algorithm>
#include <cstdlib>
#include <iostream>

#include "Mpi.hpp"

int calculate_start(int rank, int end, int workers, int begin);
int calculate_finish(int rank, int end, int workers, int begin);

int main(int argc, char* argv[]) {
  try {
    Mpi mpi(argc, argv);
    if (argc == 3) {
      const int overall_start = atoi(argv[1]);
      const int overall_finish = atoi(argv[2]);

      const int process_start = calculate_start(process_number, overall_finish
        , process_count, overall_start);
      const int process_finish = calculate_finish(process_number, overall_finish
        , process_count, overall_start);
      const int process_size = process_finish - process_start;

      std::cout << mpi.getHostname() << ':' << mpi.getProcessNumber()
          << ": range [" << process_start << ", " << process_finish
          << "[ size " << process_size << std::endl;

      int thread_start = -1;
      int thread_finish = -1;
      const int thread_size = thread_finish - thread_start;

      std::cout << '\t' << mpi.getHostname() << ':' << mpi.getProcessNumber()
          << '.' << omp_get_thread_num() << ": range [" << thread_start << ", "
          << thread_finish << "[ size " << thread_size << std::endl;
    } else {
      std::cerr << "usage: hybrid_distr_arg start finish" << std::endl;
    }
  } catch (const std::exception& error) {
    std::cerr << "error: " << error.what() << std::endl;
  }
  return 0;
}

int calculate_start(int rank, int end, int workers, int begin) {
  // ...
}

int calculate_finish(int rank, int end, int workers, int begin) {
  return calculate_start(rank + 1, end, workers, begin);
}
