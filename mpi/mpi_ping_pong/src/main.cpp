// Copyright 2024-2025 ECCI-UCR CC-BY-4
#include <unistd.h>
#include <iostream>

#include "Mpi.hpp"

void play_ping_pong(Mpi& mpi);

int main(int argc, char* argv[]) {
  try {
    Mpi mpi(argc, argv);
    const long delay = argc >= 2 ? std::stol(argv[1]) : 500;  // milliseconds
    play_ping_pong(mpi, delay);
  } catch (const Mpi::Error& error) {
    std::cerr << "error: " << error.what() << std::endl;
  }
  return 0;
}

void play_ping_pong(Mpi& mpi, const long delay) {
  while (true) {
    std::cout << mpi.rank() << " serves" << std::endl;
    usleep(1000 * delay);
  }
}
