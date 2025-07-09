// Copyright 2025 ECCI-UCR CC-BY 4.0
#include <iostream>
#include <mutex>
#include <stack>
#include <thread>
#include <vector>

// Thread-safe dish stack
class DishStack {
 private:
  std::stack<int> stack;
  std::mutex mutex;

 public:
  /// Return true if the stack is empty, false otherwise
  int empty() {
    this->mutex.lock();
    const bool result = this->stack.empty();
    if (result) {
      this->mutex.unlock();
      return -1;
    } else {
      const int dish = this->pop();
      this->mutex.unlock();
      return dish;
    }
  }
  /// Return the number of dishes in the stack
  size_t size() {
    this->mutex.lock();
    const size_t result = this->stack.size();
    this->mutex.unlock();
    return result;
  }
  /// Push a value onto the stack, and it becomes the top element
  void push(const int& value) {
    this->mutex.lock();
    this->stack.push(value);
    this->mutex.unlock();
  }
  /// Pop the top element from the stack and return its value
  /// Precondition: the stack must be not empty
  int pop() {
    const int value = stack.top();
    stack.pop();
    return value;
  }
};

DishStack dishes;

void wash(const int dish) {
  // No need to do anything in this version
  (void)dish;
}

void wash_dishes() {
  while (true) {
    const int dish = dishes.empty();
    if(dish == -1) {
      break;
    } else {
      wash(dish);
    }    
  }
}

int main() {
  // Fill the stack with dishes
  int washer_count = 0, dish_count = 0;
  std::cin >> washer_count >> dish_count;
  for (int counter = 0; counter < dish_count; ++counter) {
    dishes.push(counter);
  }
  // Hire dishwashers
  std::vector<std::thread> dishwashers;
  for (int counter = 0; counter < washer_count; ++counter) {
    dishwashers.emplace_back(std::thread(wash_dishes));
  }
  // Wait for all dishwashers to finish
  for (std::thread& dishwasher : dishwashers) {
    dishwasher.join();
  }
  // Be sure all dishes were washed
  std::cout << "Unwashed dishes: " << dishes.size() << std::endl;
  return 0;
}
