#include <iostream>
#include <spdlog/spdlog.h>
#include "game_app.h"

int main() {
  GameApp app;
  app.initialize();
  app.run();
  return 0;
}