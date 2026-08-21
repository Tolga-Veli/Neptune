#include "Game.hpp"
#include "Core/Utilities.hpp"
#include "Logging.hpp"
#include "imgui.h"

namespace Neptune {

Game::Game(u32 width, u32 height)
    : m_Window(sf::VideoMode({width, height}), "Neptune") {

  NEPTUNE_INFO("Initialized Game window and state");
  m_Window.setFramerateLimit(60);

  Engine::MoveGenerator::Init();

  if (!ImGui::SFML::Init(m_Window))
    throw std::runtime_error("ImGui-SFML init failed");

  ImGuiIO &io = ImGui::GetIO();
  io.FontGlobalScale = 2.0f;

  // ImGui::StyleColorsDark();
  ImGui::StyleColorsLight();

  ImGui::GetStyle().ScaleAllSizes(2.0f);
}

void Game::Run() {
  sf::Clock deltaClock;

  while (m_Window.isOpen()) {
    ProcessEvents();

    auto legal_moves = m_Board.GetLegalMoves();

    Core::GameState result{Core::GameState::None};
    if (legal_moves.empty()) {
      if (m_Board.InCheck(m_Board.GetSideToMove()))
        result = (m_Board.GetSideToMove() == Core::PieceColor::White
                      ? Core::GameState::BlackWins
                      : Core::GameState::WhiteWins);
      else
        result = Core::GameState::Draw;
    }

    Rendering::RenderInput input{m_IsSelected,
                                 m_PromotionPending,
                                 m_PromotionPending &&
                                     m_PendingMove.IsCapture(),
                                 m_Selected,
                                 m_Board.GetSideToMove(),
                                 legal_moves,
                                 result};

    auto chosenFlag = m_Renderer.Render(m_Window, m_Board, deltaClock, input);

    if (input.restart)
      Reset();
    else if (input.quit)
      Shutdown();
    else {
      if (chosenFlag) {
        m_PendingMove.flag = *chosenFlag;
        m_Board.MakeMove(m_PendingMove);
        m_PromotionPending = false;
      }
    }
  }
}

std::optional<Engine::Move> Game::FindMove(i32 from, i32 to) const {
  for (const auto &move : m_Board.GetLegalMoves())
    if (move.from == from && move.to == to)
      return move;
  return std::nullopt;
}

void Game::ProcessEvents() {
  while (const auto event = m_Window.pollEvent()) {
    ImGui::SFML::ProcessEvent(m_Window, *event);

    if (event->is<sf::Event::Closed>())
      m_Window.close();

    if (const auto *mouse_btn = event->getIf<sf::Event::MouseButtonPressed>()) {
      if (mouse_btn->button == sf::Mouse::Button::Left)
        HandleLeftMouseClick(mouse_btn);
    } else if (const auto *resized = event->getIf<sf::Event::Resized>())
      m_Renderer.UpdateView(m_Window);
  }
}

void Game::HandleLeftMouseClick(const auto *mouse_btn) {
  if (m_PromotionPending)
    return;

  auto world_pos = m_Window.mapPixelToCoords(mouse_btn->position);
  i32 x = static_cast<i32>(world_pos.x / Rendering::Renderer::TILE_SIZE);
  i32 y = 7 - static_cast<i32>(world_pos.y / Rendering::Renderer::TILE_SIZE);

  if (x < 0 || x >= 8 || y < 0 || y >= 8)
    return;

  std::pair<i32, i32> curr{x, y};
  i32 sq = y * 8 + x;

  auto [piece_type, piece_color] = m_Board.GetPieceAt(sq);

  // SELECT PIECE
  if (!m_IsSelected) {
    if (piece_type != Core::PieceType::None &&
        piece_color == m_Board.GetSideToMove()) {
      m_IsSelected = true;
      m_Selected = curr;
    }
    return;
  }

  i32 from = m_Selected.second * 8 + m_Selected.first;
  auto move = FindMove(from, sq);
  if (move) {
    if (move->IsPromotion()) {
      m_PendingMove = *move;
      m_PromotionPending = true;
      m_IsSelected = false;
      return;
    }
    m_Board.MakeMove(*move);
    m_IsSelected = false;
    return;
  }

  // RESELECT PIECE
  if (piece_type != Core::PieceType::None &&
      piece_color == m_Board.GetSideToMove())
    m_Selected = curr;
}
}; // namespace Neptune
