// Copyright 2024-2025 ECCI-UCR CC-BY-4
#include <iostream>

#include "Mpi.hpp"

int main(int argc, char *argv[]) {
  try {
    Mpi mpi(argc, argv);
    bool can_greet = false;
    if (mpi.rank() > 0) {
      mpi.receive(can_greet, mpi.rank() - 1);
    }
    std::cout << "Hello from main thread of process " << mpi.rank()
    << " of " << mpi.size() << " on " << mpi.getHostname() << std::endl;

    if (mpi.rank() < mpi.size() - 1) {
      mpi.send(can_greet, mpi.rank() + 1);
    }
  }
  catch (const std::exception &error) {
    std::cerr << "error: " << error.what() << std::endl; 
  }
}
