#pragma once

class Scene {
 public:
  virtual ~Scene() = default;
  virtual void update(double delta) = 0;
};