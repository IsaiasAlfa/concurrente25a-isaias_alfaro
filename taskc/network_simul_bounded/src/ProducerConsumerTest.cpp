// Copyright 2020-2024 Jeisson Hidalgo-Cespedes. ECCI-UCR. CC BY 4.0

#include <cstdlib>
#include <iostream>

#include "ProducerConsumerTest.hpp"
#include "AssemblerTest.hpp"
#include "ConsumerTest.hpp"
#include "DispatcherTest.hpp"
#include "ProducerTest.hpp"

const char* const usage =
  "Usage: netsim packages consumers prod_delay disp_delay cons_delay\n"
  "\n"
  "  packages    number of packages to be produced\n"
  "  consumers   number of consumer threads\n"
  "  prod_delay  delay of producer to create a package\n"
  "  disp_delay  delay of dispatcher to dispatch a package\n"
  "  cons_delay  delay of consumer to consume a package\n"
  "\n"
  "Delays are in milliseconds, negatives are maximums for random delays\n";

ProducerConsumerTest::~ProducerConsumerTest() {
  for (size_t index = 0; index < this->producerCount; ++index) {
    delete this->producers[index];
  }
  delete this->dispatcher;
  for (ConsumerTest* consumer : this->consumers) {
    delete consumer;
  }
  delete this->assembler;
}

int ProducerConsumerTest::start(int argc, char* argv[]) {
  // Parse arguments and store values in this object's attributes
  if (int error = this->analyzeArguments(argc, argv)) {
    return error;
  }
  // Create objects for the simulation
  this->createThreadObjects();
  // Communicate simulation objects
  this->connectQueues();
  // Start the simulation
  this->startThreads();
  // Wait for objects to finish the simulation
  this->joinThreads();
  // Simulation finished
  return EXIT_SUCCESS;
}

int ProducerConsumerTest::analyzeArguments(int argc, char* argv[]) {
  // 7 + 1 arguments are mandatory
  if (argc < 8) {
    std::cout << usage;
    return EXIT_FAILURE;
  }
  int index = 1;
  this->packageCount = std::strtoull(argv[index++], nullptr, 10);
  this->producerCount = std::strtoull(argv[index++], nullptr, 10);
  this->consumerCount = std::strtoull(argv[index++], nullptr, 10);
  this->productorDelay = std::atoi(argv[index++]);
  this->dispatcherDelay = std::atoi(argv[index++]);
  this->consumerDelay = std::atoi(argv[index++]);
  this->packetLossProbability = std::atof(argv[index++]);
  if (argc >= 9) {
    this->queueCapacity = std::strtoul(argv[index++], nullptr, 10);
  }
  // TODO(any): Validate that given arguments are fine
  return EXIT_SUCCESS;
}

void ProducerConsumerTest::createThreadObjects() {
  // Create each producer
  this->producers.resize(this->producerCount);
  for (size_t index = 0; index < this->producerCount; ++index) {
    this->producers[index] = new ProducerTest(this->packageCount,
        this->productorDelay,
        this->consumerCount,
        this->producerCount,
        this->producedCount,
        this->canAccessProducedCount);
  }
  this->dispatcher = new DispatcherTest(this->dispatcherDelay);
  this->dispatcher->createOwnQueue(this->queueCapacity);
  // Create each consumer
  this->consumers.resize(this->consumerCount);
  for (size_t index = 0; index < this->consumerCount; ++index) {
    this->consumers[index] = new ConsumerTest(this->consumerDelay);
    assert(this->consumers[index]);
    this->consumers[index]->createOwnQueue(this->queueCapacity);
  }
  this->assembler = new AssemblerTest(this->consumerDelay,
    this->packetLossProbability, this->consumerCount);
  this->assembler->createOwnQueue(this->queueCapacity);
}

void ProducerConsumerTest::connectQueues() {
  // Producer push network messages to the dispatcher queue
  for (size_t index = 0; index < this->producerCount; ++index) {
    this->producers[index]->setProducingQueue(
        this->assembler->getConsumingQueue());
  }
  // Dispatcher delivers to each consumer, and they should be registered
  for (size_t index = 0; index < this->consumerCount; ++index) {
    this->dispatcher->registerRedirect(index + 1
      , this->consumers[index]->getConsumingQueue());
  }
  // this->dispatcher->registerRedirect(this->consumerCount + 1
  //   , this->assembler->getConsumingQueue());
  this->assembler->setProducingQueue(this->dispatcher->getConsumingQueue());
}

void ProducerConsumerTest::startThreads() {
  for (size_t index = 0; index < this->producerCount; ++index) {
    this->producers[index]->startThread();
  }
  this->dispatcher->startThread();
  for (size_t index = 0; index < this->consumerCount; ++index) {
    this->consumers[index]->startThread();
  }
  this->assembler->startThread();
}

void ProducerConsumerTest::joinThreads() {
  for (size_t index = 0; index < this->producerCount; ++index) {
    this->producers[index]->waitToFinish();
  }
  this->dispatcher->waitToFinish();
  for (size_t index = 0; index < this->consumerCount; ++index) {
    this->consumers[index]->waitToFinish();
  }
  this->assembler->waitToFinish();
}
