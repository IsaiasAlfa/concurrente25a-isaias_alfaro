// Copyright 2020-2024 Jeisson Hidalgo-Cespedes. ECCI-UCR. CC BY 4.0

#include "DispatcherTest.hpp"
#include "Util.hpp"

DispatcherTest::DispatcherTest(int dispatcherDelay)
  : dispatcherDelay(dispatcherDelay) {
}

int DispatcherTest::run() {
  // Dispatch all the network messages we receive to their respective queues
  // Stop until two consecutive stop conditions are received
  while (this->stopConditionCount < 2) {
    this->consumeLoop();
    ++this->stopConditionCount;
    this->toQueues[this->toQueues.size()]->enqueue(this->stopCondition);
  }

  // If we exited from the forever loop, the finish message was received
  // For this simulation is OK to propagate it to all the queues
  for (const auto& pair : this->toQueues) {
    pair.second->enqueue(this->stopCondition);
  }

  return EXIT_SUCCESS;
}

void DispatcherTest::consume(NetworkMessage data) {
  Dispatcher::consume(data);
  this->stopConditionCount = 0;
}

int DispatcherTest::extractKey(const NetworkMessage& data) const {
  // Propagate a message requires time, simulate it
  // IMPORTANT: This simulation uses sleep() to mimic the process of propagating
  // a message. However, you must NEVER use sleep() for real projects
  Util::sleepFor(this->dispatcherDelay);

  // Return the target where this message is going to
  // as the key to find the queue
  return data.target;
}
