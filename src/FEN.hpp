#pragma once

#include <array>
#include <sstream>
#include <string>

#include "Utilities.hpp"

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
    std::string placement, side, castling, ep, half, full;

    ss >> placement >> side >> castling >> ep >> half >> full;

    i32 rank = 7, file = 0;
    data.pieces.fill(std::make_pair<PieceType, PieceColor>(PieceType::None,
                                                           PieceColor::None));
    for (char c : placement) {
      if (c == '/') {
        rank--;
        file = 0;
      } else if (std::isdigit(c)) {
        i32 empty_squares = c - '0';
        for (i32 i = 0; i < empty_squares; i++) {
          data.pieces[rank * 8 + file] = {PieceType::None, PieceColor::None};
          file++;
        }
      } else {
        data.pieces[rank * 8 + file] = CharToPiece(c);
        file++;
      }
    }

    data.isWhite = (side == "w");

    if (ep != "-") {
      i32 ep_file = ep[0] - 'a', ep_rank = ep[1] - '1';
      data.en_passant = ep_rank * 8 + ep_file;
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
