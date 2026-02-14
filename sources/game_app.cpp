#include "game_app.h"

GameApp::GameApp() {
  entt::dispatcher* dispatcher = new entt::dispatcher;
  entt::registry* registry = new entt::registry;
  Window* window = new Window(800, 800, "Fices");
  context_ = new Context(window, registry, dispatcher);
  is_running_ = true;

  render_system_ = new RenderSystem(context_);
  input_system_ = new InputSystem(context_);
  chunk_system_ = new ChunkSystem(context_);
  player_system_ = new PlayerSystem(context_);
  debug_system_ = new DebugSystem(context_);

  scene_ = new GameScene(context_);

  last_frame_time_ = std::chrono::high_resolution_clock::now();
}

void GameApp::initialize() {
  context_->getDispatcher()
      ->sink<GameQuitEvent>()
      .connect<&GameApp::onCloseWindow>(this);
  render_system_->initialize();
  player_system_->initialize();
  chunk_system_->initialize();
}

void GameApp::run() {
  while (is_running_) {
    spdlog::info("update");
    update();
  }
}

void GameApp::update() {
  std::chrono::time_point current = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
      current - last_frame_time_);
  last_frame_time_ = current;

  render_system_->update(duration.count());
  input_system_->update(duration.count());
  player_system_->update(duration.count());
  chunk_system_->update(duration.count());
  debug_system_->update(duration.count());

  scene_->update(duration.count());

  context_->getDispatcher()->update();
  context_->getWindow()->present();
}

void GameApp::onCloseWindow() {
  is_running_ = false;
}

GameApp::~GameApp() {
  context_->getWindow()->close();
  delete context_;
  delete render_system_;
  delete input_system_;
}