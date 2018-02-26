#pragma once

#include "../Effectors/SimpleMove.h"
#include "../Effectors/CaptureMove.h"
#include "../Effectors/KillMove.h"
#include "../Effectors/PromotionMove.h"
#include "../Effectors/CapturePromotionMove.h"
#include "../Effectors/CastleMove.h"

#include "../../Boards/BitBoardRayConstants.h"

#include <vector>
#include <memory>

class BitBoardMoveConstants
{
public:

	// s: static array
	// d: dynamic array

	//									s			s
	// RAY data are defined as such: [isWhite][bitPosition] -> bitBoard

	//										s			s				s								d
	// CAPTURE data are defined as such: [isWhite][bitPosition][isCollidingWithEnemy][index of obstacle on the ray (0 if no obstable)] -> moves

	//									  s			s						d
	// MOVE data are defined as such: [isWhite][bitPosition][index of obstacle on the ray (0 if no obstable)] -> moves

	//											s
	// Direction data are defined as such: [isWhite] -> isLeast

	//												s		s
	// IMMEDIATE_CAPTURE data are defined as such: [isWhite][bitPosition] -> bitBoard

	//														s			s			s
	// IMMEDIATE_CAPTURE_DATA data are defined as such: [isWhite][bitPosition][bitDestination] -> Move

	//												s			s
	// IMMEDIATE_KILL data are defined as such: [isWhite][bitPosition] -> bitBoard

	//													s			s			s
	// IMMEDIATE_KILL_DATA data are defined as such: [isWhite][bitPosition][bitDestination] -> Move

	static std::shared_ptr<std::vector<Move const *>> PAWN_MOVES[2][65][65];
	static Move const * PAWN_IMMEDIATE_CAPTURE_DATA[2][65][65];

	static std::shared_ptr<std::vector<Move const *>> ROOK_UPWARD_CAPTURES[65][2][65];
	static std::shared_ptr<std::vector<Move const *>> ROOK_DOWNWARD_CAPTURES[65][2][65];
	static std::shared_ptr<std::vector<Move const *>> ROOK_LEFTWARD_CAPTURES[65][2][65];
	static std::shared_ptr<std::vector<Move const *>> ROOK_RIGHTWARD_CAPTURES[65][2][65];

	static std::shared_ptr<std::vector<Move const *>> BISHOP_UP_LEFT_WARD_CAPTURES[65][2][65];
	static std::shared_ptr<std::vector<Move const *>> BISHOP_UP_RIGHT_WARD_CAPTURES[65][2][65];
	static std::shared_ptr<std::vector<Move const *>> BISHOP_DOWN_LEFT_WARD_CAPTURES[65][2][65];
	static std::shared_ptr<std::vector<Move const *>> BISHOP_DOWN_RIGHT_WARD_CAPTURES[65][2][65];

	static Move const * KNIGHT_IMMEDIATE_KILL_DATA[65][65];

	static std::shared_ptr<std::vector<Move const *>> QUEEN_UPWARD_CAPTURES[65][2][65];
	static std::shared_ptr<std::vector<Move const *>> QUEEN_DOWNWARD_CAPTURES[65][2][65];
	static std::shared_ptr<std::vector<Move const *>> QUEEN_LEFTWARD_CAPTURES[65][2][65];
	static std::shared_ptr<std::vector<Move const *>> QUEEN_RIGHTWARD_CAPTURES[65][2][65];
	static std::shared_ptr<std::vector<Move const *>> QUEEN_UP_LEFT_WARD_CAPTURES[65][2][65];
	static std::shared_ptr<std::vector<Move const *>> QUEEN_UP_RIGHT_WARD_CAPTURES[65][2][65];
	static std::shared_ptr<std::vector<Move const *>> QUEEN_DOWN_LEFT_WARD_CAPTURES[65][2][65];
	static std::shared_ptr<std::vector<Move const *>> QUEEN_DOWN_RIGHT_WARD_CAPTURES[65][2][65];

	static Move const * KING_IMMEDIATE_KILL_DATA[65][65];

	static Move const* CASTLES_MOVES[4];

	static void initialize()
	{
		BitBoardRayConstants::initialize();

		initializeEmptyMoves();

		initializePawnMoves();
		initializePawnImmediateData();

		initializeRookCaptures();

		initializeBishopCaptures();

		initializeKnightKills();

		initializeQueenCaptures();

		initializeKingKills();
		initializeCastles();
	}

