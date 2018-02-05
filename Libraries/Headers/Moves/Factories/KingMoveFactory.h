#pragma once

#include "BitBoardMoveConstants.h"

class KingMoveFactory
{
public:
	KingMoveFactory(const Board& currentBoard)
		: _currentBoard(currentBoard)
	{}

	const std::vector<Move const *> getLegalMoves() const
	{
		std::vector<Move const *> moves;
		_currentBoard.bitBoards().foreachBoardBit(_currentBoard.isWhiteTurn(), PieceType::KING, [this, &moves](const unsigned char& position) {
			std::vector<Move const *> localKills(BitBoardMoveConstants::getImmediateKills(
				_currentBoard.isWhiteTurn(),
				position,
				_currentBoard.bitBoards(),
				BitBoardRayConstants::KING_IMMEDIATE_KILL,
				BitBoardMoveConstants::KING_IMMEDIATE_KILL_DATA
			));
			moves.reserve(localKills.size());
			std::copy_if(localKills.begin(), localKills.end(), std::back_inserter(moves), [this](const Move const * move) {return !move->wouldKingBeChecked(_currentBoard); });
		});
		return moves;
	}

private:
	const Board _currentBoard;
};