// Copyright 2024 ECCI-UCR CC-BY-4

#include <unistd.h>
#include <iostream>

#include "Mpi.hpp"

void relay_race(Mpi& mpi, const int stage1_delay, const int stage2_delay);
void run_stage1(Mpi& mpi, int stage1_delay);
void run_stage2(Mpi& mpi, int stage2_delay);

int main(int argc, char* argv[]) {
  try {
    Mpi mpi(argc, argv);
    if (mpi.size() >= 3 && argc == 3) {
      relay_race(mpi, atoi(argv[1]), atoi(argv[2]));
    } else if (mpi.rank() == 0) {
      std::cerr << "usage: mpiexec -np n relay_race stage1_msec stage2_msec\n"
        << "n must be odd and greater or equals to 3\n";
    }
  } catch (const Mpi::Error& error) {
    std::cerr << "error: " << error.what() << std::endl;
  }
  return 0;
}

void relay_race(Mpi& mpi, const int stage1_delay, const int stage2_delay) {
  run_stage1(mpi, stage1_delay);
  run_stage2(mpi, stage2_delay);
}

void run_stage1(Mpi& mpi, int stage1_delay) {
  (void)mpi;
  usleep(1000 * stage1_delay);
}

void run_stage2(Mpi& mpi, int stage2_delay) {
  (void)mpi;
  usleep(1000 * stage2_delay);
}
