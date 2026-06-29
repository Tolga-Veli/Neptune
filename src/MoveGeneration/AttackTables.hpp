#pragma once

#include "Core/Utilities.hpp"
#include <array>
#include <vector>

namespace Neptune::Engine {

// The Attack Tables require a square index between [0,63]
class AttackTables {
public:
  static void Init();

  static u64 Knight(i32 square) { return m_KnightMasks[square]; }

  static u64 King(i32 square) { return m_KingMasks[square]; }

  static u64 Pawn(i32 square, Core::PieceColor side) {
    return (side == Core::PieceColor::White) ? m_WhitePawnMasks[square]
                                             : m_BlackPawnMasks[square];
  }

private:
  static inline std::array<u64, 64> m_KnightMasks{}, m_KingMasks{},
      m_WhitePawnMasks{}, m_BlackPawnMasks{};

  static void InitAttackTable(std::array<u64, 64> &AttackTable,
                              const std::vector<std::pair<i32, i32>> &dir);
  static void InitializeKnightMasks();
  static void InitializeKingMasks();
  static void InitializePawnMasks();
};
} // namespace Neptune::Engine
