#include "utils/random.h"

fices::Random::Random(int64_t seed) {
  seed_ = seed;
}

float fices::Random::getFloat() {
  return (float)distribution_(random_engine_) / (float)std::numeric_limits<uint32_t>::max();
}