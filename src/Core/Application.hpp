#pragma once

#include <memory>
#include <cmath>

#include "AssetManager.hpp"
#include "Chessboard.hpp"
#include "Renderer.hpp"

#include "imgui.h"
#include "imgui-SFML.h"

#define IMGUI_ENABLE_DOCKING
#define IMGUI_HAS_VIEWPORT

namespace Neptune {
class Application {
public:
  Application() = delete;
  explicit Application(u32 width, u32 height)
      : m_Window(sf::VideoMode({width, height}), "Neptune"), m_Board() {

    m_Window.setFramerateLimit(60);

    if (!ImGui::SFML::Init(m_Window))
      throw std::runtime_error("ImGui-SFML init failed");

    ImGuiIO &io = ImGui::GetIO();
    io.FontGlobalScale = 1.2;

    ImGui::StyleColorsDark();

    ImGuiStyle &style = ImGui::GetStyle();
    style.ScaleAllSizes(1.2);
  }

  ~Application() {
    m_Window.close();
    ImGui::SFML::Shutdown();
  }

  static std::unique_ptr<Application> Create(u32 width, u32 height) {
    return std::make_unique<Application>(width, height);
  }

  void run() {
    using namespace Core;

    bool isSelected = false;
    std::pair<i32, i32> selected;

    sf::Clock deltaClock;
    while (m_Window.isOpen()) {
      while (const auto event = m_Window.pollEvent()) {
        ImGui::SFML::ProcessEvent(m_Window, *event);

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

      m_Renderer.render(m_Window, m_Board, deltaClock, isSelected, selected);
    }
  }

  static Assets::AssetManager &GetAssetManager() { return s_AssetManager; }

private:
  inline static Assets::AssetManager s_AssetManager;

  sf::RenderWindow m_Window;
  Core::Chessboard m_Board;
  Rendering::Renderer m_Renderer;
};
} // namespace Neptune
