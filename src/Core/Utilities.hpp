#pragma once

#include <cstdint>
#include <string_view>

namespace Neptune {
using i32 = std::int32_t;
using i64 = std::int64_t;

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

namespace Core {
enum class PieceType : u8 {
  None = 0,
  Pawn,
  Knight,
  Bishop,
  Rook,
  Queen,
  King,
};

enum class PieceColor : u8 {
  None = 0,
  White,
  Black,
};

enum class CastlingRights : u32 {
  None = 0,
  WhiteKingSide = 1 << 0,
  WhiteQueenSide = 1 << 1,
  BlackKingSide = 1 << 2,
  BlackQueenSide = 1 << 3
};

inline CastlingRights operator|(CastlingRights a, CastlingRights b) {
  return static_cast<CastlingRights>(static_cast<u32>(a) | static_cast<u32>(b));
}

inline CastlingRights operator&(CastlingRights a, CastlingRights b) {
  return static_cast<CastlingRights>(static_cast<u32>(a) & static_cast<u32>(b));
}

inline CastlingRights operator~(CastlingRights a) {
  return static_cast<CastlingRights>(~static_cast<u32>(a));
}

inline CastlingRights &operator|=(CastlingRights &a, CastlingRights b) {
  a = a | b;
  return a;
}

inline CastlingRights &operator&=(CastlingRights &a, CastlingRights b) {
  a = a & b;
  return a;
}

static inline i32 to1D(const std::pair<i32, i32> &curr) {
  return (7 - curr.second) * 8 + curr.first;
}

static inline consteval std::string_view strip_path(std::string_view path) {
  size_t last_slash = path.find_last_of("\\/");
  return (last_slash == std::string_view::npos) ? path
                                                : path.substr(last_slash + 1);
}
} // namespace Core
} // namespace Neptune
