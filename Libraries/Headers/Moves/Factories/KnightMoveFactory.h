#pragma once

#include "GenericMoveFactory.h"
#include "BitBoardMoveConstants.h"

class KnightMoveFactory: public GenericMoveFactory
{
public:
	KnightMoveFactory(const Board& currentBoard)
		: GenericMoveFactory(8 * currentBoard.bitBoards().populationCount(PieceType::KNIGHT, currentBoard.isWhiteTurn()), currentBoard)
	{}

protected:
	void generateLegalMoves() override
	{
		_currentBoard.bitBoards().foreachBoardBit(_currentBoard.isWhiteTurn(), PieceType::KNIGHT, [this](const unsigned char& position) {
			pushAllCheckless(BitBoardMoveConstants::getImmediateKills(
				_currentBoard.isWhiteTurn(),
				position,
				_currentBoard.bitBoards(),
				BitBoardRayConstants::KNIGHT_IMMEDIATE_KILL,
				BitBoardMoveConstants::KNIGHT_IMMEDIATE_KILL_DATA
			));
		});
	}
};