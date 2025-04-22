// Copyright 2020-2024 Jeisson Hidalgo-Cespedes. ECCI-UCR. CC BY 4.0

#include "AssemblerTest.hpp"
#include "Log.hpp"
#include "Util.hpp"

AssemblerTest::AssemblerTest(const int consumerDelay,
    const double packetLossProbability, const size_t consumerCount)
  : consumerDelay(consumerDelay)
  , packetLossProbability(packetLossProbability)
  , consumerCount(consumerCount) {
}

int AssemblerTest::run() {
  // Start the forever loop to consume all the messages that arrive
  // Stop until two consecutive stop conditions are received
  while (this->stopConditionCount < 2) {
    this->consumeLoop();
    ++this->stopConditionCount;
    this->produce(this->stopCondition);
  }

  // If the forever loop finished, no more messages will arrive
  // Print statistics
  Log::append(Log::INFO, "Assembler", std::to_string(this->lostMessages)
    + " messages lost");
  return EXIT_SUCCESS;
}

void AssemblerTest::consume(NetworkMessage data) {
  this->stopConditionCount = 0;
  // IMPORTANT: This simulation uses sleep() to mimics the process of
  // consuming a message. However, you must NEVER use sleep() for real projects
  Util::sleepFor(this->consumerDelay);
  if (Util::random(0.0, 100.0) < this->packetLossProbability) {
    ++this->lostMessages;
  } else {
    data.target = Util::random(1, this->consumerCount + 1);
    this->produce(data);
  }
}
