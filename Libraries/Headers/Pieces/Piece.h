#ifndef PIECE2_H
#define PIECE2_H

#include "../Physics/Point.h"
#include "../Pieces/PieceType.h"

class Piece
{
public:
	Piece(const unsigned char& position, const bool& isWhite, const PieceType& type)
		: _position(position),
		_isWhite(isWhite),
		_type(type)
	{}

	const bool isWhite() const
	{
		return _isWhite;
	}

	const PieceType pieceType() const
	{
		return _type;
	}

	const bool isEmpty() const
	{
		return _type == PieceType::EMPTY;
	}
private:
	const unsigned char _position;
	const bool _isWhite;
	const PieceType _type;
};

#endif