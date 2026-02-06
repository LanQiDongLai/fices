#include "utils/random.h"

#include "utils/math.h"

fices::Random::Random(int64_t seed) { seed_ = seed; }

float fices::Random::getFloat() {
  return (float)distribution_(random_engine_) /
         (float)std::numeric_limits<uint32_t>::max();
}

int fices::Random::getInt() { return distribution_(random_engine_); }

float fices::Random::perlinNoise(float x, float y) {
  int xi = (int)std::floor(x);
  int yi = (int)std::floor(y);

  float xf = x - xi;
  float yf = y - yi;

  float u = fices::math::fade(xf);
  float v = fices::math::fade(yf);

  float n00 = fices::math::grad(getInt(), xf, yf);
  float n01 = fices::math::grad(getInt(), xf, yf - 1);
  float n10 = fices::math::grad(getInt(), xf - 1, yf);
  float n11 = fices::math::grad(getInt(), xf - 1, yf - 1);

  float nx0 = fices::math::lerp(u, n00, n10);
  float nx1 = fices::math::lerp(u, n01, n11);

  return fices::math::lerp(v, nx0, nx1);
}

float fices::Random::flatNoise(float x, float y) { return 1.f; }