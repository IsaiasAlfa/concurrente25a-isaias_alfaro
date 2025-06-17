// Copyright 2024 ECCI-UCR CC-BY-4
#pragma once

#include <mpi.h>
#include <string>
#include <vector>

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

 public:
  static inline MPI_Datatype map(bool) { return MPI_C_BOOL; }
  static inline MPI_Datatype map(char) { return MPI_CHAR; }
  static inline MPI_Datatype map(unsigned char) { return MPI_UNSIGNED_CHAR; }
  static inline MPI_Datatype map(short) { return MPI_SHORT; }
  static inline MPI_Datatype map(unsigned short) { return MPI_UNSIGNED_SHORT; }
  static inline MPI_Datatype map(int) { return MPI_INT; }
  static inline MPI_Datatype map(unsigned) { return MPI_UNSIGNED; }
  static inline MPI_Datatype map(long) { return MPI_LONG; }
  static inline MPI_Datatype map(unsigned long) { return MPI_UNSIGNED_LONG; }
  static inline MPI_Datatype map(long long) { return MPI_LONG_LONG; }
  static inline MPI_Datatype map(unsigned long long) {
    return MPI_UNSIGNED_LONG_LONG;
  }
  static inline MPI_Datatype map(float) { return MPI_FLOAT; }
  static inline MPI_Datatype map(double) { return MPI_DOUBLE; }
  static inline MPI_Datatype map(long double) { return MPI_LONG_DOUBLE; }

 public:  // Send
  /// Send a scalar value to another process
  template <typename Type>
  void send(const Type& value, const int toProcess, const int tag = 0) {
    return this->send(&value, /*count*/ 1, toProcess, tag, "value");
  }
  /// Send an array of count values to another process
  template <typename Type>
  void send(const Type* values, const int count, const int toProcess,
      const int tag = 0) {
    return this->send(values, count, toProcess, tag, "array");
  }
  template <typename Type>
  void send(Type* values, const int count, const int toProcess,
      const int tag = 0) {
    return this->send(values, count, toProcess, tag, "array");
  }
  /// Send an array of values to another process
  template <typename Type>
  void send(const std::vector<Type>& values, const int toProcess,
      const int tag = 0) {
    return this->send(values.data(), values.size(), toProcess, tag, "vector");
  }
  /// Send a text to another process
  template <typename Type>
  void send(const std::string& text, const int toProcess, const int tag = 0) {
    return this->send(text.data(), text.size() + 1, toProcess, tag, "string");
  }

 public:  // Receive
  /// Wait until it receives a scalar value from other process
  template <typename Type>
  void receive(Type& value, const int fromProcess = MPI_ANY_SOURCE,
      const int tag = MPI_ANY_TAG) {
    return this->receive(&value, /*capacity*/ 1, fromProcess, tag, "value");
  }
  /// Wait until it receives at most capacity values from another process
  template <typename Type>
  void receive(Type* values, const int capacity,
        const int fromProcess = MPI_ANY_SOURCE, const int tag = MPI_ANY_TAG) {
    this->receive(values, capacity, fromProcess, tag, "array");
  }
  /// Wait until it receives at most capacity values from another process
  template <typename Type>
  void receive(std::vector<Type>& values, const int capacity,
      const int fromProcess = MPI_ANY_SOURCE, const int tag = MPI_ANY_TAG) {
    if (static_cast<int>(values.size()) < capacity) {
      values.resize(capacity);
    }
    this->receive(values.data(), capacity, fromProcess, tag, "vector");
  }
  /// Wait until it receives a text of at most length chars from another process
  template <typename Type>
  void receive(std::string& text, const int capacity,
      const int fromProcess = MPI_ANY_SOURCE, const int tag = MPI_ANY_TAG) {
    std::vector<char> buffer(capacity, '\0');
    this->receive(buffer, capacity, fromProcess, tag);
    text = buffer.data();
  }

 private:
  template <typename Type>
  void send(const Type* values, const int count, const int toProcess,
      const int tag, const std::string& type) {
    if (count <= 0) {
      throw Mpi::Error("invalid count of values to send", *this);
    }
    if (MPI_Send(values, count, Mpi::map(values[0]), toProcess, tag,
        MPI_COMM_WORLD) != MPI_SUCCESS) {
      throw Mpi::Error("could not send " + type, *this);
    }
  }
  /// Wait until it receives a scalar value from other process
  template <typename Type>
  void receive(Type* values, const int capacity, const int fromProcess,
      const int tag, const std::string& type) {
    if (capacity <= 0) {
      throw Mpi::Error("invalid capacity to receive values", *this);
    }
    if (MPI_Recv(values, capacity, Mpi::map(Type()), fromProcess,
        tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE) != MPI_SUCCESS) {
      throw Mpi::Error("could not receive " + type, *this);
    }
  }
};
