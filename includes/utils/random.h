#pragma once

#include <random>

namespace fices {

class Random {
 public:
  Random(int64_t seed);
  float getFloat();
  int getInt();
  float perlinNoise(float x, float y);
  float flatNoise(float x, float y);
 private:
  int64_t seed_;
  std::mt19937 random_engine_;
  std::uniform_int_distribution<std::mt19937::result_type> distribution_;
};

}