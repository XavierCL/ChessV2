#pragma once

#pragma once

#include "Move.h"

class CapturePromotionMove : public Move
{
public:
	CapturePromotionMove(const unsigned long long& origin, const unsigned long long& destination, const PieceType& pieceTypeToPromoteTo)
		: _origin(origin),
		_destination(destination),
		_destinationOpt(destination),
		_pieceTypeToPromoteTo(pieceTypeToPromoteTo)
	{}

	const BitBoards playPieces(const BitBoards& board, const bool& isWhiteTurn) const override
	{
		return board.captureAndPromotePiece(_origin, _destination, _pieceTypeToPromoteTo, isWhiteTurn);
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
	const PieceType _pieceTypeToPromoteTo;
};