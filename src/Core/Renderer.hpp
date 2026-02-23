#pragma once

#include <SFML/Graphics.hpp>

#include "AssetManager.hpp"
#include "Chessboard.hpp"
#include "Utilities.hpp"

namespace Neptune::Rendering {
class Renderer {
public:
  static constexpr u32 TILE_SIZE = 100, BOARD_SIZE = 800;

public:
  Renderer() = default;
  ~Renderer() = default;

  void DrawChessboard(sf::RenderWindow &window) {
    sf::RectangleShape square({TILE_SIZE, TILE_SIZE});
    for (i32 row = 0; row < 8; row++)
      for (i32 col = 0; col < 8; col++) {
        if ((row + col) % 2 == 0)
          square.setFillColor(sf::Color(238, 238, 210));
        else
          square.setFillColor(sf::Color(118, 150, 86));

        square.setPosition({(float)col * TILE_SIZE, (float)row * TILE_SIZE});
        window.draw(square);
      }
  }

  void DrawHighlight(const std::pair<i32, i32> &selected,
                     sf::RenderWindow &window, Core::Chessboard &board) {

    sf::RectangleShape highlight({TILE_SIZE, TILE_SIZE});
    highlight.setFillColor(sf::Color(255, 255, 50, 100));
    highlight.setPosition({(float)selected.first * TILE_SIZE,

                           (float)selected.second * TILE_SIZE});

    window.draw(highlight);
    sf::CircleShape dot(15.0f);
    dot.setFillColor(sf::Color(0, 0, 0, 50));
    for (i32 row = 0; row < 8; row++)
      for (i32 col = 0; col < 8; col++)
        if (board.isMoveValid(selected, {col, row})) {
          dot.setPosition({(float)col * TILE_SIZE + (float)TILE_SIZE / 2 - 15,

                           (float)row * TILE_SIZE + (float)TILE_SIZE / 2 - 15});
          window.draw(dot);
        }
  }

  void DrawChessPieces(sf::RenderWindow &window, Core::Chessboard &board,
                       Assets::AssetManager &asset_manager) {

    for (i32 row = 0; row < 8; row++)
      for (i32 col = 0; col < 8; col++)
        if (auto [piece_type, piece_color] = board.at({col, row});
            piece_type != PieceType::None) {
          std::string textureName = toTextureName(piece_type, piece_color);
          sf::Sprite pieceSprite(*asset_manager.GetTexture(textureName));
          float scaleX = (float)TILE_SIZE / pieceSprite.getLocalBounds().size.x,
                scaleY = (float)TILE_SIZE / pieceSprite.getLocalBounds().size.y;

          pieceSprite.setScale({scaleX, scaleY});
          pieceSprite.setPosition(
              {(float)col * TILE_SIZE, (float)row * TILE_SIZE});
          window.draw(pieceSprite);
        }
  }

  void UpdateView(sf::RenderWindow &window) {
    auto windowSize = window.getSize();
    sf::View view(sf::FloatRect({0.0f, 0.0f}, {800.0f, 800.0f}));

    float windowRatio = (float)windowSize.x / (float)windowSize.y,
          viewRatio = 1.0f;
    float viewportWidth = 1.0f, viewportHeight = 1.0f, viewportPosX = 0.0f,
          viewportPosY = 0.0f;

    if (windowRatio > viewRatio) {
      viewportWidth = viewRatio / windowRatio;
      viewportPosX = (1.0f - viewportWidth) / 2.0f;
    } else {
      viewportHeight = windowRatio / viewRatio;
      viewportPosY = (1.0f - viewportHeight) / 2.0f;
    }

    view.setViewport(sf::FloatRect({viewportPosX, viewportPosY},

                                   {viewportWidth, viewportHeight}));
    window.setView(view);
  }

private:
  std::string toTextureName(PieceType type, PieceColor color) {
    using namespace Core;

    if (type == PieceType::None || color == PieceColor::None)
      return "";

    std::string ans = "";
    switch (color) {
    case PieceColor::White:
      ans += "White";
      break;
    case PieceColor::Black:
      ans += "Black";
      break;
    default:
      break;
    }

    switch (type) {
    case PieceType::Pawn:
      ans += "Pawn";
      break;
    case PieceType::Knight:
      ans += "Knight";
      break;
    case PieceType::Bishop:
      ans += "Bishop";
      break;
    case PieceType::Rook:
      ans += "Rook";
      break;
    case PieceType::Queen:
      ans += "Queen";
      break;
    case PieceType::King:
      ans += "King";
      break;
    default:
      break;
    }

    return ans;
  }
};

} // namespace Neptune::Rendering
