#include "Chessboard.hpp"
#include "Core/Utilities.hpp"
#include "FEN.hpp"
#include "MoveGeneration/MoveGenerator.hpp"
#include "MoveGeneration/SlidingAttacks.hpp"

namespace Neptune::Core {

void Chessboard::RebuildOccupancy() {
  auto white_occ = m_WhitePieces.GetOccupancyBitboard();
  auto black_occ = m_BlackPieces.GetOccupancyBitboard();

  white_occ = 0;
  black_occ = 0;

  for (u8 i = 1; i < 7; i++) {
    auto type = static_cast<PieceType>(i);
    white_occ |= m_WhitePieces.GetPiecesBitboard(type);
    black_occ |= m_BlackPieces.GetPiecesBitboard(type);
  }
}

void Chessboard::LoadFEN(const std::string &fen_string) {
  m_WhitePieces.Reset(), m_BlackPieces.Reset();

  auto [pieces, isWhite, castling, en_passant] = FEN::Import(fen_string);
  for (i32 idx = 0; idx < 64; idx++) {
    auto [type, color] = pieces[idx];
    if (type == PieceType::None || color == PieceColor::None)
      continue;

    u64 mask = (1ULL << idx);
    if (color == PieceColor::White)
      m_WhitePieces.GetPiecesBitboard(type) |= mask;
    else if (color == PieceColor::Black)
      m_BlackPieces.GetPiecesBitboard(type) |= mask;
  }

  m_SideToMove = (isWhite ? PieceColor::White : PieceColor::Black);
  m_Castling = castling;
  m_EnPassant = en_passant;

  RebuildOccupancy();
  NEPTUNE_INFO("Loaded the FEN string");
}

void Chessboard::ExportFEN() { /* TODO:*/ }

std::pair<PieceType, PieceColor> Chessboard::GetPieceAt(i32 square) {
  u64 mask = 1ULL << square;
  if (m_WhitePieces.GetOccupancyBitboard() & mask) {
    for (u8 i = 1; i < Bitboards::BITBOARDS_COUNT; i++) {
      PieceType type{static_cast<PieceType>(i)};
      if (m_WhitePieces.GetPiecesBitboard(type) & mask)
        return {type, PieceColor::White};
    }
  } else {
    for (u8 i = 1; i < Bitboards::BITBOARDS_COUNT; i++) {
      PieceType type{static_cast<PieceType>(i)};
      if (m_BlackPieces.GetPiecesBitboard(type) & mask)
        return {type, PieceColor::Black};
    }
  }

  return {PieceType::None, PieceColor::None};
}

const Bitboards &Chessboard::GetPieces(PieceColor side) const {
  if (side == PieceColor::White)
    return m_WhitePieces;
  else
    return m_BlackPieces;
}

bool Chessboard::InCheck(PieceColor side) const {
  u64 king = GetPieces(side).GetPiecesBitboard(PieceType::King);
  if (!king)
    return false;

  return IsAttacked(std::countr_zero(king), Opposite(side));
}

bool Chessboard::IsAttacked(i32 square, PieceColor bySide) const {
  const auto &pieces = GetPieces(bySide);
  const u64 occupied = OccupancyBoth();

  using namespace Engine;
  if (AttackTables::Pawn(square, Opposite(bySide)) & pieces.GetPiecesBitboard(PieceType::Pawn))
    return true;

  if (AttackTables::Knight(square) & pieces.GetPiecesBitboard(PieceType::Knight))
    return true;

  if (AttackTables::King(square) & pieces.GetPiecesBitboard(PieceType::King))
    return true;

  if (SlidingAttacks::Bishop(square, occupied) &
      (pieces.GetPiecesBitboard(PieceType::Bishop) | pieces.GetPiecesBitboard(PieceType::Queen)))
    return true;

  if (SlidingAttacks::Rook(square, occupied) &
      (pieces.GetPiecesBitboard(PieceType::Rook) | pieces.GetPiecesBitboard(PieceType::Queen)))
    return true;

  return false;
}

bool Chessboard::CanCastle(PieceColor side, bool kingSide) const {
  if (side == PieceColor::White)
    return (kingSide == true) ? m_Castling.whiteKingSide : m_Castling.whiteQueenSide;
  else
    return (kingSide == true) ? m_Castling.blackKingSide : m_Castling.blackQueenSide;
}

void Chessboard::MakeMove(const Engine::Move &move) {
  const PieceColor side = m_SideToMove;
  const PieceColor enemy = Opposite(side);

  auto &us = (side == PieceColor::White) ? m_WhitePieces : m_BlackPieces;
  auto &them = (side == PieceColor::White) ? m_BlackPieces : m_WhitePieces;

  u64 fromBB = 1ULL << move.from;
  u64 toBB = 1ULL << move.to;

  auto [piece, color] = GetPieceAt(move.from);
  UpdateCastlingRights(move.from, move.to, piece);

  using MoveFlag = Engine::MoveFlag;

  // CLEAR EN PASSANT EVERY MOVE
  m_EnPassant = -1;

  // REMOVE FROM OLD SQUARE
  us.GetPiecesBitboard(piece) &= ~fromBB;
  us.GetOccupancyBitboard() &= ~fromBB;

  // HANDLE CAPTURES
  if (move.IsCapture() && move.flag != MoveFlag::EnPassant) {
    auto [capPiece, capColor] = GetPieceAt(move.to);
    them.GetPiecesBitboard(capPiece) &= ~toBB;
    them.GetOccupancyBitboard() &= ~toBB;
  }

  // EN PASSANT
  if (move.flag == MoveFlag::EnPassant) {
    int capSq = (side == PieceColor::White) ? move.to - 8 : move.to + 8;
    u64 capBB = 1ULL << capSq;

    them.GetPiecesBitboard(PieceType::Pawn) &= ~capBB;
    them.GetOccupancyBitboard() &= ~capBB;
  }

  // PROMOTION
  if (move.flag >= MoveFlag::KnightPromotion) {
    PieceType promo;

    switch (move.flag) {
    case MoveFlag::KnightPromotion:
    case MoveFlag::KnightPromotionCapture:
      promo = PieceType::Knight;
      break;

    case MoveFlag::BishopPromotion:
    case MoveFlag::BishopPromotionCapture:
      promo = PieceType::Bishop;
      break;

    case MoveFlag::RookPromotion:
    case MoveFlag::RookPromotionCapture:
      promo = PieceType::Rook;
      break;

    case MoveFlag::QueenPromotion:
    case MoveFlag::QueenPromotionCapture:
      promo = PieceType::Queen;
      break;

    default:
      promo = PieceType::Queen;
      break;
    }

    us.GetPiecesBitboard(promo) |= toBB;
    us.GetOccupancyBitboard() |= toBB;

    m_SideToMove = enemy;
    return;
  }

  // CASTLING
  if (move.flag == MoveFlag::KingCastle || move.flag == MoveFlag::QueenCastle) {
    // move king
    us.GetPiecesBitboard(PieceType::King) &= ~fromBB;
    us.GetPiecesBitboard(PieceType::King) |= toBB;

    us.GetOccupancyBitboard() &= ~fromBB;
    us.GetOccupancyBitboard() |= toBB;

    // rook moves
    if (side == PieceColor::White) {
      if (move.flag == MoveFlag::KingCastle) {
        // rook h1 -> f1
        us.GetPiecesBitboard(PieceType::Rook) &= ~(1ULL << 7);
        us.GetPiecesBitboard(PieceType::Rook) |= (1ULL << 5);

        us.GetOccupancyBitboard() &= ~(1ULL << 7);
        us.GetOccupancyBitboard() |= (1ULL << 5);
      } else {
        // rook a1 -> d1
        us.GetPiecesBitboard(PieceType::Rook) &= ~(1ULL << 0);
        us.GetPiecesBitboard(PieceType::Rook) |= (1ULL << 3);

        us.GetOccupancyBitboard() &= ~(1ULL << 0);
        us.GetOccupancyBitboard() |= (1ULL << 3);
      }
    } else {
      if (move.flag == MoveFlag::KingCastle) {
        us.GetPiecesBitboard(PieceType::Rook) &= ~(1ULL << 63);
        us.GetPiecesBitboard(PieceType::Rook) |= (1ULL << 61);

        us.GetOccupancyBitboard() &= ~(1ULL << 63);
        us.GetOccupancyBitboard() |= (1ULL << 61);
      } else {
        us.GetPiecesBitboard(PieceType::Rook) &= ~(1ULL << 56);
        us.GetPiecesBitboard(PieceType::Rook) |= (1ULL << 59);

        us.GetOccupancyBitboard() &= ~(1ULL << 56);
        us.GetOccupancyBitboard() |= (1ULL << 59);
      }
    }

    m_SideToMove = enemy;
    return;
  }

  // SINGLE PUSH
  us.GetPiecesBitboard(piece) |= toBB;
  us.GetOccupancyBitboard() |= toBB;

  // DOUBLE PAWN PUSH → SET EN PASSANT
  if (piece == PieceType::Pawn && move.flag == MoveFlag::DoublePawnPush)
    m_EnPassant = (move.from + move.to) / 2;

  m_SideToMove = enemy;
}

void Chessboard::UpdateCastlingRights(i32 from, i32 to, PieceType piece) {
  auto &[whiteKingSide, whiteQueenSide, blackKingSide, blackQueenSide] = m_Castling;

  // WHITE
  if (piece == PieceType::King) {
    if (from == 4)
      whiteKingSide = whiteQueenSide = false;
    else if (from == 60)
      blackKingSide = blackQueenSide = false;

  } else if (piece == PieceType::Rook) {
    if (from == 0)
      whiteQueenSide = false;
    else if (from == 7)
      whiteKingSide = false;
    else if (from == 56)
      blackQueenSide = false;
    else if (from == 63)
      blackKingSide = false;
  }

  // rook captured on key squares
  if (to == 0)
    whiteQueenSide = false;
  else if (to == 7)
    whiteKingSide = false;
  else if (to == 56)
    blackQueenSide = false;
  else if (to == 63)
    blackKingSide = false;
}

} // namespace Neptune::Core
