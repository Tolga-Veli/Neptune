#include "AttackTables.hpp"
#include "Core/Logging.hpp"

namespace Neptune::Engine {

void AttackTables::Init() {
  InitializeKnightMasks();
  InitializeKingMasks();
  InitializePawnMasks();

  NEPTUNE_INFO("Initialized Attack Tables");
}

void AttackTables::InitAttackTable(
    std::array<u64, 64> &AttackTable,
    const std::vector<std::pair<i32, i32>> &dir) {
  for (i32 x = 0; x < 8; x++)
    for (i32 y = 0; y < 8; y++)
      for (auto [dx, dy] : dir) {
        i32 nx = x + dx, ny = y + dy;
        if (nx < 0 || nx >= 8 || ny < 0 || ny >= 8)
          continue;

        u64 from = x * 8 + y, to = nx * 8 + ny;
        AttackTable[from] |= (1ULL << to);
      }
}

void AttackTables::InitializeKnightMasks() {
  std::vector<std::pair<i32, i32>> dir{{2, 1},  {2, -1},  {1, 2},  {1, -2},
                                       {-1, 2}, {-1, -2}, {-2, 1}, {-2, -1}};
  InitAttackTable(m_KnightMasks, dir);
}

void AttackTables::InitializeKingMasks() {
  std::vector<std::pair<i32, i32>> dir{{1, -1}, {1, 0},   {1, 1},  {0, -1},
                                       {0, 1},  {-1, -1}, {-1, 0}, {-1, 1}};
  InitAttackTable(m_KingMasks, dir);
}

void AttackTables::InitializePawnMasks() {
  constexpr u64 FILE_A = 0x0101010101010101ULL, FILE_H = 0x8080808080808080ULL;

  for (i32 square = 0; square < 64; square++) {
    u64 mask = 1ULL << square;

    m_WhitePawnMasks[square] =
        ((mask << 7) & ~FILE_H) | ((mask << 9) & ~FILE_A);

    m_BlackPawnMasks[square] =
        ((mask >> 7) & ~FILE_A) | ((mask >> 9) & ~FILE_H);
  }
}
} // namespace Neptune::Engine
