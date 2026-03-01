#pragma once

#include <SFML/Graphics.hpp>

#include "AssetManager.hpp"
#include "Chessboard.hpp"
#include "Utilities.hpp"

namespace Neptune::Rendering {
class Renderer {
public:
  static constexpr float TILE_SIZE = 100.0f, BOARD_SIZE = 800.0f;

public:
  Renderer() = default;
  ~Renderer() = default;

  void render(sf::RenderWindow &window, Core::Chessboard &board,
              sf::Clock &deltaClock, bool isSelected,
              std::pair<i32, i32> &selected);

  void DrawChessboard(sf::RenderWindow &window, sf::Color light_square,
                      sf::Color dark_square) const;

  void DrawHighlight(const std::pair<i32, i32> &selected,
                     sf::RenderWindow &window, Core::Chessboard &board,
                     sf::Color highlight_color) const;

  void DrawChessPieces(sf::RenderWindow &window, Core::Chessboard &board) const;

  void UpdateView(sf::RenderWindow &window) const;

private:
  std::string ToTextureName(Core::PieceType type,
                            Core::PieceColor color) const noexcept;

  float m_LightCol[3] = {238.0f / 255.0f, 238.0f / 255.0f, 210.0f / 255.0f},
        m_DarkCol[3] = {118.0f / 255.0f, 150.0f / 255.0f, 85.0f / 255.0f},
        m_HighlightCol[4] = {255.0f / 255.0f, 255.0f / 255.0f, 50.0f / 255.0f,
                             100.0f / 255.0f};
};
} // namespace Neptune::Rendering