	static void destroy()
	{
		for (unsigned char isWhite = 0; isWhite < 2; ++isWhite)
		{
			for (unsigned char position = 0; position <= 64; ++position)
			{
				for (const auto moves : PAWN_MOVES[isWhite][position])
				{
					for (const auto move : *moves)
					{
						delete move;
					}
				}
			}
		}
		for (unsigned char isWhite = 0; isWhite < 2; ++isWhite)
		{
			for (unsigned char position = 0; position <= 64; ++position)
			{
				for (unsigned char attackPosition = 0; attackPosition <= 64; ++attackPosition)
				{
					delete PAWN_IMMEDIATE_CAPTURE_DATA[isWhite][position][attackPosition];
				}
			}
		}
		// delete the rest some time
	}

	static std::shared_ptr<std::vector<Move const *>> getMoves(const unsigned char& position, const BitBoards& bitBoard, const unsigned long long rays[65], const std::shared_ptr<std::vector<Move const *>> moves[65][65], const bool direction)
	{
		const unsigned long long collisionBitBoard = (bitBoard.allPieces(true) | bitBoard.allPieces(false)) & rays[position];
		const unsigned char collisionPosition = direction ? FastMath::positionOfLeastSignificantBit(collisionBitBoard) : FastMath::positionOfMostSignificantBit(collisionBitBoard);
		return moves[position][collisionPosition];
	}

	static std::shared_ptr<std::vector<Move const *>> getCaptures(const bool& isWhite, const unsigned char& position, const BitBoards& bitBoard, const unsigned long long rays[65], const std::shared_ptr<std::vector<Move const *>> captures[65][2][65], const bool direction)
	{
		const unsigned long long sameColorCollisionBitBoard = bitBoard.allPieces(isWhite) & rays[position];
		const unsigned long long otherColorCollisionBitBoard = bitBoard.allPieces(!isWhite) & rays[position];
		bool isCollidingWithEnemy;
		unsigned char collisionPosition;
		if (direction)
		{
			const unsigned long long sameColorCollision = FastMath::isolateLeastSignificantBit(sameColorCollisionBitBoard);
			const unsigned long long otherColorCollision = FastMath::isolateLeastSignificantBit(otherColorCollisionBitBoard);
			isCollidingWithEnemy = (otherColorCollision < sameColorCollision
				|| (sameColorCollision == 0 && otherColorCollision != 0))
				&& !(otherColorCollision == 0 && sameColorCollision != 0);
			collisionPosition = FastMath::positionOfLeastSignificantBit(sameColorCollision | otherColorCollision);
		}
		else
		{
			const unsigned long long sameColorCollision = FastMath::isolateMostSignificantBit(sameColorCollisionBitBoard);
			const unsigned long long otherColorCollision = FastMath::isolateMostSignificantBit(otherColorCollisionBitBoard);
			isCollidingWithEnemy = otherColorCollision > sameColorCollision;
			collisionPosition = FastMath::positionOfMostSignificantBit(sameColorCollision | otherColorCollision);
		}
		return captures[position][isCollidingWithEnemy][collisionPosition];
	}

	static std::shared_ptr<std::vector<Move const *>> getImmediateCaptures(const bool& isWhite, const unsigned char& position, const BitBoards& bitBoard, const unsigned long long immediateCaptures[65], Move const* const immediateData[65][65])
	{
		const unsigned long long collisionBitBoard = bitBoard.allPieces(!isWhite) & immediateCaptures[position];
		std::shared_ptr<std::vector<Move const *>> moves(std::make_shared<std::vector<Move const *>>());
		moves->reserve(FastMath::bitBoardPopulationCount(collisionBitBoard));
		FastMath::foreachBit(collisionBitBoard, [&moves, &position, &isWhite, &immediateData](const unsigned char& attackPosition) {
			moves->push_back(immediateData[position][attackPosition]);
		});
		return moves;
	}

