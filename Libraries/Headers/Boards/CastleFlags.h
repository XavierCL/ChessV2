#pragma once

#include <cstring>

#include <functional>

class CastleFlags
{
public:

	CastleFlags()
		: _allowedCastles{ true, true, true, true }
	{}

	CastleFlags(const bool& whiteKingSide, const bool& whiteQueenSide, const bool& blackKingSide, const bool& blackQueenSide)
		: _allowedCastles{ whiteKingSide, whiteQueenSide, blackKingSide, blackQueenSide }
	{}

	const bool operator==(const CastleFlags& other) const
	{
		return memcmp(
			&_allowedCastles[0],
			&other._allowedCastles[0],
			sizeof(_allowedCastles) * sizeof(bool)
		) == 0;
	}

	const size_t hash() const
	{
		return ALLOWED_CASTLE_HASH(_allowedCastles[0]) * 3
			+ ALLOWED_CASTLE_HASH(_allowedCastles[1]) * 5
			+ ALLOWED_CASTLE_HASH(_allowedCastles[2]) * 7
			+ ALLOWED_CASTLE_HASH(_allowedCastles[3]) * 11;
	}

	const bool& canWhiteCastleKingSide() const
	{
		return _allowedCastles[WHITE_KING_SIDE_INDEX];
	}

	const bool& canWhiteCastleQueenSide() const
	{
		return _allowedCastles[WHITE_QUEEN_SIDE_INDEX];
	}

	const bool& canBlackCastleKingSide() const
	{
		return _allowedCastles[BLACK_KING_SIDE_INDEX];
	}

	const bool& canBlackCastleQueenSide() const
	{
		return _allowedCastles[BLACK_QUEEN_SIDE_INDEX];
	}

private:

	static const std::hash<bool> ALLOWED_CASTLE_HASH;

	const static unsigned char WHITE_KING_SIDE_INDEX = 0;
	const static unsigned char WHITE_QUEEN_SIDE_INDEX = 1;
	const static unsigned char BLACK_KING_SIDE_INDEX = 2;
	const static unsigned char BLACK_QUEEN_SIDE_INDEX = 3;

	const bool _allowedCastles[4];
};