#pragma once

#include "BitBoardMoveConstants.h"

#include <algorithm>

class RookMoveFactory
{
public:
	RookMoveFactory(const Board& currentBoard)
		: _currentBoard(currentBoard)
	{}

	const std::vector<Move const *> getLegalMoves() const
	{
		std::vector<Move const *> moves;
		_currentBoard.bitBoards().foreachBoardBit(_currentBoard.isWhiteTurn(), PieceType::ROOK, [this, &moves](const unsigned char& position) {
			const auto upwardMoves = BitBoardMoveConstants::getCaptures(
				_currentBoard.isWhiteTurn(),
				position,
				_currentBoard.bitBoards(),
				BitBoardRayConstants::UPWARD_RAYS,
				BitBoardMoveConstants::ROOK_UPWARD_CAPTURES,
				true
			);
			const auto downwardMoves = BitBoardMoveConstants::getCaptures(
				_currentBoard.isWhiteTurn(),
				position,
				_currentBoard.bitBoards(),
				BitBoardRayConstants::DOWNWARD_RAYS,
				BitBoardMoveConstants::ROOK_DOWNWARD_CAPTURES,
				false
			);
			const auto leftwardMoves = BitBoardMoveConstants::getCaptures(
				_currentBoard.isWhiteTurn(),
				position,
				_currentBoard.bitBoards(),
				BitBoardRayConstants::LEFTWARD_RAYS,
				BitBoardMoveConstants::ROOK_LEFTWARD_CAPTURES,
				true
			);
			const auto rightWardMoves = BitBoardMoveConstants::getCaptures(
				_currentBoard.isWhiteTurn(),
				position,
				_currentBoard.bitBoards(),
				BitBoardRayConstants::RIGHTWARD_RAYS,
				BitBoardMoveConstants::ROOK_RIGHTWARD_CAPTURES,
				false
			);
			moves.reserve(upwardMoves.size() + downwardMoves.size() + leftwardMoves.size() + rightWardMoves.size());
			std::copy_if(upwardMoves.begin(), upwardMoves.end(), std::back_inserter(moves), [this](const Move const * move) {return !move->wouldKingBeChecked(_currentBoard); });
			std::copy_if(downwardMoves.begin(), downwardMoves.end(), std::back_inserter(moves), [this](const Move const * move) {return !move->wouldKingBeChecked(_currentBoard); });
			std::copy_if(leftwardMoves.begin(), leftwardMoves.end(), std::back_inserter(moves), [this](const Move const * move) {return !move->wouldKingBeChecked(_currentBoard); });
			std::copy_if(rightWardMoves.begin(), rightWardMoves.end(), std::back_inserter(moves), [this](const Move const * move) {return !move->wouldKingBeChecked(_currentBoard); });
		});
		return moves;
	}

private:
	const Board _currentBoard;
};