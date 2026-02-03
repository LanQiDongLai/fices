#define GLFW_INCLUDE_NONE
#include <iostream>
#include <spdlog/spdlog.h>
#include <GLFW/glfw3.h>

#include "glad/glad.h"
#include "window/window.h"

int main() {
  Window window(800, 800, "Fices");
  gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress);
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
    glClearColor(0.2, 0.3, 0.2, 1.);
    glClear(GL_COLOR_BUFFER_BIT);
    window.present();
  }
  return 0;
}