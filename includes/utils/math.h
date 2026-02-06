namespace fices {

namespace math {

float lerp(float t, float a, float b) { return a + t * (b - a); }

float grad(int hash, float x, float y) {
  int h = hash & 7;

  float u = (h < 4) ? x : y;
  float v = (h < 4) ? y : x;

  return ((h & 1) ? -u : u) + ((h & 2) ? -2.0f * v : 2.0f * v);
}

float fade(float t) {
    return t * t * t * (t * (t * 6 - 15) + 10);
}

}  // namespace math

}  // namespace fices