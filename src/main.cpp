#include "Core/Game.hpp"

int main() {
  auto app = Neptune::Game::Create(800, 800);
  app->Run();
}
