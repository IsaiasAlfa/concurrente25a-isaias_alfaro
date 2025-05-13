// Copyright 2024 ECCI-UCR CC-BY 4.0
#include <iostream>
#include <string>
#include <vector>

#include "stats.hpp"

template <typename Type> int analyze();
template <typename Type> int analyze(std::vector<Type>& values);

int main() {
  std::string type;
  if (std::cin >> type) {
    if (type == "short") return analyze<short>();
    if (type == "ushort") return analyze<unsigned short>();
    if (type == "int") return analyze<int>();
    if (type == "uint") return analyze<unsigned>();
    if (type == "long") return analyze<long>();
    if (type == "ulong") return analyze<unsigned long>();
    if (type == "double") return analyze<double>();
  }
}

template <typename Type> int analyze() {
  std::vector<Type> values;
  Type value = Type();
  while (std::cin >> value) {
    values.push_back(value);
  }
  return analyze(values);
}

template <typename Type> int analyze(std::vector<Type>& values) {
  if (values.size() > 0) {
    std::cout << "Count  : " << values.size() << std::endl;
    std::cout << "Minimum: " << minimum(values) << std::endl;
    std::cout << "Maximum: " << maximum(values) << std::endl;
    std::cout << "Average: " << average(values) << std::endl;
    std::cout << "Std.Dev: " << std_dev(values) << std::endl;
    std::cout << "Median : " << median(values) << std::endl;
  }
  return 0;
}
