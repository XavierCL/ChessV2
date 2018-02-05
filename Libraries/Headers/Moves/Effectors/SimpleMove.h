#pragma once

#include "Move.h"

class SimpleMove : public Move
{
public:
	SimpleMove(const unsigned long long& origin, const unsigned long long& destination, const PieceType& pieceType)
		: _origin(origin),
		_destination(destination),
		_pieceType(pieceType)
	{}

	const BitBoards playPieces(const BitBoards& board, const bool& isWhiteTurn) const override
	{
		return board.movePiece(_origin, _destination, _pieceType, isWhiteTurn);
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
		return NO_CAPTURE_DESTINATION;
	}

private:

	const static Option<unsigned long long> NO_CAPTURE_DESTINATION;

	const unsigned long long _origin;
	const unsigned long long _destination;
	const PieceType _pieceType;
};