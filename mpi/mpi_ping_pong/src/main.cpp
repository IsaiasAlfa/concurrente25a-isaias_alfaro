// Copyright 2024-2025 ECCI-UCR CC-BY-4
#include <unistd.h>
#include <iostream>

#include "Mpi.hpp"

void play_ping_pong(Mpi& mpi, const long delay);

int main(int argc, char* argv[]) {
  try {
    Mpi mpi(argc, argv);
    if (mpi.size() == 2) {
      const long delay = argc >= 2 ? std::stol(argv[1]) : 500;  // milliseconds
      play_ping_pong(mpi, delay);
    } else if (mpi.rank() == 0) {
      std::cerr << "error: to much players" << std::endl;
      std::cerr << "usage mpiexec -np 2"<< std::endl;
    }
  } catch (const Mpi::Error& error) {
    std::cerr << "error: " << error.what() << std::endl;
  }
  return 0;
}

void play_ping_pong(Mpi& mpi, const long delay) {
  long ball = 0;
  int partner = 0;

  if (mpi.rank() == 0) {
    std::cout << mpi.rank() << " serves" << ball++ <<std::endl;
    mpi.send(ball, mpi.rank() + 1);
    partner = mpi.rank() + 1;
  }
  while (true) {
    mpi.receive(ball, partner);
    ball++;
    std::cout << mpi.rank() << " serves" << ball <<std::endl;
    usleep(1000 * delay);
    mpi.send(ball, partner);
  }
}
