#pragma once

#pragma once

#include "Move.h"

class PromotionMove : public Move
{
public:
	PromotionMove(const unsigned long long& origin, const unsigned long long& destination, const PieceType& pieceTypeToPromoteTo)
		: _origin(origin),
		_destination(destination),
		_pieceTypeToPromoteTo(pieceTypeToPromoteTo)
	{}

	const BitBoards playPieces(const BitBoards& board, const bool& isWhiteTurn) const override
	{
		return board.promotePiece(_origin, _destination, _pieceTypeToPromoteTo, isWhiteTurn);
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
		return true; // does change the score of a board so...
	}

protected:

	const Option<unsigned long long> capturedPiecePosition() const override
	{
		return NO_CAPTURE_DESTINATION;
	}

private:

	static const Option<unsigned long long> NO_CAPTURE_DESTINATION;

	const unsigned long long _origin;
	const unsigned long long _destination;
	const PieceType _pieceTypeToPromoteTo;
};