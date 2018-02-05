#pragma once

#include "Move.h"

class CaptureMove : public Move
{
public:
	CaptureMove(const unsigned long long& origin, const unsigned long long& destination, const PieceType& pieceType)
		: _origin(origin),
		_destination(destination),
		_destinationOpt(destination),
		_pieceType(pieceType)
	{}

	const BitBoards playPieces(const BitBoards& board, const bool& isWhiteTurn) const override
	{
		return board.capturePiece(_origin, _destination, _pieceType, isWhiteTurn);
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
		return true;
	}

protected:

	const Option<unsigned long long> capturedPiecePosition() const override
	{
		return _destinationOpt;
	}

private:

	const unsigned long long _origin;
	const unsigned long long _destination;
	const Option<unsigned long long> _destinationOpt;
	const PieceType _pieceType;
};