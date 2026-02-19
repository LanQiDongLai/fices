namespace fices {

namespace math {

double lerp(double t, double a, double b) { return a + t * (b - a); }

double grad(int hash, double x, double y) {
  int h = hash & 7;

  double u = (h < 4) ? x : y;
  double v = (h < 4) ? y : x;

  return ((h & 1) ? -u : u) + ((h & 2) ? -2.0f * v : 2.0f * v);
}

double fade(double t) {
    return t * t * t * (t * (t * 6 - 15) + 10);
}

}  // namespace math

}  // namespace fices