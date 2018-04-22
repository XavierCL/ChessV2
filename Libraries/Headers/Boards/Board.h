#ifndef BOARD_H
#define BOARD_H

#include "BitBoard.h"
#include "CastleFlags.h"

#include <vector>

class Board
{
public:

	static const unsigned char NUMBER_COLUMN;
	static const unsigned char NUMBER_ROW;

	Board()
		: _bitBoards(),
		_isWhiteTurn(true),
		_castleFlags()
	{}

	Board(const BitBoards& bitBoards, const bool& isWhiteTurn, const CastleFlags& castleFlags)
		: _bitBoards(bitBoards),
		_isWhiteTurn(isWhiteTurn),
		_castleFlags(castleFlags)
	{}

	const bool operator==(const Board& other) const // used in the threefold draw rule
	{
		// here en passant should be taken in consideration, as it applies in the threefold rule
		return _bitBoards == other._bitBoards
			&& _isWhiteTurn == other._isWhiteTurn
			&& _castleFlags == other._castleFlags;
	}

	const bool operator!=(const Board& other) const
	{
		return !(other == *this);
	}

	const size_t hash() const
	{
		return _bitBoards.hash() * 3
			+ IS_WHITE_HASH(_isWhiteTurn) * 5
			+ _castleFlags.hash() * 7;
	}

	const bool& isWhiteTurn() const
	{
		return _isWhiteTurn;
	}

	const CastleFlags& castleFlags() const
	{
		return _castleFlags;
	}

	const BitBoards& bitBoards() const
	{
		return _bitBoards;
	}

private:

	static const std::hash<bool> IS_WHITE_HASH;

	const BitBoards _bitBoards;
	const bool _isWhiteTurn;
	const CastleFlags _castleFlags;
};

#endif