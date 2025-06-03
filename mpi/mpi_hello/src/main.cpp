// Copyright 2024 ECCI-UCR CC-BY-4
#include <mpi.h>
#include <iostream>

int main(int argc, char* argv[]) {
  if (MPI_Init(&argc, &argv) == MPI_SUCCESS) {
    int process_number = -1;
    if (MPI_Comm_rank(MPI_COMM_WORLD, &process_number) == MPI_SUCCESS) {
      int process_count = -1;
      if (MPI_Comm_size(MPI_COMM_WORLD, &process_count) == MPI_SUCCESS) {
        char process_hostname[MPI_MAX_PROCESSOR_NAME] = { '\0' };
        int hostname_len = -1;
        if (MPI_Get_processor_name(process_hostname, &hostname_len) == MPI_SUCCESS) {
          std::cout << "Hello from main thread of process " << process_number
              << " of " << process_count << " on " << process_hostname << std::endl;
        } else {
          std::cerr << "error: could not get MPI processor name (hostname)\n";
        }
      } else {
        std::cerr << "error: could not get MPI size (process count)\n";
      }
    } else {
      std::cerr << "error: could not get MPI rank (process number)\n";
    }
    MPI_Finalize();
  } else {
    std::cerr << "error: could not init MPI\n";
  }
}
