#pragma once

#include "GenericMoveFactory.h"
#include "BitBoardMoveConstants.h"

class QueenMoveFactory: public GenericMoveFactory
{
public:
	QueenMoveFactory(const Board& currentBoard)
		: GenericMoveFactory(27 * currentBoard.bitBoards().populationCount(PieceType::QUEEN, currentBoard.isWhiteTurn()), currentBoard)
	{}

protected:
	void generateLegalMoves() override
	{
		_currentBoard.bitBoards().foreachBoardBit(_currentBoard.isWhiteTurn(), PieceType::QUEEN, [this](const unsigned char& position) {
			pushAllCheckless(BitBoardMoveConstants::getCaptures(
				_currentBoard.isWhiteTurn(),
				position,
				_currentBoard.bitBoards(),
				BitBoardRayConstants::UP_LEFT_WARD_RAYS,
				BitBoardMoveConstants::QUEEN_UP_LEFT_WARD_CAPTURES,
				true
			));
			pushAllCheckless(BitBoardMoveConstants::getCaptures(
				_currentBoard.isWhiteTurn(),
				position,
				_currentBoard.bitBoards(),
				BitBoardRayConstants::UP_RIGHT_WARD_RAYS,
				BitBoardMoveConstants::QUEEN_UP_RIGHT_WARD_CAPTURES,
				true
			));
			pushAllCheckless(BitBoardMoveConstants::getCaptures(
				_currentBoard.isWhiteTurn(),
				position,
				_currentBoard.bitBoards(),
				BitBoardRayConstants::DOWN_LEFT_WARD_RAYS,
				BitBoardMoveConstants::QUEEN_DOWN_LEFT_WARD_CAPTURES,
				false
			));
			pushAllCheckless(BitBoardMoveConstants::getCaptures(
				_currentBoard.isWhiteTurn(),
				position,
				_currentBoard.bitBoards(),
				BitBoardRayConstants::DOWN_RIGHT_WARD_RAYS,
				BitBoardMoveConstants::QUEEN_DOWN_RIGHT_WARD_CAPTURES,
				false
			));
			pushAllCheckless(BitBoardMoveConstants::getCaptures(
				_currentBoard.isWhiteTurn(),
				position,
				_currentBoard.bitBoards(),
				BitBoardRayConstants::UPWARD_RAYS,
				BitBoardMoveConstants::QUEEN_UPWARD_CAPTURES,
				true
			));
			pushAllCheckless(BitBoardMoveConstants::getCaptures(
				_currentBoard.isWhiteTurn(),
				position,
				_currentBoard.bitBoards(),
				BitBoardRayConstants::DOWNWARD_RAYS,
				BitBoardMoveConstants::QUEEN_DOWNWARD_CAPTURES,
				false
			));
			pushAllCheckless(BitBoardMoveConstants::getCaptures(
				_currentBoard.isWhiteTurn(),
				position,
				_currentBoard.bitBoards(),
				BitBoardRayConstants::LEFTWARD_RAYS,
				BitBoardMoveConstants::QUEEN_LEFTWARD_CAPTURES,
				true
			));
			pushAllCheckless(BitBoardMoveConstants::getCaptures(
				_currentBoard.isWhiteTurn(),
				position,
				_currentBoard.bitBoards(),
				BitBoardRayConstants::RIGHTWARD_RAYS,
				BitBoardMoveConstants::QUEEN_RIGHTWARD_CAPTURES,
				false
			));
		});
	}
};