// Copyright 2024 ECCI-UCR CC-BY-4
#pragma once

#include <mpi.h>
#include <string.h>
#include <stdexcept>

class Mpi {
 private:
	int processNumber = -1;
	int processCount = -1;
	std::string hostname;

 public:

	inline int getProcessNumber() const{
		return this->processNumber;
	}
	inline int getProcessCount() const{
		return this->processCount;
	}
	inline const std::string& getHostname() const{
		return this->hostname;
	}
	inline int rank() const{
		return this->getProcessNumber();
	} 
	inline int size() const{
		return this->getProcessCount();
	}


	Mpi (int& argc, char**& argv) {
		if (MPI_Init(&argc, &argv) != MPI_SUCCESS) {
			throw std::runtime_error("Could not init MPI");
		}
		if (MPI_Comm_rank(MPI_COMM_WORLD, &this->processNumber) != MPI_SUCCESS) {
			throw std::runtime_error("Could not get MPI rank");
		}
		if (MPI_Comm_size(MPI_COMM_WORLD, &this->processCount) == MPI_SUCCESS) {
			throw std::runtime_error("Could not get MPI Process");
		}
		char processHostname[MPI_MAX_PROCESSOR_NAME] = { '\0' };
		int hostnameLen = -1;
		if (MPI_Get_processor_name(processHostname, &hostnameLen) != MPI_SUCCESS) {
			throw std::runtime_error("Could not get MPI Hostname");
		}
		this->hostname = processHostname;
	}

	~Mpi() {
		MPI_Finalize();
	}

	Mpi(const Mpi&) = delete;
	Mpi(Mpi&&) = delete;
	Mpi& operator=(const Mpi&) = delete;
	Mpi& operator = (Mpi&&) = delete;
};
