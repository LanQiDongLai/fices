#pragma once
#include "context.h"

#include <spdlog/spdlog.h>

#include "components/transform.h"
#include "components/tag.h"

class DebugSystem {
 public:
  DebugSystem(Context* context);
  void update(double delta_time);

 private:
  Context* context_;
};