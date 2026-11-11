#pragma once

#include "Core/Utilities.hpp"
#include "Core/Logging.hpp"

#include <array>
#include <vector>

namespace Neptune::Engine {

// The Attack Tables require a square index between [0,63]
class AttackTables {
public:
  static consteval void Init() {
    InitializeKnightMasks();
    InitializeKingMasks();
    InitializePawnMasks();

    NEPTUNE_INFO("Initialized Attack Tables");
  }

  static u64 Knight(i32 square) { return m_KnightMasks[square]; }

  static u64 King(i32 square) { return m_KingMasks[square]; }

  static u64 Pawn(i32 square, Core::PieceColor side) {
    return (side == Core::PieceColor::White) ? m_WhitePawnMasks[square] : m_BlackPawnMasks[square];
  }

private:
  static inline std::array<u64, 64> m_KnightMasks{}, m_KingMasks{}, m_WhitePawnMasks{}, m_BlackPawnMasks{};

  static consteval void InitAttackTable(std::array<u64, 64> &AttackTable, const std::vector<std::pair<i32, i32>> &dir) {
    for (i32 x = 0; x < 8; ++x)
      for (i32 y = 0; y < 8; ++y)
        for (auto [dx, dy] : dir) {
          i32 nx = x + dx, ny = y + dy;
          if (nx < 0 || nx >= 8 || ny < 0 || ny >= 8)
            continue;

          u64 from = x * 8 + y, to = nx * 8 + ny;
          AttackTable[from] |= (1ULL << to);
        }
  }

  static consteval void InitializeKnightMasks() {
    std::vector<std::pair<i32, i32>> dir{{2, 1}, {2, -1}, {1, 2}, {1, -2}, {-1, 2}, {-1, -2}, {-2, 1}, {-2, -1}};
    InitAttackTable(m_KnightMasks, dir);
  }

  static consteval void InitializeKingMasks() {
    std::vector<std::pair<i32, i32>> dir{{1, -1}, {1, 0}, {1, 1}, {0, -1}, {0, 1}, {-1, -1}, {-1, 0}, {-1, 1}};
    InitAttackTable(m_KingMasks, dir);
  }

  static consteval void InitializePawnMasks() {
    constexpr u64 FILE_A = 0x0101010101010101ULL, FILE_H = 0x8080808080808080ULL;

    for (i32 square = 0; square < 64; ++square) {
      u64 mask = 1ULL << square;
      m_WhitePawnMasks[square] = ((mask << 7) & ~FILE_H) | ((mask << 9) & ~FILE_A);
      m_BlackPawnMasks[square] = ((mask >> 7) & ~FILE_A) | ((mask >> 9) & ~FILE_H);
    }
  }
};
} // namespace Neptune::Engine