	static std::shared_ptr<std::vector<Move const *>> getImmediateKills(const bool& isWhite, const unsigned char& position, const BitBoards& bitBoard, const unsigned long long immediateKills[65], Move const* const immediateData[65][65])
	{
		const unsigned long long collisionBitBoard = (~bitBoard.allPieces(isWhite)) & immediateKills[position];
		std::shared_ptr<std::vector<Move const *>> moves(std::make_shared<std::vector<Move const *>>());
		moves->reserve(FastMath::bitBoardPopulationCount(collisionBitBoard));
		FastMath::foreachBit(collisionBitBoard, [&moves, &position, &isWhite, &immediateData](const unsigned char& attackPosition) {
			moves->push_back(immediateData[position][attackPosition]);
		});
		return moves;
	}

private:

	static void initializeEmptyMoves()
	{
		for (size_t w = 0; w < 2; ++w)
		{
			for (size_t p = 0; p < 65; ++p)
			{
				for (size_t d = 0; d < 65; ++d)
				{
					PAWN_MOVES[w][p][d] = std::make_shared<std::vector<Move const *>>();
				}
			}
		}
		for (size_t p = 0; p < 65; ++p)
		{
			for (size_t w = 0; w < 2; ++w)
			{
				for (size_t d = 0; d < 65; ++d)
				{
					ROOK_UPWARD_CAPTURES[p][w][d] = std::make_shared<std::vector<Move const *>>();
					ROOK_DOWNWARD_CAPTURES[p][w][d] = std::make_shared<std::vector<Move const *>>();
					ROOK_LEFTWARD_CAPTURES[p][w][d] = std::make_shared<std::vector<Move const *>>();
					ROOK_RIGHTWARD_CAPTURES[p][w][d] = std::make_shared<std::vector<Move const *>>();

					BISHOP_UP_LEFT_WARD_CAPTURES[p][w][d] = std::make_shared<std::vector<Move const *>>();
					BISHOP_UP_RIGHT_WARD_CAPTURES[p][w][d] = std::make_shared<std::vector<Move const *>>();
					BISHOP_DOWN_LEFT_WARD_CAPTURES[p][w][d] = std::make_shared<std::vector<Move const *>>();
					BISHOP_DOWN_RIGHT_WARD_CAPTURES[p][w][d] = std::make_shared<std::vector<Move const *>>();

					QUEEN_UPWARD_CAPTURES[p][w][d] = std::make_shared<std::vector<Move const *>>();
					QUEEN_DOWNWARD_CAPTURES[p][w][d] = std::make_shared<std::vector<Move const *>>();
					QUEEN_LEFTWARD_CAPTURES[p][w][d] = std::make_shared<std::vector<Move const *>>();
					QUEEN_RIGHTWARD_CAPTURES[p][w][d] = std::make_shared<std::vector<Move const *>>();
					QUEEN_UP_LEFT_WARD_CAPTURES[p][w][d] = std::make_shared<std::vector<Move const *>>();
					QUEEN_UP_RIGHT_WARD_CAPTURES[p][w][d] = std::make_shared<std::vector<Move const *>>();
					QUEEN_DOWN_LEFT_WARD_CAPTURES[p][w][d] = std::make_shared<std::vector<Move const *>>();
					QUEEN_DOWN_RIGHT_WARD_CAPTURES[p][w][d] = std::make_shared<std::vector<Move const *>>();
				}
			}
		}
	}



	static std::shared_ptr<std::vector<Move const *>> createCaptures(const char& x, const char& y, const char& incX, const char& incY, const unsigned char& stopPosition, const bool& captureLast, const PieceType& pieceType)
	{
		std::shared_ptr<std::vector<Move const *>> captures(std::make_shared<std::vector< Move const *>>());
		char nextX = x + incX;
		char nextY = y + incY;
		while (BitBoardRayConstants::isInsideBoard(nextX, nextY) && nextY * 8 + nextX + 1 != stopPosition)
		{
			captures->push_back(new SimpleMove(
				FastMath::positionToSingleBit(y * 8 + x + 1),
				FastMath::positionToSingleBit(nextY * 8 + nextX + 1),
				pieceType
			));
			nextX = nextX + incX;
			nextY = nextY + incY;
		}
		if (captureLast && BitBoardRayConstants::isInsideBoard(nextX, nextY) && nextY * 8 + nextX + 1 == stopPosition)
		{
			captures->push_back(new CaptureMove(
				FastMath::positionToSingleBit(y * 8 + x + 1),
				FastMath::positionToSingleBit(nextY * 8 + nextX + 1),
				pieceType
			));
		}
		return captures;
	}

