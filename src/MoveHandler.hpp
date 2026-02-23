#pragma once

#include "Utilities.hpp"

namespace Neptune::Core {

class MoveHandler {
public:
  struct MagicBitboard {
    u64 mask, magic, *ptr;
    u8 shift;
  };

  static void Init() {
    std::pair<i32, i32> dir[] = {{-2, 1}, {-1, 2}, {1, 2},   {2, 1},
                                 {2, -1}, {1, -2}, {-1, -2}, {-2, -1}};

    for (i32 i = 0; i < 64; i++) {
      m_KnightMasks[i] = 0;
      i32 x = i % 8, y = i / 8;

      // Initialize Knight Masks
      for (auto &[dx, dy] : dir) {
        i32 nx = x + dx, ny = y + dy;
        if (nx >= 0 && nx < 8 && ny >= 0 && ny < 8)
          m_KnightMasks[i] |= (1ULL << (ny * 8 + nx));
      }

      // Initialize King Masks
      m_KingMasks[i] = 0;
      for (i32 dx = -1; dx <= 1; dx++) {
        for (i32 dy = -1; dy <= 1; dy++) {
          if (dx == 0 && dy == 0)
            continue;
          i32 nf = x + dx, nr = y + dy;
          if (nf >= 0 && nf < 8 && nr >= 0 && nr < 8)
            m_KingMasks[i] |= (1ULL << (nr * 8 + nf));
        }
      }
    }
  }

  static u64 PawnMoves(i32 idx, PieceColor color, u64 occupancy,
                       i64 enPassantIdx) {
    u64 mask = (1ULL << idx), moves = 0;

    if (color == PieceColor::White) {
      u64 singlePush = (mask << 8) & ~occupancy;
      moves |= singlePush;

      if (singlePush && (idx / 8 == 1))
        moves |= (singlePush << 8) & ~occupancy;

      u64 attacks = ((mask << 7) & 0x7F7F7F7F7F7F7F7FULL) |
                    ((mask << 9) & 0xFEFEFEFEFEFEFEFEULL);

      moves |= (attacks & (occupancy |
                           (enPassantIdx != -1 ? (1ULL << enPassantIdx) : 0)));
    } else {
      u64 singlePush = (mask >> 8) & ~occupancy;
      moves |= singlePush;

      if (singlePush && (idx / 8 == 6))
        moves |= (singlePush >> 8) & ~occupancy;

      u64 attacks = ((mask >> 7) & 0xFEFEFEFEFEFEFEFEULL) |
                    ((mask >> 9) & 0x7F7F7F7F7F7F7F7FULL);

      moves |= (attacks & (occupancy |
                           (enPassantIdx != -1 ? (1ULL << enPassantIdx) : 0)));
    }

    return moves;
  }

  static u64 RookMoves(i32 square, u64 occupancy) { return 0; }

  static u64 BishopMoves(i32 square, u64 occupancy) { return 0; }

  static u64 QueenMoves(i32 square, u64 occupancy) {
    return RookMoves(square, occupancy) | BishopMoves(square, occupancy);
  }

  static u64 KnightMoves(i32 idx) { return m_KnightMasks[idx]; }

  static u64 KingMoves(i32 idx) { return m_KingMasks[idx]; }

private:
  inline static u64 m_KnightMasks[64], m_KingMasks[64];
};
} // namespace Neptune::Core
