#pragma once

#include "Core/Utilities.hpp"
#include "FEN.hpp"
#include "MoveHandler.hpp"

namespace Neptune::Core {

class Chessboard {
  static constexpr u8 PIECES_ARRAY_SIZE = 8;

public:
  Chessboard() {
    loadFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    MoveHandler::Init();
  }

  void loadFEN(std::string fen_string) {
    m_WhitePieces.fill(0);
    m_BlackPieces.fill(0);
    m_AllWhitePieces = 0;
    m_AllBlackPieces = 0;

    auto [pieces, isWhite, castling, en_passant] = Core::FEN::Parse(fen_string);

    for (i32 idx = 0; idx < 64; idx++) {
      const auto &[type, color] = pieces[idx];
      if (type == PieceType::None || color == PieceColor::None)
        continue;

      u64 mask = (1ULL << idx);
      if (color == PieceColor::White) {
        m_WhitePieces[static_cast<u8>(type)] |= mask;
        m_AllWhitePieces |= mask;
      } else if (color == PieceColor::Black) {
        m_BlackPieces[static_cast<u8>(type)] |= mask;
        m_AllBlackPieces |= mask;
      }
    }

    m_isWhite = isWhite;
    m_Castling = castling;
    m_enPassant = en_passant;
  }

  std::pair<PieceType, PieceColor> at(const std::pair<i32, i32> &curr) {
    u64 mask = (1ULL << to1D(curr));
    for (u8 i = 1; i < PIECES_ARRAY_SIZE; i++) {
      if (m_WhitePieces[i] & mask)
        return {static_cast<PieceType>(i), PieceColor::White};
      else if (m_BlackPieces[i] & mask)
        return {static_cast<PieceType>(i), PieceColor::Black};
    }

    return {PieceType::None, PieceColor::None};
  }

  bool isMoveValid(const std::pair<i32, i32> &selected,
                   const std::pair<i32, i32> &target) {
    const auto &[type, color] = at(selected);
    const auto &[targetType, targetColor] = at(target);

    if (m_isWhite) {
      if (color == PieceColor::Black)
        return false;
    } else {
      if (color == PieceColor::White)
        return false;
    }

    if (color == targetColor && targetType != PieceType::None)
      return false;

    i32 fromIdx = to1D(selected);
    u64 targetMask = 1ULL << to1D(target),
        occupancy = m_AllWhitePieces | m_AllBlackPieces;

    switch (type) {
    case PieceType::Pawn:
      return MoveHandler::PawnMoves(fromIdx, color, occupancy, m_enPassant) &
             targetMask;
    case PieceType::Knight:
      return MoveHandler::KnightMoves(fromIdx) & targetMask;
    case PieceType::Bishop:
      return MoveHandler::BishopMoves(fromIdx, occupancy) & targetMask;
    case PieceType::Rook:
      return MoveHandler::RookMoves(fromIdx, occupancy) & targetMask;
    case PieceType::Queen:
      return MoveHandler::QueenMoves(fromIdx, occupancy) & targetMask;
    case PieceType::King: {
      u64 moves = MoveHandler::KingMoves(fromIdx);
      // FIX: castling

      return moves & targetMask;
    }
    default:
      return false;
    }
  }

  void move(const std::pair<i32, i32> &from, const std::pair<i32, i32> &to) {
    const auto &[type, color] = at(from);
    if (type == PieceType::None)
      return;

    i32 fromIdx = to1D(from), toIdx = to1D(to);
    u64 fromMask = 1ULL << fromIdx, toMask = 1ULL << toIdx;

    // Castling
    if (type == PieceType::King) {
    }

    for (u8 i = 1; i < PIECES_ARRAY_SIZE; i++) {
      m_WhitePieces[i] &= ~toMask;
      m_BlackPieces[i] &= ~toMask;
    }

    m_AllWhitePieces &= ~toMask;
    m_AllBlackPieces &= ~toMask;

    if (type == PieceType::Pawn && toIdx == m_enPassant) {
      i32 capturedPawnIdx =
          (color == PieceColor::White) ? toIdx - 8 : toIdx + 8;
      u64 capMask = ~(1ULL << capturedPawnIdx);

      for (u8 i = 1; i < PIECES_ARRAY_SIZE; i++) {
        m_WhitePieces[i] &= capMask;
        m_BlackPieces[i] &= capMask;
      }

      m_AllWhitePieces &= capMask;
      m_AllBlackPieces &= capMask;
    }

    if (color == PieceColor::White) {
      m_WhitePieces[static_cast<u8>(type)] &= ~fromMask;
      m_WhitePieces[static_cast<u8>(type)] |= toMask;

      m_AllWhitePieces &= ~fromMask;
      m_AllWhitePieces |= toMask;
      m_AllBlackPieces &= ~toMask;
    } else {
      m_BlackPieces[static_cast<u8>(type)] &= ~fromMask;
      m_BlackPieces[static_cast<u8>(type)] |= toMask;

      m_AllBlackPieces &= ~fromMask;
      m_AllBlackPieces |= toMask;
      m_AllWhitePieces &= ~toMask;
    }

    m_enPassant = -1;
    if (type == PieceType::Pawn && std::abs(toIdx - fromIdx) == 16)
      m_enPassant = (color == PieceColor::White) ? fromIdx + 8 : fromIdx - 8;

    m_isWhite = !m_isWhite;
  }

private:
  std::array<u64, PIECES_ARRAY_SIZE> m_WhitePieces, m_BlackPieces;
  u64 m_AllWhitePieces, m_AllBlackPieces;

  bool m_isWhite;
  i64 m_Castling, m_enPassant;
};
} // namespace Neptune::Core