	static void initializePawnMoves()
	{
		for (unsigned char position = 9; position <= 48; ++position)
		{
			PAWN_MOVES[1][position][0] = std::make_shared<std::vector<Move const *>>(std::vector<Move const *>({ new SimpleMove(
					FastMath::positionToSingleBit(position),
					FastMath::positionToSingleBit(position + 8),
					PieceType::PAWN
				) }));
			if ((position - 1) / 8 == 1)
			{
				PAWN_MOVES[1][position][0] = std::make_shared<std::vector<Move const *>>(std::vector<Move const *>({
					new SimpleMove(
						FastMath::positionToSingleBit(position),
						FastMath::positionToSingleBit(position + 8),
						PieceType::PAWN
					), new SimpleMove(
						FastMath::positionToSingleBit(position),
						FastMath::positionToSingleBit(position + 16),
						PieceType::PAWN
					) }));
				PAWN_MOVES[1][position][position + 16] = std::make_shared<std::vector<Move const *>>(std::vector<Move const *>({ new SimpleMove(
					FastMath::positionToSingleBit(position),
					FastMath::positionToSingleBit(position + 8),
					PieceType::PAWN
				) }));
			}
		}
		for (unsigned char position = 17; position <= 56; ++position)
		{
			PAWN_MOVES[0][position][0] = std::make_shared<std::vector<Move const *>>(std::vector<Move const *>({ new SimpleMove(
				FastMath::positionToSingleBit(position),
				FastMath::positionToSingleBit(position - 8),
				PieceType::PAWN
			) }));
			if ((position - 1) / 8 == 6)
			{
				PAWN_MOVES[0][position][0] = std::make_shared<std::vector<Move const *>>(std::vector<Move const *>({
					new SimpleMove(
						FastMath::positionToSingleBit(position),
						FastMath::positionToSingleBit(position - 8),
						PieceType::PAWN
					), new SimpleMove(
						FastMath::positionToSingleBit(position),
						FastMath::positionToSingleBit(position - 16),
						PieceType::PAWN
				) }));
				PAWN_MOVES[0][position][position - 16] = std::make_shared<std::vector<Move const *>>(std::vector<Move const *>({ new SimpleMove(
					FastMath::positionToSingleBit(position),
					FastMath::positionToSingleBit(position - 8),
					PieceType::PAWN
				) }));
			}
		}
		for (unsigned char position = 49; position <= 56; ++position)
		{
			PAWN_MOVES[1][position][0] = std::make_shared<std::vector<Move const *>>(std::vector<Move const *>({
				new PromotionMove(FastMath::positionToSingleBit(position), FastMath::positionToSingleBit(position + 8), PieceType::QUEEN),
				new PromotionMove(FastMath::positionToSingleBit(position), FastMath::positionToSingleBit(position + 8), PieceType::ROOK),
				new PromotionMove(FastMath::positionToSingleBit(position), FastMath::positionToSingleBit(position + 8), PieceType::BISHOP),
				new PromotionMove(FastMath::positionToSingleBit(position), FastMath::positionToSingleBit(position + 8), PieceType::KNIGHT)
				}));
		}
		for (unsigned char position = 9; position <= 16; ++position)
		{
			PAWN_MOVES[0][position][0] = std::make_shared<std::vector<Move const *>>(std::vector<Move const *>({
				new PromotionMove(FastMath::positionToSingleBit(position), FastMath::positionToSingleBit(position - 8), PieceType::QUEEN),
				new PromotionMove(FastMath::positionToSingleBit(position), FastMath::positionToSingleBit(position - 8), PieceType::ROOK),
				new PromotionMove(FastMath::positionToSingleBit(position), FastMath::positionToSingleBit(position - 8), PieceType::BISHOP),
				new PromotionMove(FastMath::positionToSingleBit(position), FastMath::positionToSingleBit(position - 8), PieceType::KNIGHT)
				}));
		}
	}

