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
    int overall_start = -1;
    int overall_finish = -1;
    if (argc == 3) {
      overall_start = atoi(argv[1]);
      overall_finish = atoi(argv[2]);
    } else {
      if (mpi.rank() == 0) {
        std::cin >> overall_start >> overall_finish;
        for (int destination = 1; destination < mpi.size(); ++destination) {
          if (MPI_Send(&overall_start, /*count*/ 1, MPI_INT, destination,
            /*tag*/ 0, MPI_COMM_WORLD) != MPI_SUCCESS) {
            throw Mpi::Error("could not send start", mpi);
          }
          if (MPI_Send(&overall_finish, /*count*/ 1, MPI_INT, destination,
            /*tag*/ 0, MPI_COMM_WORLD) != MPI_SUCCESS) {
            throw Mpi::Error("could not send finish", mpi);
          }
        }
      } else {
        if (MPI_Recv(&overall_start, /*count*/ 1, MPI_INT, /*source*/ 0,
          /*tag*/ 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE) != MPI_SUCCESS)  {
          throw Mpi::Error("could not receive start", mpi);
        }
        if (MPI_Recv(&overall_finish, /*count*/ 1, MPI_INT, /*source*/ 0,
          /*tag*/ 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE) != MPI_SUCCESS)  {
          throw Mpi::Error("could not receive finish", mpi);
        }
      }
    }

    const int process_start = calculate_start(mpi.rank(), overall_finish
      , mpi.size(), overall_start);
    const int process_finish = calculate_finish(mpi.rank(), overall_finish
      , mpi.size(), overall_start);
    const int process_size = process_finish - process_start;

    std::cout << mpi.getHostname() << ':' << mpi.getProcessNumber()
        << ": range [" << process_start << ", " << process_finish
        << "[ size " << process_size << std::endl;

    #pragma omp parallel default(none) \
      shared(process_start, process_finish, std::cout, mpi)
    {  // NOLINT(?)
      int thread_start = -1;
      int thread_finish = -1;

      #pragma omp for
      for (int index = process_start; index < process_finish; ++index) {
        // do_task(index);
        if (thread_start == -1) {
          thread_start = index;
        }
        thread_finish = index + 1;
      }

      const int thread_size = thread_finish - thread_start;

      #pragma omp critical(print)
      std::cout << '\t' << mpi.getHostname() << ':' << mpi.getProcessNumber()
          << '.' << omp_get_thread_num() << ": range [" << thread_start << ", "
          << thread_finish << "[ size " << thread_size << std::endl;
    }
  } catch (const std::exception& error) {
    std::cerr << "error: " << error.what() << std::endl;
  }
  return 0;
}

int calculate_start(int rank, int end, int workers, int begin) {
  const int range = end - begin;
  return begin + rank * (range / workers) + std::min(rank, range % workers);
}

int calculate_finish(int rank, int end, int workers, int begin) {
  return calculate_start(rank + 1, end, workers, begin);
}
