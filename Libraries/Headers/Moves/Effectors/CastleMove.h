#pragma once

#include "Move.h"

class CastleMove : public Move
{
public:
	CastleMove(const unsigned long long& origin, const unsigned long long& destination)
		: _origin(origin),
		_destination(destination)
	{}

	const BitBoards playPieces(const BitBoards& board, const bool& isWhiteTurn) const override
	{
		return board.castle(_origin, _destination, isWhiteTurn);
	}

	const unsigned long long movingPieceOrigin() const override
	{
		return _origin;
	}

	const unsigned long long movingPieceDestination() const override
	{
		return _destination;
	}

	const bool isOffensiveMove(const Board& board) const override
	{
		return false;
	}

protected:

	const Option<unsigned long long> capturedPiecePosition() const override
	{
		return NO_CAPTURE_POSITION;
	}

private:

	static const Option<unsigned long long> NO_CAPTURE_POSITION;

	const unsigned long long _origin;
	const unsigned long long _destination;
};
