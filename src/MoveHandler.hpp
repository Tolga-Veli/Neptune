#pragma once

#include <array>
#include <vector>

#include "Core/Utilities.hpp"

namespace Neptune::Core {

class MoveHandler {
public:
  struct MagicBitboard {
    u64 mask, magic, *ptr;
    u8 shift;
  };

  static void Init() {
    m_KingMasks.fill(0);
    m_KnightMasks.fill(0);

    std::pair<i32, i32> dir[] = {{-2, 1}, {-1, 2}, {1, 2},   {2, 1},
                                 {2, -1}, {1, -2}, {-1, -2}, {-2, -1}};

    for (i32 i = 0; i < 64; i++) {
      // row, col, so {b3} is x=b, y=3
      // so a1 = {col,row} => x=col, y=row
      i32 x = i % 8, y = i / 8;

      for (auto &[dx, dy] : dir) {
        i32 nx = x + dx, ny = y + dy;
        if (nx >= 0 && nx < 8 && ny >= 0 && ny < 8) {
          m_KnightMasks[i] |= (1ULL << (ny * 8 + nx));
        }
      }

      for (i32 dx = -1; dx <= 1; dx++)
        for (i32 dy = -1; dy <= 1; dy++) {
          if (dx == 0 && dy == 0)
            continue;

          i32 nx = x + dx, ny = y + dy;
          if (nx >= 0 && nx < 8 && ny >= 0 && ny < 8) {
            m_KingMasks[i] |= (1ULL << (ny * 8 + nx));
          }
        }
    }
  }

  static u64 PawnMoves(i32 square, PieceColor color, u64 occupancy,
                       i64 enPassantIdx) {
    u64 mask = (1ULL << square), moves = 0;
    u64 epBit = (enPassantIdx != -1) ? (1ULL << enPassantIdx) : 0;

    if (color == PieceColor::White) {
      u64 singlePush = (mask << 8) & ~occupancy;
      moves |= singlePush;

      if (singlePush && (square / 8 == 1))
        moves |= (singlePush << 8) & ~occupancy;

      u64 leftCapture = (mask << 7) & 0x7F7F7F7F7F7F7F7FULL;
      u64 rightCapture = (mask << 9) & 0xFEFEFEFEFEFEFEFEULL;
      moves |= (leftCapture | rightCapture) & (occupancy | epBit);
    } else {
      u64 singlePush = (mask >> 8) & ~occupancy;
      moves |= singlePush;

      if (singlePush && (square / 8 == 6))
        moves |= (singlePush >> 8) & ~occupancy;

      u64 leftCapture = (mask >> 9) & 0x7F7F7F7F7F7F7F7FULL;
      u64 rightCapture = (mask >> 7) & 0xFEFEFEFEFEFEFEFEULL;
      moves |= (leftCapture | rightCapture) & (occupancy | epBit);
    }

    return moves;
  }

  static u64 KnightMoves(i32 square) { return m_KnightMasks[square]; }

  static u64 KingMoves(i32 square) { return m_KingMasks[square]; }

  static u64 RookMoves(i32 square, u64 occupancy) {
    static std::vector<std::pair<i32, i32>> dir = {
        {-1, 0}, {0, -1}, {0, 1}, {1, 0}};
    return SlidingMoves(square, occupancy, dir);
  }

  static u64 BishopMoves(i32 square, u64 occupancy) {
    static std::vector<std::pair<i32, i32>> dir = {
        {-1, -1}, {-1, 1}, {1, -1}, {1, 1}};
    return SlidingMoves(square, occupancy, dir);
  }

  static u64 QueenMoves(i32 square, u64 occupancy) {
    return RookMoves(square, occupancy) | BishopMoves(square, occupancy);
  }

private:
  inline static std::array<u64, 64> m_KnightMasks, m_KingMasks;

  static u64 SlidingMoves(i32 square, u64 occupancy,
                          const std::vector<std::pair<i32, i32>> &dir) {
    i32 x = square % 8, y = square / 8;
    u64 ans = 0;
    for (auto &[dx, dy] : dir)
      for (i32 step = 1; step < 8; step++) {
        i32 nx = x + step * dx, ny = y + step * dy;
        if (nx < 0 || nx >= 8 || ny < 0 || ny >= 8)
          break;

        u64 mask = (1ULL << (ny * 8 + nx));
        ans |= mask;

        if (occupancy & mask)
          break;
      }

    return ans;
  }
};
} // namespace Neptune::Core
