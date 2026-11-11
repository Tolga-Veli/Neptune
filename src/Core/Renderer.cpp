#include "Renderer.hpp"
#include "Core/Utilities.hpp"
#include "Game.hpp"
#include "MoveGeneration/Move.hpp"
#include "imgui.h"

namespace Neptune::Rendering {
std::optional<Engine::MoveFlag> Renderer::Render(sf::RenderWindow &window, Core::Chessboard &board,
                                                 sf::Clock &deltaClock, RenderInput &input) {
  ImGui::SFML::Update(window, deltaClock.restart());
  window.clear();

  std::optional<Engine::MoveFlag> chosenPromotion;
  // Draw chess GUI
  DrawChessboard(window);

  if (input.result == Core::GameState::None) {
    if (input.isSelected) {
      DrawSelectedSquare(window, input.selected);
      DrawLegalMoves(window, input.selected, input.legal_moves);
    }
  }

  DrawChessPieces(window, board);

  // Promotions
  if (input.promotionPending)
    chosenPromotion = DrawPromotionInterface(input.promotionSide, input.promotionIsCapture);

  DrawGameOverScreen(input);

  ImGui::SFML::Render(window);

  if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
  }

  window.display();
  return chosenPromotion;
}

void Renderer::DrawChessboard(sf::RenderWindow &window) const {
  sf::Color light((uint8_t)(m_LightCol[0] * 255), (uint8_t)(m_LightCol[1] * 255), (uint8_t)(m_LightCol[2] * 255));

  sf::Color dark((uint8_t)(m_DarkCol[0] * 255), (uint8_t)(m_DarkCol[1] * 255), (uint8_t)(m_DarkCol[2] * 255));

  sf::RectangleShape square({TILE_SIZE, TILE_SIZE});

  for (i32 row = 0; row < 8; row++) {
    for (i32 col = 0; col < 8; col++) {
      square.setFillColor(((row + col) % 2 == 0) ? light : dark);
      square.setPosition({col * TILE_SIZE, row * TILE_SIZE});
      window.draw(square);
    }
  }
}

void Renderer::DrawSelectedSquare(sf::RenderWindow &window, const std::pair<i32, i32> &selected) const {
  sf::Color highlight((uint8_t)(m_HighlightCol[0] * 255), (uint8_t)(m_HighlightCol[1] * 255),
                      (uint8_t)(m_HighlightCol[2] * 255), (uint8_t)(m_HighlightCol[3] * 255));

  sf::RectangleShape r({TILE_SIZE, TILE_SIZE});
  r.setFillColor(highlight);
  i32 renderY = 7 - selected.second;
  r.setPosition({selected.first * TILE_SIZE, renderY * TILE_SIZE});
  window.draw(r);
}

void Renderer::DrawLegalMoves(sf::RenderWindow &window, const std::pair<i32, i32> &selected,
                              const std::vector<Engine::Move> &moves) const {
  sf::CircleShape dot(TILE_SIZE * 0.15f);
  dot.setFillColor(sf::Color(0, 0, 0, 90));
  dot.setOrigin({dot.getRadius(), dot.getRadius()});

  i32 from = selected.second * 8 + selected.first;

  for (const auto &m : moves) {
    if (m.from != from)
      continue;

    i32 to = m.to;
    i32 x = to % 8, renderY = 7 - (to / 8);

    dot.setPosition({x * TILE_SIZE + TILE_SIZE / 2, renderY * TILE_SIZE + TILE_SIZE / 2});

    window.draw(dot);
  }
}

void Renderer::DrawChessPieces(sf::RenderWindow &window, Core::Chessboard &board) const {
  for (i32 sq = 0; sq < 64; sq++) {
    auto [type, color] = board.GetPieceAt(sq);

    if (type == Core::PieceType::None)
      continue;

    i32 x = sq % 8;
    i32 y = sq / 8;

    std::string tex = ToTextureName(type, color);
    auto texture = Neptune::Game::GetAssetManager().GetTexture(tex);

    sf::Sprite sprite(*texture);

    float scaleX = TILE_SIZE / sprite.getLocalBounds().size.x;
    float scaleY = TILE_SIZE / sprite.getLocalBounds().size.y;

    sprite.setScale({scaleX, scaleY});
    i32 renderY = 7 - y;
    sprite.setPosition({x * TILE_SIZE, renderY * TILE_SIZE});

    window.draw(sprite);
  }
}

