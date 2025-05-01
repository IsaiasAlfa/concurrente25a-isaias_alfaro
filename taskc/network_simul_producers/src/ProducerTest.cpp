// Copyright 2020-2024 Jeisson Hidalgo-Cespedes. ECCI-UCR. CC BY 4.0

#include "ProducerTest.hpp"
#include "Log.hpp"
#include "Util.hpp"

ProducerTest::ProducerTest(const size_t packageCount,
    const int productorDelay,
    const size_t consumerCount,
    const size_t producerCount,
    size_t& producedCount,
    std::mutex& canAccessProducedCount)
  : packageCount(packageCount)
  , productorDelay(productorDelay)
  , consumerCount(consumerCount)
  , producerCount(producerCount)
  , producedCount(producedCount)
  , canAccessProducedCount(canAccessProducedCount) {
}

int ProducerTest::run() {
  // Produce each asked message
  size_t myPackageCount = 0;
  while (true) {
    this->canAccessProducedCount.lock();
    const size_t myMessageNumber = ++this->producedCount;
    this->canAccessProducedCount.unlock();
    if (myMessageNumber <= this->packageCount) {
      this->produce(this->createMessage(myMessageNumber));
      ++myPackageCount;
    } else {
        if (myMessageNumber == this->packageCount + this->producerCount) {
          // Produce an empty message to communicate we finished
          this->produce(NetworkMessage());
        }
      break;
    }
  }

  // Report production is done
  Log::append(Log::INFO, "Producer", std::to_string(myPackageCount)
    + " messages sent");
  return EXIT_SUCCESS;
}

NetworkMessage ProducerTest::createMessage(size_t index) const {
  // Source is always 1: this producer
  const uint16_t source = 1;
  // Target is selected by random
  const uint16_t target = 1 + Util::random(0
    , static_cast<int>(this->consumerCount));
  // IMPORTANT: This simulation uses sleep() to mimics the process of
  // producing a message. However, you must NEVER use sleep() for real projects
  Util::sleepFor(this->productorDelay);
  // Create and return a copy of the network message
  return NetworkMessage(target, source, index);
}
