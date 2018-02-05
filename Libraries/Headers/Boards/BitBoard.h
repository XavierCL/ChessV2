#pragma once

#include "BitBoardRayConstants.h"

#include "../Pieces/Piece.h"

#include <functional>

#include <cstring>

class BitBoards
{
public:

	BitBoards()
		: _bitBoards{ { 0x00FF000000000000, 0x000000000000FF00 },
	{ 0x4200000000000000, 0x0000000000000042 },
	{ 0x2400000000000000, 0x0000000000000024 },
	{ 0x8100000000000000, 0x0000000000000081 },
	{ 0x1000000000000000, 0x0000000000000010 },
	{ 0x0800000000000000, 0x0000000000000008 },
	{ 0xFFFF000000000000, 0x000000000000FFFF } }
	{}

	const bool operator==(const BitBoards& bitBoards) const
	{
		// ignore the ALL_INDEX because it only contain redundant information
		// 6 * 2 * size_of(unsigned long long) == 96
		return memcmp(&_bitBoards[0][0], &bitBoards._bitBoards[0][0], 96) == 0;
	}

	const size_t hash() const
	{
		return _bitBoardHash(_bitBoards[0][0]) * 3
			+ _bitBoardHash(_bitBoards[1][0]) * 5
			+ _bitBoardHash(_bitBoards[2][0]) * 7
			+ _bitBoardHash(_bitBoards[3][0]) * 11
			+ _bitBoardHash(_bitBoards[4][0]) * 13
			+ _bitBoardHash(_bitBoards[5][0]) * 17
			+ _bitBoardHash(_bitBoards[0][0]) * 19
			+ _bitBoardHash(_bitBoards[1][1]) * 23
			+ _bitBoardHash(_bitBoards[2][1]) * 29
			+ _bitBoardHash(_bitBoards[3][1]) * 31
			+ _bitBoardHash(_bitBoards[4][1]) * 37
			+ _bitBoardHash(_bitBoards[5][1]) * 41;
	}

	const bool equalsSinglePieceType(const PieceType& pieceType, const BitBoards& bitBoards) const
	{
		const unsigned char pieceIndex = pieceTypeToBoardIndex(pieceType);

		// 2 * size_of(unsigned long long) == 16
		return memcmp(&_bitBoards[pieceIndex][0], &bitBoards._bitBoards[pieceIndex][0], 16) == 0;
	}

	const unsigned char pieceCountEquals(const BitBoards& bitBoards) const
	{
		return bitBoardPopulationCount(_bitBoards[6][0] | _bitBoards[6][1])
			== bitBoardPopulationCount(bitBoards._bitBoards[6][0] | bitBoards._bitBoards[6][1]);
	}

	const BitBoards movePiece(const unsigned long long& origin, const unsigned long long& destination, const PieceType& pieceType, const bool& isWhiteTurn) const
	{
		BitBoards newBitBoard(*this);
		const unsigned long long bothPositions = origin | destination;
		newBitBoard._bitBoards[pieceTypeToBoardIndex(pieceType)][isWhiteTurn] ^= bothPositions;
		newBitBoard._bitBoards[ALL_INDEX][isWhiteTurn] ^= bothPositions;
		return newBitBoard;
	}

	const BitBoards capturePiece(const unsigned long long& origin, const unsigned long long& destination, const PieceType& pieceType, const bool& isWhiteTurn) const
	{
		BitBoards newBitBoard(*this);
		const unsigned long long bothPositions = origin | destination;
		const unsigned long long flippedDestination = ~destination;
		newBitBoard._bitBoards[pieceTypeToBoardIndex(pieceType)][isWhiteTurn] ^= bothPositions;
		newBitBoard._bitBoards[ALL_INDEX][isWhiteTurn] ^= bothPositions;
		for (unsigned long long* bitBoard : newBitBoard._bitBoards)
		{
			bitBoard[!isWhiteTurn] &= flippedDestination;
		}
		return newBitBoard;
	}

	const BitBoards promotePiece(const unsigned long long& origin, const unsigned long long& destination, const PieceType& pieceTypeToPromoteTo, const bool& isWhiteTurn) const
	{
		BitBoards newBitBoard(*this);
		newBitBoard._bitBoards[PAWN_INDEX][isWhiteTurn] ^= origin;
		newBitBoard._bitBoards[pieceTypeToBoardIndex(pieceTypeToPromoteTo)][isWhiteTurn] ^= destination;
		newBitBoard._bitBoards[ALL_INDEX][isWhiteTurn] ^= origin | destination;
		return newBitBoard;
	}

