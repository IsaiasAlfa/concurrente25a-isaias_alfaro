// Copyright 2021-2025 Jeisson Hidalgo-Cespedes. ECCI-UCR. CC BY 4.0

#include <chrono>
#include <mutex>
#include <random>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include "Util.hpp"

// Try to get a seed from hardware, if available
static std::random_device::result_type seed = std::random_device()();
// This object generates random numbers using the Mersenne-Twister algorithm
static std::mt19937 randomEngine(seed);
// Protects the random engine to be thread-safe
static std::mutex canAccessRandomEngine;

int Util::random(int min, int max) {
  // Produce random values with uniform discrete distribution
  std::uniform_int_distribution<int> randomDistribution(min, max - 1);
  // Generate and return a random number using the uniform distribution
  canAccessRandomEngine.lock();
  const int result = randomDistribution(randomEngine);
  canAccessRandomEngine.unlock();
  return result;
}

double Util::random(double min, double max) {
  // Produce random values with uniform discrete distribution
  std::uniform_real_distribution<double> randomDistribution(min, max);
  // Generate and return a random number using the uniform distribution
  canAccessRandomEngine.lock();
  const double result = randomDistribution(randomEngine);
  canAccessRandomEngine.unlock();
  return result;
}

void Util::sleepFor(int milliseconds) {
  // If a negative number of milliseconds is given, sleep for a random number
  // of milliseconds in range [0, milliseconds[
  if (milliseconds < 0) {
    milliseconds = Util::random(0, std::abs(milliseconds));
  }

  // Sleep for the given amount of milliseconds
  std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

std::vector<std::string> Util::split(const std::string& text,
  const std::string& delim, bool trimEmpty) {
  std::vector<std::string> tokens;
  // Util::tokenize(text, tokens, static_cast<size_t(std::string::*)(const
  // std::string&, size_t)>(&std::string::find_first_of), delim, trimEmpty);
  Util::tokenize(text, tokens, delim, trimEmpty);
  return tokens;
}

std::string Util::decodeURI(const std::string& uri) {
  std::stringstream result;
  const size_t len = uri.length();
  // Traverse all chars in URI
  for (size_t pos = 0; pos < len; ++pos) {
    switch (uri[pos]) {
      case '%':
        // Decode %xx hexadecimal substrings to their respective ASCII char
        if (pos + 2 < len) {
          const char hex[] { uri[pos + 1], uri[pos + 2], '\0' };  // e.g: "2C"
          result << static_cast<char>(std::strtol(hex, nullptr, 16));  // ','
          pos += 2;  // Skip two chars, ++pos in for moves to char after %xx
        }
        break;
      case '+':  // Web ask to scape white spaces with + chars
        result << ' ';
        break;
      default:  // All other chars are un-scaped
        result << uri[pos];
    }
  }
  return result.str();
}
