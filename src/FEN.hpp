#pragma once

#include <array>
#include <sstream>
#include <string>

#include "Core/Utilities.hpp"

namespace Neptune::Core {
class FEN {
public:
  struct FEN_Data {
    std::array<std::pair<PieceType, PieceColor>, 64> pieces;
    bool isWhite;
    i64 castling = 0, en_passant = -1;
  };

  static FEN_Data Parse(std::string fen) {
    FEN_Data data;
    std::stringstream ss(fen);
    std::string placement_str, side_str, castling_str, ep_str, half_str,
        full_str;

    ss >> placement_str >> side_str >> castling_str >> ep_str >> half_str >>
        full_str;

    i32 rank = 7, file = 0;
    auto &[pieces, isWhite, castling, en_passant] = data;
    pieces.fill(std::make_pair<PieceType, PieceColor>(PieceType::None,
                                                      PieceColor::None));
    for (char c : placement_str) {
      if (c == '/') {
        rank--;
        file = 0;
      } else if (std::isdigit(c)) {
        i32 empty_squares = c - '0';
        for (i32 i = 0; i < empty_squares; i++) {
          pieces[rank * 8 + file] = {PieceType::None, PieceColor::None};
          file++;
        }
      } else {
        pieces[rank * 8 + file] = CharToPiece(c);
        file++;
      }
    }

    isWhite = (side_str == "w");

    if (ep_str != "-") {
      i32 ep_file = ep_str[0] - 'a', ep_rank = ep_str[1] - '1';
      en_passant = ep_rank * 8 + ep_file;
    }

    return data;
  }

private:
  static std::pair<PieceType, PieceColor> CharToPiece(char c) {
    PieceColor color = std::isupper(c) ? PieceColor::White : PieceColor::Black;
    c = std::tolower(c);

    PieceType type;
    switch (c) {
    case 'p':
      type = PieceType::Pawn;
      break;
    case 'n':
      type = PieceType::Knight;
      break;
    case 'b':
      type = PieceType::Bishop;
      break;
    case 'r':
      type = PieceType::Rook;
      break;
    case 'q':
      type = PieceType::Queen;
      break;
    case 'k':
      type = PieceType::King;
      break;
    default:
      type = PieceType::None;
      break;
    }

    return {type, color};
  }
};
} // namespace Neptune::Core
