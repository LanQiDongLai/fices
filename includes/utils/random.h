#pragma once

#include <random>

namespace fices {

class Random {
 public:
  Random(int64_t seed);
  float getFloat();
 private:
  int64_t seed_;
  std::mt19937 random_engine_;
  std::uniform_int_distribution<std::mt19937::result_type> distribution_;
};

}