std::optional<Engine::MoveFlag> Renderer::DrawPromotionInterface(Core::PieceColor side, bool isCapture) {
  ImGui::OpenPopup("Choose Promotion");

  ImVec2 center(BOARD_SIZE * 0.5f, BOARD_SIZE * 0.5f);
  ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
  ImGui::SetNextWindowSize(ImVec2(420, 160), ImGuiCond_Always);

  ImGuiWindowFlags flags =
      ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse;

  std::optional<Engine::MoveFlag> chosen;

  if (ImGui::BeginPopupModal("Choose Promotion", nullptr, flags)) {
    ImGui::Text("Promote pawn to:");
    ImGui::Spacing();

    std::string prefix = (side == Core::PieceColor::White) ? "White" : "Black";

    using enum Engine::MoveFlag;

    struct Option {
      std::string label;
      std::string texSuffix;
      Engine::MoveFlag quietFlag, captureFlag;
    };

    Option options[] = {
        {"Queen", "Queen", QueenPromotion, QueenPromotionCapture},
        {"Rook", "Rook", RookPromotion, RookPromotionCapture},
        {"Bishop", "Bishop", BishopPromotion, BishopPromotionCapture},
        {"Knight", "Knight", KnightPromotion, KnightPromotionCapture},
    };

    for (auto &opt : options) {
      auto tex = Game::GetAssetManager().GetTexture(prefix + opt.texSuffix);
      ImGui::PushID(opt.label.c_str());

      if (ImGui::ImageButton(opt.label.c_str(), *tex, ImVec2(64, 64))) {
        chosen = isCapture ? opt.captureFlag : opt.quietFlag;
        ImGui::CloseCurrentPopup();
      }

      ImGui::PopID();
      ImGui::SameLine();
    }

    ImGui::EndPopup();
  }

  return chosen;
}

void Renderer::DrawGameOverScreen(RenderInput &input) {
  if (input.result == Core::GameState::None)
    return;

  ImDrawList *draw = ImGui::GetForegroundDrawList();
  const ImVec2 screen = ImGui::GetIO().DisplaySize;

  draw->AddRectFilled(ImVec2(0, 0), screen, IM_COL32(0, 0, 0, 120));

  const char *text = nullptr;
  using enum Core::GameState;
  switch (input.result) {
  case WhiteWins:
    text = "White wins";
    break;
  case BlackWins:
    text = "Black wins";
    break;
  case Draw:
    text = "Draw";
    break;
  default:
    return;
  }

  ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));

  ImGui::SetNextWindowBgAlpha(0.85f);

  ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize;

  ImGui::Begin("Game Over", nullptr, flags);

  ImGui::TextUnformatted(text);
  ImGui::Spacing();

  if (ImGui::Button("Play again"))
    input.restart = true;

  ImGui::SameLine();

  if (ImGui::Button("Quit"))
    input.quit = true;

  ImGui::End();
}

void Renderer::UpdateView(sf::RenderWindow &window) const {
  auto size = window.getSize();
  sf::View view(sf::FloatRect({0, 0}, {BOARD_SIZE, BOARD_SIZE}));

  float ratio = static_cast<float>(size.x) / static_cast<float>(size.y);
  float viewRatio = 1.0f;

  float w = 1.0f, h = 1.0f, px = 0.0f, py = 0.0f;

  if (ratio > viewRatio) {
    w = viewRatio / ratio;
    px = (1 - w) / 2;
  } else {
    h = ratio / viewRatio;
    py = (1 - h) / 2;
  }

  view.setViewport(sf::FloatRect({px, py}, {w, h}));
  window.setView(view);
}

std::string Renderer::ToTextureName(Core::PieceType type, Core::PieceColor color) const noexcept {
  using namespace Core;

  std::string s;

  if (color == PieceColor::White)
    s += "White";
  if (color == PieceColor::Black)
    s += "Black";

  switch (type) {
  case PieceType::Pawn:
    s += "Pawn";
    break;
  case PieceType::Knight:
    s += "Knight";
    break;
  case PieceType::Bishop:
    s += "Bishop";
    break;
  case PieceType::Rook:
    s += "Rook";
    break;
  case PieceType::Queen:
    s += "Queen";
    break;
  case PieceType::King:
    s += "King";
    break;
  default:
    break;
  }

  return s;
}

} // namespace Neptune::Rendering
