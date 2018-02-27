#pragma once

#include "GenericMoveFactory.h"
#include "BitBoardMoveConstants.h"

class PawnMoveFactory: public GenericMoveFactory
{
public:
	PawnMoveFactory(const Board& currentBoard)
		: GenericMoveFactory(4 * currentBoard.bitBoards().populationCount(PieceType::PAWN, currentBoard.isWhiteTurn()), currentBoard)
	{}

protected:
	void generateLegalMoves() override
	{
		_currentBoard.bitBoards().foreachBoardBit(_currentBoard.isWhiteTurn(), PieceType::PAWN, [this](const unsigned char& position) {
			pushAllCheckless(BitBoardMoveConstants::getMoves(
				position,
				_currentBoard.bitBoards(),
				BitBoardRayConstants::PAWN_RAYS[_currentBoard.isWhiteTurn()],
				BitBoardMoveConstants::PAWN_MOVES[_currentBoard.isWhiteTurn()],
				_currentBoard.isWhiteTurn()
			));
			pushAllCheckless(BitBoardMoveConstants::getImmediateCaptures(
				_currentBoard.isWhiteTurn(),
				position,
				_currentBoard.bitBoards(),
				BitBoardRayConstants::PAWN_IMMEDIATE_CAPTURES[_currentBoard.isWhiteTurn()],
				BitBoardMoveConstants::PAWN_IMMEDIATE_CAPTURE_DATA[_currentBoard.isWhiteTurn()]
			));
		});
	}
};