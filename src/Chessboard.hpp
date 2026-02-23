#pragma once

#include <array>
#include <string>

#include "FEN.hpp"
#include "MoveHandler.hpp"
#include "Utilities.hpp"

namespace Neptune::Core {

class Chessboard {
  static constexpr u8 PIECES_COUNT = 7;

public:
  Chessboard() {
    loadFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    MoveHandler::Init();
  }

  void loadFEN(std::string fen_string) {
    m_WhitePieces.fill(0);
    m_BlackPieces.fill(0);

    auto [pieces, isWhite, castling, en_passant] = Core::FEN::Parse(fen_string);

    for (i32 idx = 0; idx < 64; idx++) {
      auto [type, color] = pieces[idx];
      if (type == PieceType::None || color == PieceColor::None)
        continue;

      if (color == PieceColor::White)
        m_WhitePieces[static_cast<u8>(type)] |= (1ULL << idx);
      else if (color == PieceColor::Black)
        m_BlackPieces[static_cast<u8>(type)] |= (1ULL << idx);
    }

    m_isWhite = isWhite;
    m_Castling = castling;
    m_enPassant = en_passant;
  }

  std::pair<PieceType, PieceColor> at(std::pair<i32, i32> curr) {
    u64 mask = (1ULL << to1D(curr));
    for (u8 i = 1; i < PIECES_COUNT; i++) {
      if (m_WhitePieces[i] & mask)
        return {static_cast<PieceType>(i), PieceColor::White};
      else if (m_BlackPieces[i] & mask)
        return {static_cast<PieceType>(i), PieceColor::Black};
    }
    return {PieceType::None, PieceColor::None};
  }

  bool isMoveValid(std::pair<i32, i32> selected, std::pair<i32, i32> target) {
    auto [type, color] = at(selected);
    auto [targetType, targetColor] = at(target);

    if (color == targetColor && targetType != PieceType::None)
      return false;

    i32 fromIdx = to1D(selected);

    u64 targetMask = 1ULL << to1D(target);
    u64 occupancy = 0;

    for (int i = 1; i < PIECES_COUNT; i++)
      occupancy |= m_WhitePieces[i] | m_BlackPieces[i];

    switch (type) {
    case PieceType::Pawn:
      return (MoveHandler::PawnMoves(fromIdx, color, occupancy, m_enPassant) &
              targetMask);
    case PieceType::Knight:
      return (MoveHandler::KnightMoves(fromIdx) & targetMask);
    case PieceType::Bishop:
      return (MoveHandler::BishopMoves(fromIdx, occupancy) & targetMask);
    case PieceType::Rook:
      return (MoveHandler::RookMoves(fromIdx, occupancy) & targetMask);
    case PieceType::Queen:
      return (MoveHandler::QueenMoves(fromIdx, occupancy) & targetMask);
    case PieceType::King:
      return (MoveHandler::KingMoves(fromIdx) & targetMask);
    default:
      return false;
    }
  }

  void move(std::pair<i32, i32> from, std::pair<i32, i32> to) {
    auto [type, color] = at(from);
    if (type == PieceType::None)
      return;

    u64 fromMask = 1ULL << to1D(from), toMask = 1ULL << to1D(to);
    for (u8 i = 1; i < PIECES_COUNT; i++) {
      m_WhitePieces[i] &= ~toMask;
      m_BlackPieces[i] &= ~toMask;
    }

    if (color == PieceColor::White) {
      m_WhitePieces[static_cast<u8>(type)] &= ~fromMask;
      m_WhitePieces[static_cast<u8>(type)] |= toMask;
    } else {
      m_BlackPieces[static_cast<u8>(type)] &= ~fromMask;
      m_BlackPieces[static_cast<u8>(type)] |= toMask;
    }

    m_isWhite = !m_isWhite;
  }

private:
  std::array<u64, PIECES_COUNT> m_BlackPieces, m_WhitePieces;

  bool m_isWhite;
  i64 m_Castling, m_enPassant;

  i32 to1D(const std::pair<i32, i32> &curr) {
    return (7 - curr.second) * 8 + curr.first;
  }
};
} // namespace Neptune::Core
