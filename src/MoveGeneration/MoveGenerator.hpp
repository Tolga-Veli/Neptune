#pragma once

#include "AttackTables.hpp"
#include "Core/Logging.hpp"
#include "Core/Utilities.hpp"
#include "Move.hpp"

namespace Neptune::Core {
class Chessboard;
}

namespace Neptune::Engine {

class MoveGenerator {
public:
  consteval static void Init() {
    AttackTables::Init();
    NEPTUNE_INFO("Initialized Move Generator");
  }

  static std::vector<Move> GeneratePseudoLegalMoves(const Core::Chessboard &board);

  static std::vector<Move> GenerateLegalMoves(const Core::Chessboard &board);

private:
  template <Core::PieceType type>
    requires(type != Core::PieceType::King)
  static void GeneratePieceMoves(const Core::Chessboard &board, std::vector<Move> &moves);

  static void GeneratePawnMoves(const Core::Chessboard &board, std::vector<Move> &moves);

  static void GenerateKnightMoves(const Core::Chessboard &board, std::vector<Move> &moves);

  static void GenerateBishopMoves(const Core::Chessboard &board, std::vector<Move> &moves);

  static void GenerateRookMoves(const Core::Chessboard &board, std::vector<Move> &moves);

  static void GenerateQueenMoves(const Core::Chessboard &board, std::vector<Move> &moves);

  static void GenerateKingMoves(const Core::Chessboard &board, std::vector<Move> &moves);
};

} // namespace Neptune::Engine
