// Copyright 2020-2024 Jeisson Hidalgo-Cespedes. ECCI-UCR. CC BY 4.0

#include "ConsumerTest.hpp"
#include "Log.hpp"
#include "Util.hpp"

ConsumerTest::ConsumerTest(int consumerDelay)
  : consumerDelay(consumerDelay) {
}

int ConsumerTest::run() {
  // Start the forever loop to consume all the messages that arrive
  this->consumeLoop();

  // If the forever loop finished, no more messages will arrive
  // Print statistics
  Log::append(Log::INFO, "Consumer", std::to_string(this->receivedMessages)
    + " messages consumed");
  return EXIT_SUCCESS;
}

void ConsumerTest::consume(NetworkMessage data) {
  (void)data;
  // IMPORTANT: This simulation uses sleep() to mimics the process of
  // consuming a message. However, you must NEVER use sleep() for real projects
  Util::sleepFor(this->consumerDelay);
  ++this->receivedMessages;
}