	static void initializePawnImmediateData()
	{
		for (unsigned char position = 9; position <= 48; ++position)
		{
			if (position % 8 != 0)
			{
				PAWN_IMMEDIATE_CAPTURE_DATA[1][position][position + 9] = new CaptureMove(
					FastMath::positionToSingleBit(position),
					FastMath::positionToSingleBit(position + 9),
					PieceType::PAWN
				);
			}
			if (position % 8 != 1)
			{
				PAWN_IMMEDIATE_CAPTURE_DATA[1][position][position + 7] = new CaptureMove(
					FastMath::positionToSingleBit(position),
					FastMath::positionToSingleBit(position + 7),
					PieceType::PAWN
				);
			}
		}
		for (unsigned char position = 17; position <= 56; ++position)
		{
			if (position % 8 != 0)
			{
				PAWN_IMMEDIATE_CAPTURE_DATA[0][position][position - 7] = new CaptureMove(
					FastMath::positionToSingleBit(position),
					FastMath::positionToSingleBit(position - 7),
					PieceType::PAWN
				);
			}
			if (position % 8 != 1)
			{
				PAWN_IMMEDIATE_CAPTURE_DATA[0][position][position - 9] = new CaptureMove(
					FastMath::positionToSingleBit(position),
					FastMath::positionToSingleBit(position - 9),
					PieceType::PAWN
				);
			}
		}
		for (unsigned char position = 49; position <= 56; ++position)
		{
			if (position % 8 != 0)
			{
				PAWN_IMMEDIATE_CAPTURE_DATA[1][position][position + 9] = new CapturePromotionMove(
					FastMath::positionToSingleBit(position),
					FastMath::positionToSingleBit(position + 9),
					PieceType::QUEEN
				);
			}
			if (position % 8 != 1)
			{
				PAWN_IMMEDIATE_CAPTURE_DATA[1][position][position + 7] = new CapturePromotionMove(
					FastMath::positionToSingleBit(position),
					FastMath::positionToSingleBit(position + 7),
					PieceType::QUEEN
				);
			}
		}
		for (unsigned char position = 9; position <= 16; ++position)
		{
			if (position % 8 != 0)
			{
				PAWN_IMMEDIATE_CAPTURE_DATA[0][position][position - 7] = new CapturePromotionMove(
					FastMath::positionToSingleBit(position),
					FastMath::positionToSingleBit(position - 7),
					PieceType::QUEEN
				);
			}
			if (position % 8 != 1)
			{
				PAWN_IMMEDIATE_CAPTURE_DATA[0][position][position - 9] = new CapturePromotionMove(
					FastMath::positionToSingleBit(position),
					FastMath::positionToSingleBit(position - 9),
					PieceType::QUEEN
				);
			}
		}
	}

	static void initializeRookCaptures()
	{
		for (unsigned char x = 0; x < 8; ++x)
		{
			for (unsigned char y = 0; y < 8; ++y)
			{
				ROOK_UPWARD_CAPTURES[y * 8 + x + 1][0][0] = BitBoardMoveConstants::createCaptures(
					x, y, 0, 1, 0, false, PieceType::ROOK
				);
				FastMath::foreachBit(BitBoardRayConstants::UPWARD_RAYS[y * 8 + x + 1], [&x, &y](const unsigned char& collisionPosition) {
					for (unsigned char capturesLast = 0; capturesLast < 2; ++capturesLast)
					{
						ROOK_UPWARD_CAPTURES[y * 8 + x + 1][capturesLast][collisionPosition] = BitBoardMoveConstants::createCaptures(
							x, y, 0, 1, collisionPosition, capturesLast, PieceType::ROOK
						);
					}
				});
				ROOK_DOWNWARD_CAPTURES[y * 8 + x + 1][0][0] = BitBoardMoveConstants::createCaptures(
					x, y, 0, -1, 0, false, PieceType::ROOK
				);
				FastMath::foreachBit(BitBoardRayConstants::DOWNWARD_RAYS[y * 8 + x + 1], [&x, &y](const unsigned char& collisionPosition) {
					for (unsigned char capturesLast = 0; capturesLast < 2; ++capturesLast)
					{
						ROOK_DOWNWARD_CAPTURES[y * 8 + x + 1][capturesLast][collisionPosition] = BitBoardMoveConstants::createCaptures(
							x, y, 0, -1, collisionPosition, capturesLast, PieceType::ROOK
						);
					}
				});
				ROOK_RIGHTWARD_CAPTURES[y * 8 + x + 1][0][0] = BitBoardMoveConstants::createCaptures(
					x, y, -1, 0, 0, false, PieceType::ROOK
				);
				FastMath::foreachBit(BitBoardRayConstants::RIGHTWARD_RAYS[y * 8 + x + 1], [&x, &y](const unsigned char& collisionPosition) {
					for (unsigned char capturesLast = 0; capturesLast < 2; ++capturesLast)
					{
						ROOK_RIGHTWARD_CAPTURES[y * 8 + x + 1][capturesLast][collisionPosition] = BitBoardMoveConstants::createCaptures(
							x, y, -1, 0, collisionPosition, capturesLast, PieceType::ROOK
						);
					}
				});
				ROOK_LEFTWARD_CAPTURES[y * 8 + x + 1][0][0] = BitBoardMoveConstants::createCaptures(
					x, y, 1, 0, 0, false, PieceType::ROOK
				);
				FastMath::foreachBit(BitBoardRayConstants::LEFTWARD_RAYS[y * 8 + x + 1], [&x, &y](const unsigned char& collisionPosition) {
					for (unsigned char capturesLast = 0; capturesLast < 2; ++capturesLast)
					{
						ROOK_LEFTWARD_CAPTURES[y * 8 + x + 1][capturesLast][collisionPosition] = BitBoardMoveConstants::createCaptures(
							x, y, 1, 0, collisionPosition, capturesLast, PieceType::ROOK
						);
					}
				});
			}
		}
	}

