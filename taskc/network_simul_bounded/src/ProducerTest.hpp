// Copyright 2020-2024 Jeisson Hidalgo-Cespedes. ECCI-UCR. CC BY 4.0

#ifndef PRODUCERTEST_HPP
#define PRODUCERTEST_HPP

#include <mutex>

#include "NetworkMessage.hpp"
#include "Producer.hpp"

/**
 * @brief A productor class example
 * Produces network messages and push them to the queue
 */
class ProducerTest : public Producer<NetworkMessage> {
  DISABLE_COPY(ProducerTest);

 protected:
  /// Number of packages to be produced
  const size_t packageCount = 0;
  /// Delay of producer to create a package, negative for max random
  const int productorDelay = 0;
  /// Number of consumer threads
  const size_t consumerCount = 0;
  /// Number of producer threads
  const size_t producerCount = 0;
  /// Number of produced network messages
  size_t& producedCount;
  /// Protects the producedCount against race conditions
  std::mutex& canAccessProducedCount;

 public:
  /// Constructor
  ProducerTest(const size_t packageCount,
      const int productorDelay,
      const size_t consumerCount,
      const size_t producerCount,
      size_t& producedCount,
      std::mutex& canAccessProducedCount);
  /// Do the message production in its own execution thread
  int run() override;
  /// Creates a simulation message to be sent by the network
  NetworkMessage createMessage(size_t index) const;
};

#endif  // PRODUCERTEST_HPP
