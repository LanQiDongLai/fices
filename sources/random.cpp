#include "utils/random.h"

#include "utils/math.h"

fices::Random::Random(int64_t seed) : seed_(seed), random_engine_(seed) {
  p_arr_.resize(512);
  for (int i = 0; i < 256; ++i) {
    p_arr_[i] = i;
  }
  std::shuffle(p_arr_.begin(), p_arr_.begin() + 256,
               std::default_random_engine(seed));
  for (int i = 0; i < 256; ++i) {
    p_arr_[256 + i] = p_arr_[i];
  }
}

float fices::Random::getFloat() {
  return (float)distribution_(random_engine_) /
         (float)std::numeric_limits<uint32_t>::max();
}

int fices::Random::getInt() { return distribution_(random_engine_); }

double fices::Random::perlinNoise(double x, double y) {
  int xi = (int)std::floor(x) & 255;
  int yi = (int)std::floor(y) & 255;

  double xf = x - xi;
  double yf = y - yi;

  double u = fices::math::fade(xf);
  double v = fices::math::fade(yf);

  double aa = p_arr_[p_arr_[xi] + yi];
  double ab = p_arr_[p_arr_[xi] + yi + 1];
  double ba = p_arr_[p_arr_[xi + 1] + yi];
  double bb = p_arr_[p_arr_[xi + 1] + yi + 1];

  double x1 = fices::math::lerp(u, fices::math::grad(aa, xf, yf), fices::math::grad(ba, xf - 1, yf));
  double x2 = fices::math::lerp(u, fices::math::grad(ab, xf, yf - 1), fices::math::grad(bb, xf - 1, yf - 1));

  return fices::math::lerp(v, x1, x2);
}

double fices::Random::flatNoise(double x, double y) { return 1.f; }

double fices::Random::fractalNoise(double x, double y, int octaves,
                                   double persistence, double lacunarity) {
  double value = 0.0;
  double amplitude = 1.0;
  double frequency = persistence;
  double max_value = 0.0;
  for (int i = 0; i < octaves; ++i) {
    value += amplitude * perlinNoise(x * frequency, y * frequency);
    max_value += amplitude;
    amplitude *= persistence;
    frequency *= lacunarity;
  }
  return (value / max_value + 1.0) * 0.5;
}