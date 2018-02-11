#ifndef MOVE_H
#define MOVE_H

#include "CastleChecker.h"

class Move
{
public:
	const Board play(const Board& board) const
	{
		return Board(
			playPieces(board.bitBoards(), board.isWhiteTurn()),
			!board.isWhiteTurn(),
			CastleChecker::updateBoardCastleFlags(board.castleFlags(), movingPieceOrigin(), capturedPiecePosition())
		);
	}

	virtual const BitBoards playPieces(const BitBoards& board, const bool& isWhiteTurn) const = 0;

	virtual const unsigned long long movingPieceOrigin() const = 0; // update castle allowed properties and display
	virtual const unsigned long long movingPieceDestination() const = 0; // display
	virtual const bool isOffensiveMove(const Board& board) const = 0;

	const bool wouldKingBeChecked(const Board& board) const
	{
		return playPieces(board.bitBoards(), board.isWhiteTurn()).isKingChecked(board.isWhiteTurn());
	}

protected:
	Move()
	{}

	virtual const Option<unsigned long long> capturedPiecePosition() const = 0; // update castle allowed properties
};

#endif