#include "game_app.h"

GameApp::GameApp()
    : window_(800, 800, "Fices"),
      context_(window_, registry_, dispatcher_),
      terrain_generator_(123),
      render_system_(context_, mesh_manager_),
      input_system_(&context_),
      player_system_(&context_),
      chunk_system_(context_, terrain_generator_, chunk_mesher_, mesh_manager_),
      debug_system_(&context_),
      scene_(context_),
      last_frame_time_(std::chrono::high_resolution_clock::now()) {}

GameApp::~GameApp() {
  dispatcher_.sink<GameQuitEvent>().disconnect(this);
  window_.close();
}

void GameApp::initialize() {
  dispatcher_.sink<GameQuitEvent>().connect<&GameApp::onCloseWindow>(this);
  render_system_.initialize();
  player_system_.initialize();
  chunk_system_.initialize();
}

void GameApp::run() {
  while (is_running_) {
    update();
  }
}

void GameApp::update() {
  const auto current = std::chrono::high_resolution_clock::now();
  const auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
      current - last_frame_time_);
  last_frame_time_ = current;
  const double delta_time = static_cast<double>(duration.count());

  input_system_.update(delta_time);
  player_system_.update(delta_time);
  chunk_system_.update(delta_time);
  scene_.update(delta_time);
  debug_system_.update(delta_time);
  dispatcher_.update();

  render_system_.update(delta_time);
  window_.present();
}

void GameApp::onCloseWindow() {
  is_running_ = false;
  window_.close();
}
