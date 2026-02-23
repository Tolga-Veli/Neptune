#pragma once

#include <memory>

#include "AssetManager.hpp"
#include "Chessboard.hpp"
#include "Renderer.hpp"

namespace Neptune {
class Application {
public:
  Application()
      : m_Window(sf::VideoMode({800, 800}), "Neptune"), m_AssetManager(),
        m_Board() {
    m_Window.setFramerateLimit(60);
  }

  static std::unique_ptr<Application> Create() {
    return std::make_unique<Application>();
  }

  void run() {
    bool isSelected = false;
    std::pair<i32, i32> selected;

    using namespace Core;

    while (m_Window.isOpen()) {
      while (const auto event = m_Window.pollEvent()) {
        if (event->is<sf::Event::Closed>())
          m_Window.close();

        if (const auto *mouse_btn =
                event->getIf<sf::Event::MouseButtonPressed>()) {
          if (mouse_btn->button == sf::Mouse::Button::Left) {

            auto mouse_pos = mouse_btn->position;
            auto world_pos = m_Window.mapPixelToCoords(mouse_pos);

            i32 x = static_cast<i32>(world_pos.x /
                                     Rendering::Renderer::TILE_SIZE),
                y = static_cast<i32>(world_pos.y /
                                     Rendering::Renderer::TILE_SIZE);

            if (x < 0 || x >= 8 || y < 0 || y >= 8)
              continue;

            std::pair<i32, i32> curr{x, y};
            auto [piece_type, piece_color] = m_Board.at(curr);
            if (!isSelected) {
              if (piece_type != PieceType::None) {
                isSelected = true;
                selected = curr;
              }
            } else {
              if (m_Board.isMoveValid(selected, curr)) {
                m_Board.move(selected, curr);
                isSelected = false;
              } else if (piece_type != PieceType::None &&
                         piece_color == m_Board.at(selected).second) {
                selected = {x, y};
              } else
                isSelected = false;
            }
          }
        } else if (const auto *resized = event->getIf<sf::Event::Resized>())
          m_Renderer.UpdateView(m_Window);
      }

      m_Window.clear();

      m_Renderer.DrawChessboard(m_Window);
      if (isSelected)
        m_Renderer.DrawHighlight(selected, m_Window, m_Board);
      m_Renderer.DrawChessPieces(m_Window, m_Board, m_AssetManager);

      m_Window.display();
    }
  }

private:
  sf::RenderWindow m_Window;
  Assets::AssetManager m_AssetManager;
  Core::Chessboard m_Board;
  Rendering::Renderer m_Renderer;
};
} // namespace Neptune
