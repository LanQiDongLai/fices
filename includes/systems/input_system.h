#pragma once
#include "context.h"

class InputSystem {
 public:
  InputSystem(Context* context);
  void update(double delta_time);

 private:
  Context* context_;
};