	const BitBoards captureAndPromotePiece(const unsigned long long& origin, const unsigned long long & destination, const PieceType& pieceTypeToPromoteTo, const bool& isWhiteTurn) const
	{
		BitBoards newBitBoard(*this);
		const unsigned long long flippedDestination = ~destination;
		newBitBoard._bitBoards[PAWN_INDEX][isWhiteTurn] ^= origin;
		newBitBoard._bitBoards[pieceTypeToBoardIndex(pieceTypeToPromoteTo)][isWhiteTurn] ^= destination;
		newBitBoard._bitBoards[ALL_INDEX][isWhiteTurn] ^= origin | destination;
		for (unsigned long long* bitBoard : newBitBoard._bitBoards)
		{
			bitBoard[!isWhiteTurn] &= flippedDestination;
		}
		return newBitBoard;
	}

	const BitBoards castle(const unsigned long long& origin, const unsigned long long & destination, const bool& isWhiteTurn) const
	{
		BitBoards newBitBoard(*this);
		const unsigned long long rookOrigin = (((destination < origin) * KING_SIDE_ROOK_INITIAL_POSITIONS) | ((destination > origin) * QUEEN_SIDE_ROOK_INITIAL_POSITIONS))
			& ((isWhiteTurn * WHITE_ROOK_INITIAL_POSITIONS) | (!isWhiteTurn * BLACK_ROOK_INITIAL_POSITIONS));
		const unsigned long long rookDestination = isolateLeastSignificantBit((origin | destination) << 1);
		const unsigned long long bothKingPositions = origin | destination;
		const unsigned long long bothRookPositions = rookOrigin | rookDestination;
		const unsigned long long allPositions = bothKingPositions | bothRookPositions;
		newBitBoard._bitBoards[ROOK_INDEX][isWhiteTurn] ^= bothRookPositions;
		newBitBoard._bitBoards[KING_INDEX][isWhiteTurn] ^= bothKingPositions;
		newBitBoard._bitBoards[ALL_INDEX][isWhiteTurn] ^= allPositions;
		return newBitBoard;
	}

	template <typename _FunctionType>
	void foreachBoardBit(const bool& isWhite, const PieceType& pieceType, const _FunctionType& foreachFunction) const
	{
		unsigned long long actualBitBoard = _bitBoards[pieceTypeToBoardIndex(pieceType)][isWhite];
		foreachBit(actualBitBoard, foreachFunction);
	}

	const unsigned long long& allPieces(const bool& isWhite) const
	{
		return _bitBoards[ALL_INDEX][isWhite];
	}

	const bool isKingChecked(const bool& isWhite) const
	{
		const unsigned long long ownKingBoard = _bitBoards[KING_INDEX][isWhite];
		if (BitBoardRayConstants::isAtImmediateReach(ownKingBoard, _bitBoards[PAWN_INDEX][!isWhite], BitBoardRayConstants::PAWN_IMMEDIATE_CAPTURES[isWhite])) return true;

		if (BitBoardRayConstants::isAtImmediateReach(ownKingBoard, _bitBoards[KNIGHT_INDEX][!isWhite], BitBoardRayConstants::KNIGHT_IMMEDIATE_KILL)) return true;

		if (BitBoardRayConstants::isAtImmediateReach(ownKingBoard, _bitBoards[KING_INDEX][!isWhite], BitBoardRayConstants::KING_IMMEDIATE_KILL)) return true;

		const unsigned long long rookOrQueens = _bitBoards[ROOK_INDEX][!isWhite] | _bitBoards[QUEEN_INDEX][!isWhite];
		const unsigned long long allPieces = _bitBoards[ALL_INDEX][0] | _bitBoards[ALL_INDEX][1];

		if (BitBoardRayConstants::isAtReach(ownKingBoard, rookOrQueens, allPieces, BitBoardRayConstants::UPWARD_RAYS, true)) return true;
		if (BitBoardRayConstants::isAtReach(ownKingBoard, rookOrQueens, allPieces, BitBoardRayConstants::DOWNWARD_RAYS, false)) return true;
		if (BitBoardRayConstants::isAtReach(ownKingBoard, rookOrQueens, allPieces, BitBoardRayConstants::LEFTWARD_RAYS, true)) return true;
		if (BitBoardRayConstants::isAtReach(ownKingBoard, rookOrQueens, allPieces, BitBoardRayConstants::RIGHTWARD_RAYS, false)) return true;

		const unsigned long long bishopOrQueens = _bitBoards[BISHOP_INDEX][!isWhite] | _bitBoards[QUEEN_INDEX][!isWhite];

		if (BitBoardRayConstants::isAtReach(ownKingBoard, bishopOrQueens, allPieces, BitBoardRayConstants::UP_LEFT_WARD_RAYS, true)) return true;
		if (BitBoardRayConstants::isAtReach(ownKingBoard, bishopOrQueens, allPieces, BitBoardRayConstants::UP_RIGHT_WARD_RAYS, true))return true;
		if (BitBoardRayConstants::isAtReach(ownKingBoard, bishopOrQueens, allPieces, BitBoardRayConstants::DOWN_LEFT_WARD_RAYS, false))return true;
		if (BitBoardRayConstants::isAtReach(ownKingBoard, bishopOrQueens, allPieces, BitBoardRayConstants::DOWN_RIGHT_WARD_RAYS, false)) return true;
		return false;
	}