	static void initializeBishopCaptures()
	{
		for (unsigned char x = 0; x < 8; ++x)
		{
			for (unsigned char y = 0; y < 8; ++y)
			{
				BISHOP_UP_LEFT_WARD_CAPTURES[y * 8 + x + 1][0][0] = BitBoardMoveConstants::createCaptures(
					x, y, 1, 1, 0, false, PieceType::BISHOP
				);
				FastMath::foreachBit(BitBoardRayConstants::UP_LEFT_WARD_RAYS[y * 8 + x + 1], [&x, &y](const unsigned char& collisionPosition) {
					for (unsigned char capturesLast = 0; capturesLast < 2; ++capturesLast)
					{
						BISHOP_UP_LEFT_WARD_CAPTURES[y * 8 + x + 1][capturesLast][collisionPosition] = BitBoardMoveConstants::createCaptures(
							x, y, 1, 1, collisionPosition, capturesLast, PieceType::BISHOP
						);
					}
				});
				BISHOP_UP_RIGHT_WARD_CAPTURES[y * 8 + x + 1][0][0] = BitBoardMoveConstants::createCaptures(
					x, y, -1, 1, 0, false, PieceType::BISHOP
				);
				FastMath::foreachBit(BitBoardRayConstants::UP_RIGHT_WARD_RAYS[y * 8 + x + 1], [&x, &y](const unsigned char& collisionPosition) {
					for (unsigned char capturesLast = 0; capturesLast < 2; ++capturesLast)
					{
						BISHOP_UP_RIGHT_WARD_CAPTURES[y * 8 + x + 1][capturesLast][collisionPosition] = BitBoardMoveConstants::createCaptures(
							x, y, -1, 1, collisionPosition, capturesLast, PieceType::BISHOP
						);
					}
				});
				BISHOP_DOWN_LEFT_WARD_CAPTURES[y * 8 + x + 1][0][0] = BitBoardMoveConstants::createCaptures(
					x, y, 1, -1, 0, false, PieceType::BISHOP
				);
				FastMath::foreachBit(BitBoardRayConstants::DOWN_LEFT_WARD_RAYS[y * 8 + x + 1], [&x, &y](const unsigned char& collisionPosition) {
					for (unsigned char capturesLast = 0; capturesLast < 2; ++capturesLast)
					{
						BISHOP_DOWN_LEFT_WARD_CAPTURES[y * 8 + x + 1][capturesLast][collisionPosition] = BitBoardMoveConstants::createCaptures(
							x, y, 1, -1, collisionPosition, capturesLast, PieceType::BISHOP
						);
					}
				});
				BISHOP_DOWN_RIGHT_WARD_CAPTURES[y * 8 + x + 1][0][0] = BitBoardMoveConstants::createCaptures(
					x, y, -1, -1, 0, false, PieceType::BISHOP
				);
				FastMath::foreachBit(BitBoardRayConstants::DOWN_RIGHT_WARD_RAYS[y * 8 + x + 1], [&x, &y](const unsigned char& collisionPosition) {
					for (unsigned char capturesLast = 0; capturesLast < 2; ++capturesLast)
					{
						BISHOP_DOWN_RIGHT_WARD_CAPTURES[y * 8 + x + 1][capturesLast][collisionPosition] = BitBoardMoveConstants::createCaptures(
							x, y, -1, -1, collisionPosition, capturesLast, PieceType::BISHOP
						);
					}
				});
			}
		}
	}

