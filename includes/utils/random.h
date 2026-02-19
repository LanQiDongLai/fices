#pragma once

#include <random>
#include <algorithm>

namespace fices {

class Random {
 public:
  Random(int64_t seed);
  float getFloat();
  int getInt();
  double perlinNoise(double x, double y);
  double fractalNoise(double x, double y, int octaves = 4, double persistence = 0.5,
                   double lacunarity = 2.0);
  double flatNoise(double x, double y);

 private:
  int64_t seed_;
  std::mt19937 random_engine_;
  std::uniform_int_distribution<std::mt19937::result_type> distribution_;
  std::vector<int> p_arr_;
};

}  // namespace fices