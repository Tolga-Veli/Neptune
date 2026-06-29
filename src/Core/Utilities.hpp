#pragma once

#include <cstdint>
#include <string>
#include <string_view>

namespace Neptune {
using i32 = std::int32_t;
using i64 = std::int64_t;

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

namespace Core {
enum class PieceType : u8 { None = 0, Pawn, Knight, Bishop, Rook, Queen, King };
enum class PieceColor : u8 { None = 0, White, Black };
enum class GameState { None = 0, WhiteWins, BlackWins, Draw };

inline std::string to_string(PieceType type) {
  using enum PieceType;
  switch (type) {
  case Pawn:
    return "Pawn";
  case Knight:
    return "Knight";
  case Bishop:
    return "Bishop";
  case Rook:
    return "Rook";
  case Queen:
    return "Queen";
  case King:
    return "King";
  default:
    return "";
  }
}

inline std::string to_string(PieceColor color) {
  if (color == PieceColor::White)
    return "White";
  else if (color == PieceColor::Black)
    return "Black";
  else
    return "";
}

struct CastlingRights {
  bool whiteKingSide, whiteQueenSide, blackKingSide, blackQueenSide;
};

static inline consteval std::string_view strip_path(std::string_view path) {
  size_t last_slash = path.find_last_of("\\/");
  return (last_slash == std::string_view::npos) ? path
                                                : path.substr(last_slash + 1);
}
} // namespace Core
} // namespace Neptune
