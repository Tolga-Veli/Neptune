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

  Chessboard(std::string fen) {
    loadFEN(fen);
    MoveHandler::Init();
  }

  void loadFEN(std::string fen_string) {
    m_WhitePieces.fill(0);
    m_BlackPieces.fill(0);
    m_AllWhitePieces = 0;
    m_AllBlackPieces = 0;

    auto [pieces, isWhite, castling, en_passant] = FEN::Parse(fen_string);
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

      if (color == PieceColor::White && fromIdx == 4) {
        if (((m_Castling & CastlingRights::WhiteKingSide) ==
             CastlingRights::WhiteKingSide) &&
            !(occupancy & ((1ULL << 5) | (1ULL << 6))))
          moves |= (1ULL << 6);

        if (((m_Castling & CastlingRights::WhiteQueenSide) ==
             CastlingRights::WhiteQueenSide) &&
            !(occupancy & ((1ULL << 1) | (1ULL << 2) | (1ULL << 3))))
          moves |= (1ULL << 2);
      }

      if (color == PieceColor::Black && fromIdx == 60) {
        if (((m_Castling & CastlingRights::BlackKingSide) ==
             CastlingRights::BlackKingSide) &&
            !(occupancy & ((1ULL << 61) | (1ULL << 62))))
          moves |= (1ULL << 62);

        if (((m_Castling & CastlingRights::BlackQueenSide) ==
             CastlingRights::BlackQueenSide) &&
            !(occupancy & ((1ULL << 57) | (1ULL << 58) | (1ULL << 59))))
          moves |= (1ULL << 58);
      }
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

    // 1. Remove captured pieces
    for (u8 i = 1; i < PIECES_ARRAY_SIZE; i++) {
      m_WhitePieces[i] &= ~toMask;
      m_BlackPieces[i] &= ~toMask;
    }

    m_AllWhitePieces &= ~toMask;
    m_AllBlackPieces &= ~toMask;

    // 1.1 En passant captures
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

    m_enPassant = -1;
    if (type == PieceType::Pawn && std::abs(toIdx - fromIdx) == 16)
      m_enPassant = (color == PieceColor::White) ? fromIdx + 8 : fromIdx - 8;

    // 2. Castling
    if (type == PieceType::King) {
      if (color == PieceColor::White)
        m_Castling &=
            ~(CastlingRights::WhiteKingSide | CastlingRights::WhiteQueenSide);
      else
        m_Castling &=
            ~(CastlingRights::BlackKingSide | CastlingRights::BlackQueenSide);

      if (std::abs(toIdx - fromIdx) == 2) {
        i32 rookFrom, rookTo;
        if (color == PieceColor::White) {
          if (toIdx > fromIdx) {
            rookFrom = 7;
            rookTo = 5;
          } else {
            rookFrom = 0;
            rookTo = 3;
          }
        } else {
          if (toIdx > fromIdx) {
            rookFrom = 63;
            rookTo = 61;
          } else {
            rookFrom = 56;
            rookTo = 59;
          }
        }

        u64 rookFromMask = 1ULL << rookFrom;
        u64 rookToMask = 1ULL << rookTo;

        if (color == PieceColor::White) {
          m_WhitePieces[static_cast<u8>(PieceType::Rook)] &= ~rookFromMask;
          m_WhitePieces[static_cast<u8>(PieceType::Rook)] |= rookToMask;

          m_AllWhitePieces &= ~rookFromMask;
          m_AllWhitePieces |= rookToMask;
        } else {
          m_BlackPieces[static_cast<u8>(PieceType::Rook)] &= ~rookFromMask;
          m_BlackPieces[static_cast<u8>(PieceType::Rook)] |= rookToMask;

          m_AllBlackPieces &= ~rookFromMask;
          m_AllBlackPieces |= rookToMask;
        }
      }
    }

    if (type == PieceType::Rook) {
      if (fromIdx == 0)
        m_Castling &= ~CastlingRights::WhiteQueenSide;
      if (fromIdx == 7)
        m_Castling &= ~CastlingRights::WhiteKingSide;
      if (fromIdx == 56)
        m_Castling &= ~CastlingRights::BlackQueenSide;
      if (fromIdx == 63)
        m_Castling &= ~CastlingRights::BlackKingSide;
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

    if (type == PieceType::Pawn) {
      if (color == PieceColor::White) {
        if (toIdx >= 56) {
          m_WhitePieces[static_cast<u8>(PieceType::Pawn)] &= ~toMask;
          m_WhitePieces[static_cast<u8>(PieceType::Queen)] |= toMask;
        }
      } else {
        if (toIdx <= 7) {
          m_BlackPieces[static_cast<u8>(PieceType::Pawn)] &= ~toMask;
          m_BlackPieces[static_cast<u8>(PieceType::Queen)] |= toMask;
        }
      }
    }

    if (toIdx == 0)
      m_Castling &= ~CastlingRights::WhiteQueenSide;
    if (toIdx == 7)
      m_Castling &= ~CastlingRights::WhiteKingSide;
    if (toIdx == 56)
      m_Castling &= ~CastlingRights::BlackQueenSide;
    if (toIdx == 63)
      m_Castling &= ~CastlingRights::BlackKingSide;

    m_isWhite = !m_isWhite;
  }

private:
  bool m_isWhite;

  std::array<u64, PIECES_ARRAY_SIZE> m_WhitePieces, m_BlackPieces;
  u64 m_AllWhitePieces = 0, m_AllBlackPieces = 0;
  CastlingRights m_Castling = CastlingRights::None;
  i64 m_enPassant = 0;
};
} // namespace Neptune::Core
