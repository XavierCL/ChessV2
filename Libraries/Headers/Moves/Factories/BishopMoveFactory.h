#pragma once

#include "GenericMoveFactory.h"
#include "BitBoardMoveConstants.h"

class BishopMoveFactory: public GenericMoveFactory
{
public:
	BishopMoveFactory(const Board& currentBoard)
		: GenericMoveFactory(13 * currentBoard.bitBoards().populationCount(PieceType::BISHOP, currentBoard.isWhiteTurn()), currentBoard)
	{}

protected:
	void generateLegalMoves() override
	{
		_currentBoard.bitBoards().foreachBoardBit(_currentBoard.isWhiteTurn(), PieceType::BISHOP, [this](const unsigned char& position) {
			pushAllCheckless(BitBoardMoveConstants::getCaptures(
				_currentBoard.isWhiteTurn(),
				position,
				_currentBoard.bitBoards(),
				BitBoardRayConstants::UP_LEFT_WARD_RAYS,
				BitBoardMoveConstants::BISHOP_UP_LEFT_WARD_CAPTURES,
				true
			));
			pushAllCheckless(BitBoardMoveConstants::getCaptures(
				_currentBoard.isWhiteTurn(),
				position,
				_currentBoard.bitBoards(),
				BitBoardRayConstants::UP_RIGHT_WARD_RAYS,
				BitBoardMoveConstants::BISHOP_UP_RIGHT_WARD_CAPTURES,
				true
			));
			pushAllCheckless(BitBoardMoveConstants::getCaptures(
				_currentBoard.isWhiteTurn(),
				position,
				_currentBoard.bitBoards(),
				BitBoardRayConstants::DOWN_LEFT_WARD_RAYS,
				BitBoardMoveConstants::BISHOP_DOWN_LEFT_WARD_CAPTURES,
				false
			));
			pushAllCheckless(BitBoardMoveConstants::getCaptures(
				_currentBoard.isWhiteTurn(),
				position,
				_currentBoard.bitBoards(),
				BitBoardRayConstants::DOWN_RIGHT_WARD_RAYS,
				BitBoardMoveConstants::BISHOP_DOWN_RIGHT_WARD_CAPTURES,
				false
			));
		});
	}
};