#include "window/window.h"

Window::Window(int width, int height, std::string_view title) {
  SDL_SetHint(SDL_HINT_VIDEO_DOUBLE_BUFFER, "1");
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
  SDL_GL_SetAttribute(SDL_GLAttr::SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GLAttr::SDL_GL_CONTEXT_MINOR_VERSION, 6);
  SDL_GL_SetAttribute(SDL_GLAttr::SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GLAttr::SDL_GL_RED_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GLAttr::SDL_GL_GREEN_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GLAttr::SDL_GL_BLUE_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GLAttr::SDL_GL_ALPHA_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GLAttr::SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GLAttr::SDL_GL_MULTISAMPLESAMPLES, 4);
  SDL_GL_SetAttribute(SDL_GLAttr::SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  
  window_ = SDL_CreateWindow(title.data(), width, height, SDL_WINDOW_OPENGL);
  context_ = SDL_GL_CreateContext(window_);
  is_running_ = true;
  SDL_GL_MakeCurrent(window_, context_);
  SDL_ShowWindow(window_);
  SDL_GL_SetSwapInterval(1);

  SDL_SetWindowRelativeMouseMode(window_, true);
  SDL_SetWindowMouseGrab(window_, true);
  SDL_HideCursor();
  SDL_WarpMouseInWindow(window_, 100, 100);
}

Window::~Window() {
  SDL_DestroyWindow(window_);
  SDL_GL_DestroyContext(context_);
  SDL_Quit();
}

bool Window::pollEvent(SDL_Event* event) {
  return SDL_PollEvent(event);
}

bool Window::shouldClose() {
  return !is_running_;
}

void Window::present() {
  SDL_GL_SwapWindow(window_);
}

void Window::close() {
  is_running_ = false;
}