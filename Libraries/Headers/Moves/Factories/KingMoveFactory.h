#pragma once

#include "GenericMoveFactory.h"
#include "BitBoardMoveConstants.h"

class KingMoveFactory: public GenericMoveFactory
{
public:
	KingMoveFactory(const Board& currentBoard)
		: GenericMoveFactory(8, currentBoard)
	{}

protected:
	void generateLegalMoves() override
	{
		_currentBoard.bitBoards().foreachBoardBit(_currentBoard.isWhiteTurn(), PieceType::KING, [this](const unsigned char& position) {
			append(BitBoardMoveConstants::getImmediateKills(
				_currentBoard.isWhiteTurn(),
				position,
				_currentBoard.bitBoards(),
				BitBoardRayConstants::KING_IMMEDIATE_KILL,
				BitBoardMoveConstants::KING_IMMEDIATE_KILL_DATA
			));
		});
	}
};