#pragma once

#include "BitBoardMoveConstants.h"

#include <algorithm>

class PawnMoveFactory
{
public:
	PawnMoveFactory(const Board& currentBoard, const Option<Board>& lastBoard)
		: _currentBoard(currentBoard),
		_lastBoard(lastBoard)
	{}

	const std::vector<Move const *> getLegalMoves() const
	{
		std::vector<Move const *> moves;
		_currentBoard.bitBoards().foreachBoardBit(_currentBoard.isWhiteTurn(), PieceType::PAWN, [this, &moves](const unsigned char& position) {
			const auto localMoves = BitBoardMoveConstants::getMoves(
				position,
				_currentBoard.bitBoards(),
				BitBoardRayConstants::PAWN_RAYS[_currentBoard.isWhiteTurn()],
				BitBoardMoveConstants::PAWN_MOVES[_currentBoard.isWhiteTurn()],
				_currentBoard.isWhiteTurn()
			);
			const auto localCaptures = BitBoardMoveConstants::getImmediateCaptures(
				_currentBoard.isWhiteTurn(),
				position,
				_currentBoard.bitBoards(),
				BitBoardRayConstants::PAWN_IMMEDIATE_CAPTURES[_currentBoard.isWhiteTurn()],
				BitBoardMoveConstants::PAWN_IMMEDIATE_CAPTURE_DATA[_currentBoard.isWhiteTurn()]
			);
			moves.reserve(localCaptures.size() + localMoves.size());
			std::copy_if(localCaptures.begin(), localCaptures.end(), std::back_inserter(moves), [this](const Move const * move) {return !move->wouldKingBeChecked(_currentBoard); });
			std::copy_if(localMoves.begin(), localMoves.end(), std::back_inserter(moves), [this](const Move const * move) {return !move->wouldKingBeChecked(_currentBoard); });
		});
		return moves;
	}

private:
	const Board _currentBoard;
	const Option<Board> _lastBoard;
};