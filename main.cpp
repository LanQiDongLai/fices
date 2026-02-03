#include <iostream>
#include <spdlog/spdlog.h>

#include "window/window.h"
#include "glad/glad.h"

int main() {
  Window window(400, 400, "Fices");
  while(!window.shouldClose()) {
    SDL_Event event;
    while(window.pollEvent(&event)) {
      switch (event.type)
      {
      case SDL_EVENT_QUIT:
        window.close();
        break;
      default:
        break;
      }
    }
    window.present();
  }
  return 0;
}