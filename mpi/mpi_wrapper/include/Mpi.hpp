// Copyright 2024 ECCI-UCR CC-BY-4
#pragma once

#include <mpi.h>
#include <string>

#include <stdexcept>

class Mpi {
 private:
  int processNumber = -1;
  int processCount = -1;
  std::string hostname;

 public:
  class Error : public std::runtime_error {
   public:
    explicit Error(const std::string& message)
      : runtime_error(message) {
    }
    Error(const std::string& message, const Mpi& mpi)
      : runtime_error(mpi.getHostname() + ':' + std::to_string(mpi.rank())
      + ": " + message) {
    }
    Error(const std::string& message, const Mpi& mpi, const int threadNumber)
      : runtime_error(mpi.getHostname() + ':' + std::to_string(mpi.rank())
      + '.' + std::to_string(threadNumber) + ": " + message) {
    }
  };
  
 public:
  Mpi(int& argc, char**& argv) {
    if (MPI_Init(&argc, &argv) != MPI_SUCCESS) {
      throw Error("could not init MPI");
    }
    if (MPI_Comm_rank(MPI_COMM_WORLD, &this->processNumber) != MPI_SUCCESS) {
      throw Error("could not get MPI rank");
    }
    if (MPI_Comm_size(MPI_COMM_WORLD, &this->processCount) != MPI_SUCCESS) {
      throw Error("could not get MPI size");
    }
    char processHostname[MPI_MAX_PROCESSOR_NAME] = { '\0' };
    int hostnameLen = -1;
    if (MPI_Get_processor_name(processHostname, &hostnameLen) != MPI_SUCCESS) {
      throw Error("could not get MPI processor name");
    }
    this->hostname = processHostname;
  }
  Mpi(const Mpi&) = delete;
  Mpi(Mpi&&) = delete;
  ~Mpi() {
    MPI_Finalize();
  }
  Mpi& operator=(const Mpi&) = delete;
  Mpi& operator=(Mpi&&) = delete;

 public:  // Accessors
  inline int getProcessNumber() const {
    return this->processNumber;
  }
  inline int getProcessCount() const {
    return this->processCount;
  }
  inline const std::string& getHostname() const {
    return this->hostname;
  }
  inline int rank() const {
    return this->getProcessNumber();
  }
  inline int size() const {
    return this->getProcessCount();
  }
};
