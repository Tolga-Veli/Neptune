#include "MoveGenerator.hpp"
#include "AttackTables.hpp"
#include "Chessboard.hpp"
#include "MoveGeneration/Move.hpp"
#include "SlidingAttacks.hpp"

namespace Neptune::Engine {

std::vector<Move>
MoveGenerator::GeneratePseudoLegalMoves(const Core::Chessboard &board) {
  std::vector<Move> moves;
  moves.reserve(64);

  GeneratePawnMoves(board, moves);
  GenerateKnightMoves(board, moves);
  GenerateBishopMoves(board, moves);
  GenerateRookMoves(board, moves);
  GenerateQueenMoves(board, moves);
  GenerateKingMoves(board, moves);

  return moves;
}

std::vector<Move>
MoveGenerator::GenerateLegalMoves(const Core::Chessboard &board) {
  auto pseudo = GeneratePseudoLegalMoves(board);
  std::vector<Move> legal;

  auto side = board.GetSideToMove();
  for (const Move &move : pseudo) {
    Core::Chessboard copy = board;
    copy.MakeMove(move);

    if (!copy.InCheck(side))
      legal.push_back(move);
  }

  return legal;
}

template <Core::PieceType type>
  requires(type != Core::PieceType::Pawn && type != Core::PieceType::King)
void MoveGenerator::GeneratePieceMoves(const Core::Chessboard &board,
                                       std::vector<Move> &moves) {
  const auto &pieces = board.GetPieces(board.GetSideToMove());
  const u64 own = pieces.GetOccupancyBitboard(),
            enemy = board.GetPieces(board.Opposite(board.GetSideToMove()))
                        .GetOccupancyBitboard();

  u64 bitboard = pieces.GetPiecesBitboard(type);

  using enum Core::PieceType;
  while (bitboard) {
    i32 from = std::countr_zero(bitboard);
    bitboard &= bitboard - 1;

    u64 attacks = 0;

    if constexpr (type == Knight)
      attacks = AttackTables::Knight(from);
    else if constexpr (type == Bishop)
      attacks = SlidingAttacks::Bishop(from, own | enemy);
    else if constexpr (type == Rook)
      attacks = SlidingAttacks::Rook(from, own | enemy);
    else if constexpr (type == Queen)
      attacks = SlidingAttacks::Queen(from, own | enemy);

    attacks &= ~own;

    while (attacks) {
      i32 to = std::countr_zero(attacks);
      attacks &= attacks - 1;

      u64 toBit = 1ULL << to;
      MoveFlag flag = (enemy & toBit) ? MoveFlag::Capture : MoveFlag::Quiet;

      moves.emplace_back(from, to, flag);
    }
  }
}

void MoveGenerator::GenerateKnightMoves(const Core::Chessboard &board,
                                        std::vector<Move> &moves) {
  GeneratePieceMoves<Core::PieceType::Knight>(board, moves);
}

void MoveGenerator::GenerateBishopMoves(const Core::Chessboard &board,
                                        std::vector<Move> &moves) {
  GeneratePieceMoves<Core::PieceType::Bishop>(board, moves);
}

void MoveGenerator::GenerateRookMoves(const Core::Chessboard &board,
                                      std::vector<Move> &moves) {
  GeneratePieceMoves<Core::PieceType::Rook>(board, moves);
}

void MoveGenerator::GenerateQueenMoves(const Core::Chessboard &board,
                                       std::vector<Move> &moves) {
  GeneratePieceMoves<Core::PieceType::Queen>(board, moves);
}

void MoveGenerator::GeneratePawnMoves(const Core::Chessboard &board,
                                      std::vector<Move> &moves) {
  const auto &pieces = board.GetPieces(board.GetSideToMove());
  const u64 own = pieces.GetOccupancyBitboard(),
            enemy = board.GetPieces(board.Opposite(board.GetSideToMove()))
                        .GetOccupancyBitboard();
  const i32 epSquare = board.GetEnPassant();

  u64 pawns = pieces.GetPiecesBitboard(Core::PieceType::Pawn);
  while (pawns) {
    i32 from = std::countr_zero(pawns);
    pawns &= pawns - 1;

    i32 rank = from / 8;

    // PUSHES
    if (board.GetSideToMove() == Core::PieceColor::White) {
      i32 one = from + 8;
      if (!(own & (1ULL << one)) && !(enemy & (1ULL << one))) {
        if (rank == 6) {
          // promotion push
          moves.emplace_back(from, one, MoveFlag::KnightPromotion);
          moves.emplace_back(from, one, MoveFlag::BishopPromotion);
          moves.emplace_back(from, one, MoveFlag::RookPromotion);
          moves.emplace_back(from, one, MoveFlag::QueenPromotion);
        } else {
          moves.emplace_back(from, one, MoveFlag::Quiet);

          if (rank == 1) {
            i32 two = from + 16;

            if (!(own & (1ULL << two)) && !(enemy & (1ULL << two)))
              moves.emplace_back(from, two, MoveFlag::DoublePawnPush);
          }
        }
      }
    } else {
      i32 one = from - 8;
      if (!(own & (1ULL << one)) && !(enemy & (1ULL << one))) {
        if (rank == 1) {
          moves.emplace_back(from, one, MoveFlag::KnightPromotion);
          moves.emplace_back(from, one, MoveFlag::BishopPromotion);
          moves.emplace_back(from, one, MoveFlag::RookPromotion);
          moves.emplace_back(from, one, MoveFlag::QueenPromotion);
        } else {
          moves.emplace_back(from, one, MoveFlag::Quiet);

          if (rank == 6) {
            i32 two = from - 16;

            if (!(own & (1ULL << two)) && !(enemy & (1ULL << two)))
              moves.emplace_back(from, two, MoveFlag::DoublePawnPush);
          }
        }
      }
    }

    // CAPTURES
    u64 attacks = AttackTables::Pawn(from, board.GetSideToMove()),
        targets = attacks & enemy;

    while (targets) {
      i32 to = std::countr_zero(targets);
      targets &= targets - 1;

      if ((board.GetSideToMove() == Core::PieceColor::White)
              ? (from / 8 == 6)
              : (from / 8 == 1)) {
        moves.emplace_back(from, to, MoveFlag::KnightPromotionCapture);
        moves.emplace_back(from, to, MoveFlag::BishopPromotionCapture);
        moves.emplace_back(from, to, MoveFlag::RookPromotionCapture);
        moves.emplace_back(from, to, MoveFlag::QueenPromotionCapture);
      } else
        moves.emplace_back(from, to, MoveFlag::Capture);
    }

    // EN PASSANT
    if (epSquare != -1 && attacks & (1ULL << epSquare))
      moves.emplace_back(from, epSquare, MoveFlag::EnPassant);
  }
}

void MoveGenerator::GenerateKingMoves(const Core::Chessboard &board,
                                      std::vector<Move> &moves) {
  const auto side = board.GetSideToMove();
  const auto &pieces = board.GetPieces(side);
  const u64 own = pieces.GetOccupancyBitboard();

  u64 kingBB = pieces.GetPiecesBitboard(Core::PieceType::King);
  i32 from = std::countr_zero(kingBB);

  u64 attacks = AttackTables::King(from);
  attacks &= ~own;

  const auto enemy =
      board.GetPieces(board.Opposite(side)).GetOccupancyBitboard();

  // NORMAL KING MOVES
  while (attacks) {
    i32 to = std::countr_zero(attacks);
    attacks &= attacks - 1;

    MoveFlag flag =
        (enemy & (1ULL << to)) ? MoveFlag::Capture : MoveFlag::Quiet;
    moves.emplace_back(from, to, flag);
  }

  // CASTLING
  if (board.InCheck(side))
    return; // cannot castle out of check

  const u64 occ = board.OccupancyBoth();
  auto enemySide = board.Opposite(side);
  if (side == Core::PieceColor::White) {
    // KING SIDE: e1 -> g1 (4 -> 6)
    if (board.CanCastle(Core::PieceColor::White, true))
      if (!(occ & ((1ULL << 5) | (1ULL << 6))) &&
          !board.IsAttacked(5, enemySide) && !board.IsAttacked(6, enemySide))
        moves.emplace_back(4, 6, MoveFlag::KingCastle);

    // QUEEN SIDE: e1 -> c1 (4 -> 2)
    if (board.CanCastle(Core::PieceColor::White, false))
      if (!(occ & ((1ULL << 1) | (1ULL << 2) | (1ULL << 3))) &&
          !board.IsAttacked(3, enemySide) && !board.IsAttacked(2, enemySide))
        moves.emplace_back(4, 2, MoveFlag::QueenCastle);

  } else {
    // KING SIDE: e8 -> g8 (60 -> 62)
    if (board.CanCastle(Core::PieceColor::Black, true))
      if (!(occ & ((1ULL << 61) | (1ULL << 62))) &&
          !board.IsAttacked(61, enemySide) && !board.IsAttacked(62, enemySide))
        moves.emplace_back(60, 62, MoveFlag::KingCastle);

    // QUEEN SIDE: e8 -> c8 (60 -> 58)
    if (board.CanCastle(Core::PieceColor::Black, false))
      if (!(occ & ((1ULL << 57) | (1ULL << 58) | (1ULL << 59))) &&
          !board.IsAttacked(59, enemySide) && !board.IsAttacked(58, enemySide))
        moves.emplace_back(60, 58, MoveFlag::QueenCastle);
  }
}
} // namespace Neptune::Engine
