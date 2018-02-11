#pragma once

#include "GenericMoveFactory.h"
#include "BitBoardMoveConstants.h"

class RookMoveFactory: public GenericMoveFactory
{
public:
	RookMoveFactory(const Board& currentBoard)
		: GenericMoveFactory(14 * currentBoard.bitBoards().populationCount(PieceType::ROOK, currentBoard.isWhiteTurn()), currentBoard)
	{}

protected:
	void generateLegalMoves() override
	{
		_currentBoard.bitBoards().foreachBoardBit(_currentBoard.isWhiteTurn(), PieceType::ROOK, [this](const unsigned char& position) {
			append(BitBoardMoveConstants::getCaptures(
				_currentBoard.isWhiteTurn(),
				position,
				_currentBoard.bitBoards(),
				BitBoardRayConstants::UPWARD_RAYS,
				BitBoardMoveConstants::ROOK_UPWARD_CAPTURES,
				true
			));
			append(BitBoardMoveConstants::getCaptures(
				_currentBoard.isWhiteTurn(),
				position,
				_currentBoard.bitBoards(),
				BitBoardRayConstants::DOWNWARD_RAYS,
				BitBoardMoveConstants::ROOK_DOWNWARD_CAPTURES,
				false
			));
			append(BitBoardMoveConstants::getCaptures(
				_currentBoard.isWhiteTurn(),
				position,
				_currentBoard.bitBoards(),
				BitBoardRayConstants::LEFTWARD_RAYS,
				BitBoardMoveConstants::ROOK_LEFTWARD_CAPTURES,
				true
			));
			append(BitBoardMoveConstants::getCaptures(
				_currentBoard.isWhiteTurn(),
				position,
				_currentBoard.bitBoards(),
				BitBoardRayConstants::RIGHTWARD_RAYS,
				BitBoardMoveConstants::ROOK_RIGHTWARD_CAPTURES,
				false
			));
		});
	}
};