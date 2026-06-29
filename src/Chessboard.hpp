#pragma once

#include <string>

#include "Core/Utilities.hpp"
#include "MoveGeneration/Move.hpp"
#include "MoveGeneration/MoveGenerator.hpp"

namespace Neptune::Core {

class Bitboards {
public:
  static constexpr u8 BITBOARDS_COUNT = 8;

  u64 &GetPiecesBitboard(PieceType type) {
    return m_Bitboards[static_cast<u8>(type)];
  }
  u64 GetPiecesBitboard(PieceType type) const {
    return m_Bitboards[static_cast<u8>(type)];
  }

  u64 &GetOccupancyBitboard() { return m_Bitboards[OCCUPANCY_INDEX]; }
  u64 GetOccupancyBitboard() const { return m_Bitboards[OCCUPANCY_INDEX]; }

  auto begin() { return m_Bitboards.begin() + 1; }
  auto begin() const { return m_Bitboards.begin() + 1; }
  auto end() { return m_Bitboards.begin() + OCCUPANCY_INDEX; }
  auto end() const { return m_Bitboards.begin() + OCCUPANCY_INDEX; }

  void Reset() { m_Bitboards.fill(0); }

private:
  static constexpr u8 OCCUPANCY_INDEX = 7;
  std::array<u64, BITBOARDS_COUNT> m_Bitboards;
};

class Chessboard {

public:
  Chessboard(const std::string &fen =
                 "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1") {
    LoadFEN(fen);
  }

  void Reset() {
    m_WhitePieces.Reset(), m_BlackPieces.Reset();
    m_SideToMove = PieceColor::White;
    m_Castling = {true, true, true, true};
    m_EnPassant = -1;
    LoadFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
  }

  void RebuildOccupancy();

  void LoadFEN(std::string fen_string);

  std::pair<PieceType, PieceColor> GetPieceAt(i32 square);

  const Bitboards &GetPieces(PieceColor side) const;

  bool InCheck(PieceColor side) const;

  constexpr PieceColor Opposite(PieceColor side) const {
    return side == PieceColor::White ? PieceColor::Black : PieceColor::White;
  }

  i32 GetEnPassant() const { return m_EnPassant; }

  bool IsAttacked(i32 square, PieceColor bySide) const;

  PieceColor GetSideToMove() const { return m_SideToMove; }

  u64 OccupancyBoth() const {
    return m_WhitePieces.GetOccupancyBitboard() |
           m_BlackPieces.GetOccupancyBitboard();
  }

  bool CanCastle(PieceColor side, bool kingSide) const;

  void MakeMove(const Engine::Move &move);

  void UpdateCastlingRights(i32 from, i32 to, PieceType piece);

  std::vector<Engine::Move> GetLegalMoves() const {
    return Engine::MoveGenerator::GenerateLegalMoves(*this);
  }

private:
  Bitboards m_WhitePieces, m_BlackPieces;
  PieceColor m_SideToMove{PieceColor::White};

  CastlingRights m_Castling{true, true, true, true};
  i32 m_EnPassant{-1};
};
} // namespace Neptune::Core
