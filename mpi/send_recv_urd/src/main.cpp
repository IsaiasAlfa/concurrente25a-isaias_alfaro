// Copyright 2024-2025 ECCI-UCR CC-BY-4
#include <iostream>
#include <sstream>

#include "Mpi.hpp"

// 70 chars for "Hello from main thread of process <i> of <w> on <hostname>"
constexpr size_t MAX_MSG_LEN = MPI_MAX_PROCESSOR_NAME + 70;

int main(int argc, char* argv[]) {
  try {
    Mpi mpi(argc, argv);


    std::stringstream message;
    message << "Hello from main thread of process " << mpi.rank()
    << " of " << mpi.size() << " on " << mpi.getHostname();

    if (mpi.rank() > 0) {
      mpi.send(message.str(), 0);
    } else {
      std::cout << message.str() << std::endl;
      for (int source = 1; source < mpi.size(); ++source) {
        std::string buffer;
        mpi.receive(buffer, MAX_MSG_LEN);
        std::cout << buffer << std::endl;
      }
    }
  } catch (const std::exception& error) {
    std::cerr << "error: " << error.what() << std::endl;
  }
}

