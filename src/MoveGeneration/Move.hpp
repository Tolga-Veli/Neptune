#pragma once

#include "Core/Utilities.hpp"

namespace Neptune::Engine {

enum class MoveFlag : u8 {
  Quiet = 0,

  DoublePawnPush = 1,
  EnPassant,

  KingCastle,
  QueenCastle,

  Capture,

  KnightPromotion,
  BishopPromotion,
  RookPromotion,
  QueenPromotion,

  KnightPromotionCapture,
  BishopPromotionCapture,
  RookPromotionCapture,
  QueenPromotionCapture,
};

struct Move {
  u8 from, to;
  MoveFlag flag{MoveFlag::Quiet};

  constexpr bool IsCapture() const {
    switch (flag) {
    case MoveFlag::Capture:
    case MoveFlag::EnPassant:
    case MoveFlag::KnightPromotionCapture:
    case MoveFlag::BishopPromotionCapture:
    case MoveFlag::RookPromotionCapture:
    case MoveFlag::QueenPromotionCapture:
      return true;
    default:
      return false;
    }
  }

  constexpr bool IsPromotion() const {
    return flag >= MoveFlag::KnightPromotion;
  }
};

} // namespace Neptune::Engine
