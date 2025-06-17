// Copyright 2024-2025 ECCI-UCR CC-BY-4
#include <iostream>

#include "Mpi.hpp"

// 70 chars for "Hello from main thread of process <i> of <w> on <hostname>"
constexpr size_t MAX_MSG_LEN = MPI_MAX_PROCESSOR_NAME + 70;

int main(int argc, char* argv[]) {
  try {
    Mpi mpi(argc, argv);
    std::cout << "Hello from main thread of process " << mpi.rank()
        << " of " << mpi.size() << " on " << mpi.getHostname() << std::endl;
  } catch (const std::exception& error) {
    std::cerr << "error: " << error.what() << std::endl;
  }
}
