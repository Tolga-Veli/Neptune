#pragma once

#include <SFML/Graphics.hpp>

#include "Chessboard.hpp"
#include "MoveGeneration/Move.hpp"
#include "Utilities.hpp"
#include "imgui-SFML.h"

namespace Neptune {
class Game;
}

namespace Neptune::Rendering {
struct RenderInput {
  bool isSelected, promotionPending, promotionIsCapture;
  std::pair<i32, i32> selected;
  Core::PieceColor promotionSide;
  std::vector<Engine::Move> legal_moves;
  Core::GameState result;
  bool restart, quit;
};

class Renderer {
public:
  static constexpr float TILE_SIZE = 100.0f, BOARD_SIZE = 800.0f;

public:
  Renderer() = default;
  ~Renderer() = default;

  // returns bool if the game ends or there are no promotions (no promotions =
  // 0, game ends = 1)
  std::optional<Engine::MoveFlag> Render(sf::RenderWindow &window, Core::Chessboard &board, sf::Clock &deltaClock,
                                         RenderInput &input);

  void DrawChessboard(sf::RenderWindow &window) const;

  void DrawSelectedSquare(sf::RenderWindow &window, const std::pair<i32, i32> &selected) const;

  void DrawLegalMoves(sf::RenderWindow &window, const std::pair<i32, i32> &selected,
                      const std::vector<Engine::Move> &moves) const;

  void DrawChessPieces(sf::RenderWindow &window, Core::Chessboard &board) const;

  void DrawGameOverScreen(RenderInput &input);

  void UpdateView(sf::RenderWindow &window) const;

private:
  float m_LightCol[3] = {0.93f, 0.93f, 0.82f}, m_DarkCol[3] = {0.46f, 0.59f, 0.33f},
        m_HighlightCol[4] = {1.0f, 1.0f, 0.2f, 0.4f};

  std::optional<Engine::MoveFlag> DrawPromotionInterface(Core::PieceColor side, bool isCapture);

  std::string ToTextureName(Core::PieceType type, Core::PieceColor color) const noexcept;
};

} // namespace Neptune::Rendering
