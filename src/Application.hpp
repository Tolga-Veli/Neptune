#pragma once

#include <memory>

namespace ChessEngine {
class Application {
public:
  Application();

  std::shared_ptr<Application> Create() {
    return std::make_shared<Application>();
  }

private:
};
} // namespace ChessEngine
