#pragma once

#include "Core/Utilities.hpp"
#include <array>

namespace Neptune::Engine {
class SlidingAttacks {
public:
  static u64 Bishop(i32 square, u64 occupancy) {
    return GenerateSlidingAttacks(square, s_DiagDirs, occupancy);
  }

  static u64 Rook(i32 square, u64 occupancy) {
    return GenerateSlidingAttacks(square, s_CardinalDirs, occupancy);
  }

  static u64 Queen(i32 square, u64 occupancy) {
    return Bishop(square, occupancy) | Rook(square, occupancy);
  }

private:
  static constexpr std::array<std::pair<i32, i32>, 4> s_DiagDirs{
      {{1, -1}, {1, 1}, {-1, 1}, {-1, -1}}},
      s_CardinalDirs{{{1, 0}, {0, 1}, {0, -1}, {-1, 0}}};

  static u64
  GenerateSlidingAttacks(i32 square,
                         const std::array<std::pair<i32, i32>, 4> &dirs,
                         u64 occupied) {
    u64 attacks = 0;
    i32 x = square / 8, y = square % 8;
    for (auto [dx, dy] : dirs) {
      i32 nx = x, ny = y;
      for (int i = 0; i < 7; ++i) {
        nx += dx, ny += dy;
        if (nx < 0 || nx >= 8 || ny < 0 || ny >= 8)
          break;

        u64 to = nx * 8 + ny, bit = 1ULL << to;
        attacks |= bit;

        // stop at blocker
        if (occupied & bit)
          break;
      }
    }

    return attacks;
  }
};
} // namespace Neptune::Engine
