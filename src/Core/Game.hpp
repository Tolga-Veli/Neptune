#pragma once

#include <cmath>
#include <memory>
#include <optional>

#include "AssetManager.hpp"
#include "Chessboard.hpp"
#include "Core/Utilities.hpp"
#include "MoveGeneration/MoveGenerator.hpp"
#include "Renderer.hpp"

#include "imgui-SFML.h"

namespace Neptune {

class Game {
public:
  Game(u32 width = 800, u32 height = 800);
  ~Game() { ImGui::SFML::Shutdown(); }

  static std::unique_ptr<Game> Create(u32 width, u32 height) {
    return std::make_unique<Game>(width, height);
  }

  static Assets::AssetManager &GetAssetManager() { return s_AssetManager; }

  void Run();

  void Reset() {
    m_Board.Reset();
    m_IsSelected = false, m_Selected = {0, 0};
    m_PromotionPending = false, m_PendingMove = {};
  }

  void Shutdown() { m_Window.close(); }

private:
  inline static Assets::AssetManager s_AssetManager;

  sf::RenderWindow m_Window;
  Core::Chessboard m_Board;
  Rendering::Renderer m_Renderer;

  bool m_IsSelected{};
  std::pair<i32, i32> m_Selected{};

  bool m_PromotionPending{};
  Engine::Move m_PendingMove;

private:
  std::optional<Engine::Move> FindMove(i32 from, i32 to) const;
  void ProcessEvents();
  void HandleLeftMouseClick(const auto *mouse_btn);
};

} // namespace Neptune
