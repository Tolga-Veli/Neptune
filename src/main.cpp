#include "Core/Application.hpp"

int main() {
  auto app = Neptune::Application::Create(800, 800);
  app->run();
}