	const bool arePiecesEmpty(const unsigned long long& positions) const
	{
		return ((_bitBoards[ALL_INDEX][0] | _bitBoards[ALL_INDEX][1]) & positions) == 0;
	}

	const bool arePiecesAt(const unsigned long long& pieceMask, const bool& isWhite) const
	{
		return _bitBoards[ALL_INDEX][isWhite] & pieceMask;
	}

	const unsigned char populationCount(const PieceType& pieceType, const bool& isWhite) const
	{
		return bitBoardPopulationCount(_bitBoards[pieceTypeToBoardIndex(pieceType)][isWhite]);
	}

	const Piece getPiece(const unsigned char& x, const unsigned char& y) const
	{
		const unsigned char position(y * 8 + x + 1);
		const unsigned long long singleBitBoard = positionToSingleBit(position);
		if (singleBitBoard & _bitBoards[PAWN_INDEX][0])
		{
			return Piece(position, false, PieceType::PAWN);
		}
		else if (singleBitBoard & _bitBoards[PAWN_INDEX][1])
		{
			return Piece(position, true, PieceType::PAWN);
		}
		else if (singleBitBoard & _bitBoards[KNIGHT_INDEX][0])
		{
			return Piece(position, false, PieceType::KNIGHT);
		}
		else if (singleBitBoard & _bitBoards[KNIGHT_INDEX][1])
		{
			return Piece(position, true, PieceType::KNIGHT);
		}
		else if (singleBitBoard & _bitBoards[BISHOP_INDEX][0])
		{
			return Piece(position, false, PieceType::BISHOP);
		}
		else if (singleBitBoard & _bitBoards[BISHOP_INDEX][1])
		{
			return Piece(position, true, PieceType::BISHOP);
		}
		else if (singleBitBoard & _bitBoards[ROOK_INDEX][0])
		{
			return Piece(position, false, PieceType::ROOK);
		}
		else if (singleBitBoard & _bitBoards[ROOK_INDEX][1])
		{
			return Piece(position, true, PieceType::ROOK);
		}
		else if (singleBitBoard & _bitBoards[QUEEN_INDEX][0])
		{
			return Piece(position, false, PieceType::QUEEN);
		}
		else if (singleBitBoard & _bitBoards[QUEEN_INDEX][1])
		{
			return Piece(position, true, PieceType::QUEEN);
		}
		else if (singleBitBoard & _bitBoards[KING_INDEX][0])
		{
			return Piece(position, false, PieceType::KING);
		}
		else if (singleBitBoard & _bitBoards[KING_INDEX][1])
		{
			return Piece(position, true, PieceType::KING);
		}
		else
		{
			return Piece(position, false, PieceType::EMPTY);
		}
	}

private:
	static const unsigned char INDEXED_PIECE_TYPE_TO_BOARD[];
	static const std::hash<unsigned long long> _bitBoardHash;

	static const unsigned long long KING_SIDE_ROOK_INITIAL_POSITIONS = 0x0100000000000001;
	static const unsigned long long QUEEN_SIDE_ROOK_INITIAL_POSITIONS = 0x8000000000000080;
	static const unsigned long long WHITE_ROOK_INITIAL_POSITIONS = 0x0000000000000081;
	static const unsigned long long BLACK_ROOK_INITIAL_POSITIONS = 0x8100000000000000;

	static const unsigned char pieceTypeToBoardIndex(const PieceType& pieceType)
	{
		return INDEXED_PIECE_TYPE_TO_BOARD[(unsigned int)pieceType];
	}

	static const unsigned char PAWN_INDEX = 0;
	static const unsigned char KNIGHT_INDEX = 1;
	static const unsigned char BISHOP_INDEX = 2;
	static const unsigned char ROOK_INDEX = 3;
	static const unsigned char QUEEN_INDEX = 4;
	static const unsigned char KING_INDEX = 5;
	static const unsigned char ALL_INDEX = 6;

	unsigned long long _bitBoards[7][2];
};