	static void initializeKnightKills()
	{
		for (char x = 0; x < 8; ++x)
		{
			for (char y = 0; y < 8; ++y)
			{
				FastMath::foreachBit(BitBoardRayConstants::KNIGHT_IMMEDIATE_KILL[y * 8 + x + 1], [&x, &y](const unsigned char& attackPosition) {
					BitBoardMoveConstants::KNIGHT_IMMEDIATE_KILL_DATA[y * 8 + x + 1][attackPosition] = new KillMove(
						FastMath::positionToSingleBit(y * 8 + x + 1),
						FastMath::positionToSingleBit(attackPosition),
						PieceType::KNIGHT
					);
				});
			}
		}
	}

	static void initializeQueenCaptures()
	{
		for (unsigned char x = 0; x < 8; ++x)
		{
			for (unsigned char y = 0; y < 8; ++y)
			{
				QUEEN_UP_LEFT_WARD_CAPTURES[y * 8 + x + 1][0][0] = BitBoardMoveConstants::createCaptures(
					x, y, 1, 1, 0, false, PieceType::QUEEN
				);
				FastMath::foreachBit(BitBoardRayConstants::UP_LEFT_WARD_RAYS[y * 8 + x + 1], [&x, &y](const unsigned char& collisionPosition) {
					for (unsigned char capturesLast = 0; capturesLast < 2; ++capturesLast)
					{
						QUEEN_UP_LEFT_WARD_CAPTURES[y * 8 + x + 1][capturesLast][collisionPosition] = BitBoardMoveConstants::createCaptures(
							x, y, 1, 1, collisionPosition, capturesLast, PieceType::QUEEN
						);
					}
				});
				QUEEN_UP_RIGHT_WARD_CAPTURES[y * 8 + x + 1][0][0] = BitBoardMoveConstants::createCaptures(
					x, y, -1, 1, 0, false, PieceType::QUEEN
				);
				FastMath::foreachBit(BitBoardRayConstants::UP_RIGHT_WARD_RAYS[y * 8 + x + 1], [&x, &y](const unsigned char& collisionPosition) {
					for (unsigned char capturesLast = 0; capturesLast < 2; ++capturesLast)
					{
						QUEEN_UP_RIGHT_WARD_CAPTURES[y * 8 + x + 1][capturesLast][collisionPosition] = BitBoardMoveConstants::createCaptures(
							x, y, -1, 1, collisionPosition, capturesLast, PieceType::QUEEN
						);
					}
				});
				QUEEN_DOWN_LEFT_WARD_CAPTURES[y * 8 + x + 1][0][0] = BitBoardMoveConstants::createCaptures(
					x, y, 1, -1, 0, false, PieceType::QUEEN
				);
				FastMath::foreachBit(BitBoardRayConstants::DOWN_LEFT_WARD_RAYS[y * 8 + x + 1], [&x, &y](const unsigned char& collisionPosition) {
					for (unsigned char capturesLast = 0; capturesLast < 2; ++capturesLast)
					{
						QUEEN_DOWN_LEFT_WARD_CAPTURES[y * 8 + x + 1][capturesLast][collisionPosition] = BitBoardMoveConstants::createCaptures(
							x, y, 1, -1, collisionPosition, capturesLast, PieceType::QUEEN
						);
					}
				});
				QUEEN_DOWN_RIGHT_WARD_CAPTURES[y * 8 + x + 1][0][0] = BitBoardMoveConstants::createCaptures(
					x, y, -1, -1, 0, false, PieceType::QUEEN
				);
				FastMath::foreachBit(BitBoardRayConstants::DOWN_RIGHT_WARD_RAYS[y * 8 + x + 1], [&x, &y](const unsigned char& collisionPosition) {
					for (unsigned char capturesLast = 0; capturesLast < 2; ++capturesLast)
					{
						QUEEN_DOWN_RIGHT_WARD_CAPTURES[y * 8 + x + 1][capturesLast][collisionPosition] = BitBoardMoveConstants::createCaptures(
							x, y, -1, -1, collisionPosition, capturesLast, PieceType::QUEEN
						);
					}
				});
				QUEEN_UPWARD_CAPTURES[y * 8 + x + 1][0][0] = BitBoardMoveConstants::createCaptures(
					x, y, 0, 1, 0, false, PieceType::QUEEN
				);
				FastMath::foreachBit(BitBoardRayConstants::UPWARD_RAYS[y * 8 + x + 1], [&x, &y](const unsigned char& collisionPosition) {
					for (unsigned char capturesLast = 0; capturesLast < 2; ++capturesLast)
					{
						QUEEN_UPWARD_CAPTURES[y * 8 + x + 1][capturesLast][collisionPosition] = BitBoardMoveConstants::createCaptures(
							x, y, 0, 1, collisionPosition, capturesLast, PieceType::QUEEN
						);
					}
				});
				QUEEN_DOWNWARD_CAPTURES[y * 8 + x + 1][0][0] = BitBoardMoveConstants::createCaptures(
					x, y, 0, -1, 0, false, PieceType::QUEEN
				);
				FastMath::foreachBit(BitBoardRayConstants::DOWNWARD_RAYS[y * 8 + x + 1], [&x, &y](const unsigned char& collisionPosition) {
					for (unsigned char capturesLast = 0; capturesLast < 2; ++capturesLast)
					{
						QUEEN_DOWNWARD_CAPTURES[y * 8 + x + 1][capturesLast][collisionPosition] = BitBoardMoveConstants::createCaptures(
							x, y, 0, -1, collisionPosition, capturesLast, PieceType::QUEEN
						);
					}
				});
				QUEEN_RIGHTWARD_CAPTURES[y * 8 + x + 1][0][0] = BitBoardMoveConstants::createCaptures(
					x, y, -1, 0, 0, false, PieceType::QUEEN
				);
				FastMath::foreachBit(BitBoardRayConstants::RIGHTWARD_RAYS[y * 8 + x + 1], [&x, &y](const unsigned char& collisionPosition) {
					for (unsigned char capturesLast = 0; capturesLast < 2; ++capturesLast)
					{
						QUEEN_RIGHTWARD_CAPTURES[y * 8 + x + 1][capturesLast][collisionPosition] = BitBoardMoveConstants::createCaptures(
							x, y, -1, 0, collisionPosition, capturesLast, PieceType::QUEEN
						);
					}
				});
				QUEEN_LEFTWARD_CAPTURES[y * 8 + x + 1][0][0] = BitBoardMoveConstants::createCaptures(
					x, y, 1, 0, 0, false, PieceType::QUEEN
				);
				FastMath::foreachBit(BitBoardRayConstants::LEFTWARD_RAYS[y * 8 + x + 1], [&x, &y](const unsigned char& collisionPosition) {
					for (unsigned char capturesLast = 0; capturesLast < 2; ++capturesLast)
					{
						QUEEN_LEFTWARD_CAPTURES[y * 8 + x + 1][capturesLast][collisionPosition] = BitBoardMoveConstants::createCaptures(
							x, y, 1, 0, collisionPosition, capturesLast, PieceType::QUEEN
						);
					}
				});
			}
		}
	}

	static void initializeKingKills()
	{
		for (char x = 0; x < 8; ++x)
		{
			for (char y = 0; y < 8; ++y)
			{
				FastMath::foreachBit(BitBoardRayConstants::KING_IMMEDIATE_KILL[y * 8 + x + 1], [&x, &y](const unsigned char& attackPosition) {
					BitBoardMoveConstants::KING_IMMEDIATE_KILL_DATA[y * 8 + x + 1][attackPosition] = new KillMove(
						FastMath::positionToSingleBit(y * 8 + x + 1),
						FastMath::positionToSingleBit(attackPosition),
						PieceType::KING
					);
				});
			}
		}
	}

	static void initializeCastles()
	{
		CASTLES_MOVES[0] = new CastleMove(0x0000000000000008, 0x0000000000000002);
		CASTLES_MOVES[1] = new CastleMove(0x0000000000000008, 0x0000000000000020);
		CASTLES_MOVES[2] = new CastleMove(0x0800000000000000, 0x0200000000000000);
		CASTLES_MOVES[3] = new CastleMove(0x0800000000000000, 0x2000000000000000);
	}
};