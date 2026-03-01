#include "Renderer.hpp"

#include "Application.hpp"

namespace Neptune::Rendering {

void Renderer::render(sf::RenderWindow &window, Core::Chessboard &board,
                      sf::Clock &deltaClock, bool isSelected,
                      std::pair<i32, i32> &selected) {
  ImGui::SFML::Update(window, deltaClock.restart());

  ImGui::Begin("ImGui window");

  ImGui::ColorEdit3("Light square colors:", m_LightCol);
  ImGui::ColorEdit3("Dark square colors:", m_DarkCol);
  ImGui::ColorEdit4("Highlight color:", m_HighlightCol);

  ImGui::End();

  window.clear();

  sf::Color light(static_cast<uint8_t>(std::round(m_LightCol[0] * 255.0f)),
                  static_cast<uint8_t>(std::round(m_LightCol[1] * 255.0f)),
                  static_cast<uint8_t>(std::round(m_LightCol[2] * 255.0f)));

  sf::Color dark(static_cast<uint8_t>(std::round(m_DarkCol[0] * 255.0f)),
                 static_cast<uint8_t>(std::round(m_DarkCol[1] * 255.0f)),
                 static_cast<uint8_t>(std::round(m_DarkCol[2] * 255.0f)));

  sf::Color highlight(
      static_cast<uint8_t>(std::round(m_HighlightCol[0] * 255.0f)),
      static_cast<uint8_t>(std::round(m_HighlightCol[1] * 255.0f)),
      static_cast<uint8_t>(std::round(m_HighlightCol[2] * 255.0f)),
      static_cast<uint8_t>(std::round(m_HighlightCol[3] * 255.0f)));

  DrawChessboard(window, light, dark);
  if (isSelected)
    DrawHighlight(selected, window, board, highlight);
  DrawChessPieces(window, board);

  ImGui::SFML::Render(window);
  if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
  }

  window.display();
}

void Renderer::DrawChessboard(sf::RenderWindow &window, sf::Color light_square,
                              sf::Color dark_square) const {
  sf::RectangleShape square({TILE_SIZE, TILE_SIZE});
  for (i32 row = 0; row < 8; row++)
    for (i32 col = 0; col < 8; col++) {
      if ((row + col) % 2 == 0)
        square.setFillColor(light_square);
      else
        square.setFillColor(dark_square);

      square.setPosition({TILE_SIZE * col, TILE_SIZE * row});
      window.draw(square);
    }
}

void Renderer::DrawHighlight(const std::pair<i32, i32> &selected,
                             sf::RenderWindow &window, Core::Chessboard &board,
                             sf::Color highlight_color) const {

  sf::RectangleShape highlight({TILE_SIZE, TILE_SIZE});
  highlight.setFillColor(highlight_color);
  highlight.setPosition(
      {TILE_SIZE * selected.first, TILE_SIZE * selected.second});

  window.draw(highlight);
  sf::CircleShape dot(15.0f);
  dot.setFillColor(sf::Color(0, 0, 0, 50));
  for (i32 row = 0; row < 8; row++)
    for (i32 col = 0; col < 8; col++)
      if (board.isMoveValid(selected, {col, row})) {
        dot.setPosition({TILE_SIZE * col + TILE_SIZE / 2 - 15,
                         TILE_SIZE * row + TILE_SIZE / 2 - 15});
        window.draw(dot);
      }
}

void Renderer::DrawChessPieces(sf::RenderWindow &window,
                               Core::Chessboard &board) const {
  for (i32 row = 0; row < 8; row++)
    for (i32 col = 0; col < 8; col++)
      if (auto [piece_type, piece_color] = board.at({col, row});
          piece_type != Core::PieceType::None) {
        std::string textureName = ToTextureName(piece_type, piece_color);
        auto texture = Application::GetAssetManager().GetTexture(textureName);

        sf::Sprite pieceSprite(*texture);
        float scaleX = TILE_SIZE / pieceSprite.getLocalBounds().size.x,
              scaleY = TILE_SIZE / pieceSprite.getLocalBounds().size.y;

        pieceSprite.setScale({scaleX, scaleY});
        pieceSprite.setPosition({TILE_SIZE * col, TILE_SIZE * row});
        window.draw(pieceSprite);
      }
}

void Renderer::UpdateView(sf::RenderWindow &window) const {
  auto windowSize = window.getSize();
  sf::View view(sf::FloatRect({0.0f, 0.0f}, {BOARD_SIZE, BOARD_SIZE}));

  float windowRatio = static_cast<float>(windowSize.x) / windowSize.y,
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

std::string Renderer::ToTextureName(Core::PieceType type,
                                    Core::PieceColor color) const noexcept {
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
} // namespace Neptune::Rendering
