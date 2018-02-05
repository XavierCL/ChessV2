#pragma once

#include "BitBoardMoveConstants.h"

#include <algorithm>

class BishopMoveFactory
{
public:
	BishopMoveFactory(const Board& currentBoard)
		: _currentBoard(currentBoard)
	{}

	const std::vector<Move const *> getLegalMoves() const
	{
		std::vector<Move const *> moves;
		_currentBoard.bitBoards().foreachBoardBit(_currentBoard.isWhiteTurn(), PieceType::BISHOP, [this, &moves](const unsigned char& position) {
			const auto upLeftMoves = BitBoardMoveConstants::getCaptures(
				_currentBoard.isWhiteTurn(),
				position,
				_currentBoard.bitBoards(),
				BitBoardRayConstants::UP_LEFT_WARD_RAYS,
				BitBoardMoveConstants::BISHOP_UP_LEFT_WARD_CAPTURES,
				true
			);
			const auto upRightMoves = BitBoardMoveConstants::getCaptures(
				_currentBoard.isWhiteTurn(),
				position,
				_currentBoard.bitBoards(),
				BitBoardRayConstants::UP_RIGHT_WARD_RAYS,
				BitBoardMoveConstants::BISHOP_UP_RIGHT_WARD_CAPTURES,
				true
			);
			const auto downLeftMoves = BitBoardMoveConstants::getCaptures(
				_currentBoard.isWhiteTurn(),
				position,
				_currentBoard.bitBoards(),
				BitBoardRayConstants::DOWN_LEFT_WARD_RAYS,
				BitBoardMoveConstants::BISHOP_DOWN_LEFT_WARD_CAPTURES,
				false
			);
			const auto downRightMoves = BitBoardMoveConstants::getCaptures(
				_currentBoard.isWhiteTurn(),
				position,
				_currentBoard.bitBoards(),
				BitBoardRayConstants::DOWN_RIGHT_WARD_RAYS,
				BitBoardMoveConstants::BISHOP_DOWN_RIGHT_WARD_CAPTURES,
				false
			);
			moves.reserve(upLeftMoves.size() + upRightMoves.size() + downLeftMoves.size() + downRightMoves.size());
			std::copy_if(upLeftMoves.begin(), upLeftMoves.end(), std::back_inserter(moves), [this](const Move const * move) {return !move->wouldKingBeChecked(_currentBoard); });
			std::copy_if(upRightMoves.begin(), upRightMoves.end(), std::back_inserter(moves), [this](const Move const * move) {return !move->wouldKingBeChecked(_currentBoard); });
			std::copy_if(downLeftMoves.begin(), downLeftMoves.end(), std::back_inserter(moves), [this](const Move const * move) {return !move->wouldKingBeChecked(_currentBoard); });
			std::copy_if(downRightMoves.begin(), downRightMoves.end(), std::back_inserter(moves), [this](const Move const * move) {return !move->wouldKingBeChecked(_currentBoard); });
		});
		return moves;
	}

private:
	const Board _currentBoard;
};