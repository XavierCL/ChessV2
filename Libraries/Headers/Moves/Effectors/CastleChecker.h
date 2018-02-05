#pragma once

#include "../../Boards/Board.h"

#include "../../utils/Option.hpp"

class CastleChecker
{
public:
	static const CastleFlags updateBoardCastleFlags(const CastleFlags& castleFlags, const unsigned long long& movingPieceOrigin, const Option<unsigned long long>& capturedPiecePosition)
	{
		const CastleFlags intermediateFlags = updateCastleFlagsGivenPosition(castleFlags, movingPieceOrigin);

		return capturedPiecePosition.map<CastleFlags>([&intermediateFlags](const unsigned long long& position) {
			return updateCastleFlagsGivenPosition(intermediateFlags, position);
		}).getOrElse(intermediateFlags);
	}

private:

	static const CastleFlags updateCastleFlagsGivenPosition(const CastleFlags& castleFlags, const unsigned long long& position)
	{
		if (position == WHITE_ROOK_KING_SIDE_INITIAL_POSITION)
		{
			return CastleFlags(false, castleFlags.canWhiteCastleQueenSide(), castleFlags.canBlackCastleKingSide(), castleFlags.canBlackCastleQueenSide());
		}
		else if (position == WHITE_ROOK_QUEEN_SIDE_INITIAL_POSITION)
		{
			return CastleFlags(castleFlags.canWhiteCastleKingSide(), false, castleFlags.canBlackCastleKingSide(), castleFlags.canBlackCastleQueenSide());
		}
		else if (position == WHITE_KING_INITIAL_POSITION)
		{
			return CastleFlags(false, false, castleFlags.canBlackCastleKingSide(), castleFlags.canBlackCastleQueenSide());
		}

		else if (position == BLACK_ROOK_KING_SIDE_INITIAL_POSITION)
		{
			return CastleFlags(castleFlags.canWhiteCastleKingSide(), castleFlags.canWhiteCastleQueenSide(), false, castleFlags.canBlackCastleQueenSide());
		}
		else if (position == BLACK_ROOK_QUEEN_SIDE_INITIAL_POSITION)
		{
			return CastleFlags(castleFlags.canWhiteCastleKingSide(), castleFlags.canWhiteCastleQueenSide(), castleFlags.canBlackCastleKingSide(), false);
		}
		else if (position == BLACK_KING_INITIAL_POSITION)
		{
			return CastleFlags(castleFlags.canWhiteCastleKingSide(), castleFlags.canWhiteCastleQueenSide(), false, false);
		}

		else
		{
			return castleFlags;
		}
	}

	static const unsigned long long WHITE_KING_INITIAL_POSITION = 0x0000000000000008;
	static const unsigned long long WHITE_ROOK_KING_SIDE_INITIAL_POSITION = 0x0000000000000001;
	static const unsigned long long WHITE_ROOK_QUEEN_SIDE_INITIAL_POSITION = 0x0000000000000080;

	static const unsigned long long BLACK_KING_INITIAL_POSITION = 0x0800000000000000;
	static const unsigned long long BLACK_ROOK_KING_SIDE_INITIAL_POSITION = 0x0100000000000000;
	static const unsigned long long BLACK_ROOK_QUEEN_SIDE_INITIAL_POSITION = 0x8000000000000000;

};