#pragma once
#include <SDL3/SDL.h>
#include <spdlog/spdlog.h>

#include <string_view>

class Window {
  Window(const Window&) = delete;
  Window& operator=(const Window&) = delete;

 public:
  Window(int width, int height, std::string_view title);
  ~Window();
  bool pollEvent(SDL_Event* event);
  bool shouldClose();
  void close();
  void present();

 private:
  bool is_running_;
  SDL_Window *window_;
  SDL_GLContext context_;
};