#pragma once

#include "BitBoardMoveConstants.h"

class QueenMoveFactory
{
public:
	QueenMoveFactory(const Board& currentBoard)
		: _currentBoard(currentBoard)
	{}

	const std::vector<Move const *> getLegalMoves() const
	{
		std::vector<Move const *> moves;
		_currentBoard.bitBoards().foreachBoardBit(_currentBoard.isWhiteTurn(), PieceType::QUEEN, [this, &moves](const unsigned char& position) {
			const auto upLeftMoves = BitBoardMoveConstants::getCaptures(
				_currentBoard.isWhiteTurn(),
				position,
				_currentBoard.bitBoards(),
				BitBoardRayConstants::UP_LEFT_WARD_RAYS,
				BitBoardMoveConstants::QUEEN_UP_LEFT_WARD_CAPTURES,
				true
			);
			const auto upRightMoves = BitBoardMoveConstants::getCaptures(
				_currentBoard.isWhiteTurn(),
				position,
				_currentBoard.bitBoards(),
				BitBoardRayConstants::UP_RIGHT_WARD_RAYS,
				BitBoardMoveConstants::QUEEN_UP_RIGHT_WARD_CAPTURES,
				true
			);
			const auto downLeftMoves = BitBoardMoveConstants::getCaptures(
				_currentBoard.isWhiteTurn(),
				position,
				_currentBoard.bitBoards(),
				BitBoardRayConstants::DOWN_LEFT_WARD_RAYS,
				BitBoardMoveConstants::QUEEN_DOWN_LEFT_WARD_CAPTURES,
				false
			);
			const auto downRightMoves = BitBoardMoveConstants::getCaptures(
				_currentBoard.isWhiteTurn(),
				position,
				_currentBoard.bitBoards(),
				BitBoardRayConstants::DOWN_RIGHT_WARD_RAYS,
				BitBoardMoveConstants::QUEEN_DOWN_RIGHT_WARD_CAPTURES,
				false
			);
			const auto upwardMoves = BitBoardMoveConstants::getCaptures(
				_currentBoard.isWhiteTurn(),
				position,
				_currentBoard.bitBoards(),
				BitBoardRayConstants::UPWARD_RAYS,
				BitBoardMoveConstants::QUEEN_UPWARD_CAPTURES,
				true
			);
			const auto downwardMoves = BitBoardMoveConstants::getCaptures(
				_currentBoard.isWhiteTurn(),
				position,
				_currentBoard.bitBoards(),
				BitBoardRayConstants::DOWNWARD_RAYS,
				BitBoardMoveConstants::QUEEN_DOWNWARD_CAPTURES,
				false
			);
			const auto leftwardMoves = BitBoardMoveConstants::getCaptures(
				_currentBoard.isWhiteTurn(),
				position,
				_currentBoard.bitBoards(),
				BitBoardRayConstants::LEFTWARD_RAYS,
				BitBoardMoveConstants::QUEEN_LEFTWARD_CAPTURES,
				true
			);
			const auto rightWardMoves = BitBoardMoveConstants::getCaptures(
				_currentBoard.isWhiteTurn(),
				position,
				_currentBoard.bitBoards(),
				BitBoardRayConstants::RIGHTWARD_RAYS,
				BitBoardMoveConstants::QUEEN_RIGHTWARD_CAPTURES,
				false
			);
			moves.reserve(upwardMoves.size() + downwardMoves.size() + leftwardMoves.size() + rightWardMoves.size() + upLeftMoves.size() + upRightMoves.size() + downLeftMoves.size() + downRightMoves.size());
			std::copy_if(upwardMoves.begin(), upwardMoves.end(), std::back_inserter(moves), [this](const Move const * move) {return !move->wouldKingBeChecked(_currentBoard); });
			std::copy_if(downwardMoves.begin(), downwardMoves.end(), std::back_inserter(moves), [this](const Move const * move) {return !move->wouldKingBeChecked(_currentBoard); });
			std::copy_if(leftwardMoves.begin(), leftwardMoves.end(), std::back_inserter(moves), [this](const Move const * move) {return !move->wouldKingBeChecked(_currentBoard); });
			std::copy_if(rightWardMoves.begin(), rightWardMoves.end(), std::back_inserter(moves), [this](const Move const * move) {return !move->wouldKingBeChecked(_currentBoard); });
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