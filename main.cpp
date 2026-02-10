#include <iostream>
#include <spdlog/spdlog.h>
#include "game_app.h"

int main() {
  spdlog::set_level(spdlog::level::debug);
  GameApp app;
  app.initialize();
  app.run();
  return 0;
}