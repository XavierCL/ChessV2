#pragma once

#include "../Physics/FastMath.h"

class BitBoardRayConstants
{
public:
	static unsigned long long PAWN_RAYS[2][65];
	static unsigned long long PAWN_IMMEDIATE_CAPTURES[2][65];

	static unsigned long long UPWARD_RAYS[65];
	static unsigned long long DOWNWARD_RAYS[65];
	static unsigned long long LEFTWARD_RAYS[65];
	static unsigned long long RIGHTWARD_RAYS[65];

	static unsigned long long UP_LEFT_WARD_RAYS[65];
	static unsigned long long UP_RIGHT_WARD_RAYS[65];
	static unsigned long long DOWN_LEFT_WARD_RAYS[65];
	static unsigned long long DOWN_RIGHT_WARD_RAYS[65];

	static unsigned long long KNIGHT_IMMEDIATE_KILL[65];

	static unsigned long long KING_IMMEDIATE_KILL[65];

	static void initialize()
	{
		initializePawnRays();
		initializePawnImmediateCaptures();

		initializeRookRays();

		initializeBishopRays();

		initializeKnightRays();

		initializeKingRays();
	}

	static const bool isInsideBoard(const char& x, const char& y)
	{
		return x > -1 && x < 8 && y > -1 && y < 8;
	}

	static const unsigned long long createRay(const char& x, const char& y, const char& incX, const char& incY)
	{
		unsigned long long ray = 0x0;
		char nextX = x + incX;
		char nextY = y + incY;
		while (isInsideBoard(nextX, nextY))
		{
			ray |= positionToSingleBit(nextY * 8 + nextX + 1);
			nextX = nextX + incX;
			nextY = nextY + incY;
		}
		return ray;
	}

	static const bool isAtImmediateReach(const unsigned long long& attackPosition, const unsigned long long& enemies, const unsigned long long ray[65])
	{
		return ray[singleBitToPosition(attackPosition)] & enemies;
	}

	static const bool isAtReach(const unsigned long long& attackPosition, const unsigned long long& enemies, const unsigned long long& allBlocking, const unsigned long long ray[65], const bool& isLeastDirection)
	{
		const unsigned long long attackRay = ray[singleBitToPosition(attackPosition)];
		const unsigned long long enemyRay = attackRay & enemies;
		const unsigned long long allBlockingRay = attackRay & allBlocking;
		if (isLeastDirection)
		{
			const unsigned long long blockingEnemy = isolateLeastSignificantBit(enemyRay);
			const unsigned long long blockingPiece = isolateLeastSignificantBit(allBlockingRay);
			return (blockingEnemy | ((blockingEnemy == 0) * 0xc000000000000000))
				<= (blockingPiece | ((blockingPiece == 0) * 0x9000000000000000));
		}
		else
		{
			const unsigned long long blockingEnemy = isolateMostSignificantBit(enemyRay);
			const unsigned long long blockingPiece = isolateMostSignificantBit(allBlockingRay);
			return (blockingEnemy >= blockingPiece) && !(blockingEnemy == 0 && blockingPiece == 0);
		}
	}

private:

	static void initializePawnRays()
	{
		for (unsigned char position = 9; position <= 56; ++position)
		{
			PAWN_RAYS[1][position] = positionToSingleBit(position + 8);
			if ((position - 1) / 8 == 1)
			{
				PAWN_RAYS[1][position] |= positionToSingleBit(position + 16);
			}
		}
		for (unsigned char position = 9; position <= 56; ++position)
		{
			PAWN_RAYS[0][position] = positionToSingleBit(position - 8);
			if ((position - 1) / 8 == 6)
			{
				PAWN_RAYS[0][position] |= positionToSingleBit(position - 16);
			}
		}
	}

	static void initializePawnImmediateCaptures()
	{
		for (unsigned char position = 1; position <= 56; ++position)
		{
			if (position % 8 != 0)
			{
				PAWN_IMMEDIATE_CAPTURES[1][position] |= positionToSingleBit(position + 9);
			}
			if (position % 8 != 1)
			{
				PAWN_IMMEDIATE_CAPTURES[1][position] |= positionToSingleBit(position + 7);
			}
		}
		for (unsigned char position = 9; position <= 64; ++position)
		{
			if (position % 8 != 0)
			{
				PAWN_IMMEDIATE_CAPTURES[0][position] |= positionToSingleBit(position - 7);
			}
			if (position % 8 != 1)
			{
				PAWN_IMMEDIATE_CAPTURES[0][position] |= positionToSingleBit(position - 9);
			}
		}
	}

	static void initializeRookRays()
	{
		for (unsigned char x = 0; x < 8; ++x)
		{
			for (unsigned char y = 0; y < 8; ++y)
			{
				UPWARD_RAYS[y * 8 + x + 1] = createRay(x, y, 0, 1);
				DOWNWARD_RAYS[y * 8 + x + 1] = createRay(x, y, 0, -1);
				LEFTWARD_RAYS[y * 8 + x + 1] = createRay(x, y, 1, 0);
				RIGHTWARD_RAYS[y * 8 + x + 1] = createRay(x, y, -1, 0);
			}
		}
	}

	static void initializeBishopRays()
	{
		for (unsigned char x = 0; x < 8; ++x)
		{
			for (unsigned char y = 0; y < 8; ++y)
			{
				UP_LEFT_WARD_RAYS[y * 8 + x + 1] = createRay(x, y, 1, 1);
				UP_RIGHT_WARD_RAYS[y * 8 + x + 1] = createRay(x, y, -1, 1);
				DOWN_LEFT_WARD_RAYS[y * 8 + x + 1] = createRay(x, y, 1, -1);
				DOWN_RIGHT_WARD_RAYS[y * 8 + x + 1] = createRay(x, y, -1, -1);
			}
		}
	}

	static void initializeKnightRays()
	{
		for (char x = 0; x < 8; x++)
		{
			for (char y = 0; y < 8; ++y)
			{
				if (isInsideBoard(x + 2, y + 1)) KNIGHT_IMMEDIATE_KILL[y * 8 + x + 1] |= positionToSingleBit((y + 1) * 8 + x + 3);
				if (isInsideBoard(x + 2, y - 1)) KNIGHT_IMMEDIATE_KILL[y * 8 + x + 1] |= positionToSingleBit((y - 1) * 8 + x + 3);
				if (isInsideBoard(x + 1, y - 2)) KNIGHT_IMMEDIATE_KILL[y * 8 + x + 1] |= positionToSingleBit((y - 2) * 8 + x + 2);
				if (isInsideBoard(x - 1, y - 2)) KNIGHT_IMMEDIATE_KILL[y * 8 + x + 1] |= positionToSingleBit((y - 2) * 8 + x);
				if (isInsideBoard(x - 2, y - 1)) KNIGHT_IMMEDIATE_KILL[y * 8 + x + 1] |= positionToSingleBit((y - 1) * 8 + x - 1);
				if (isInsideBoard(x - 2, y + 1)) KNIGHT_IMMEDIATE_KILL[y * 8 + x + 1] |= positionToSingleBit((y + 1) * 8 + x - 1);
				if (isInsideBoard(x - 1, y + 2)) KNIGHT_IMMEDIATE_KILL[y * 8 + x + 1] |= positionToSingleBit((y + 2) * 8 + x);
				if (isInsideBoard(x + 1, y + 2)) KNIGHT_IMMEDIATE_KILL[y * 8 + x + 1] |= positionToSingleBit((y + 2) * 8 + x + 2);
			}
		}
	}

	static void initializeKingRays()
	{
		for (char x = 0; x < 8; x++)
		{
			for (char y = 0; y < 8; ++y)
			{
				if (isInsideBoard(x + 1, y + 1)) KING_IMMEDIATE_KILL[y * 8 + x + 1] |= positionToSingleBit((y + 1) * 8 + x + 2);
				if (isInsideBoard(x + 1, y)) KING_IMMEDIATE_KILL[y * 8 + x + 1] |= positionToSingleBit((y) * 8 + x + 2);
				if (isInsideBoard(x + 1, y - 1)) KING_IMMEDIATE_KILL[y * 8 + x + 1] |= positionToSingleBit((y - 1) * 8 + x + 2);
				if (isInsideBoard(x, y - 1)) KING_IMMEDIATE_KILL[y * 8 + x + 1] |= positionToSingleBit((y - 1) * 8 + x + 1);
				if (isInsideBoard(x - 1, y - 1)) KING_IMMEDIATE_KILL[y * 8 + x + 1] |= positionToSingleBit((y - 1) * 8 + x);
				if (isInsideBoard(x - 1, y)) KING_IMMEDIATE_KILL[y * 8 + x + 1] |= positionToSingleBit((y) * 8 + x);
				if (isInsideBoard(x - 1, y + 1)) KING_IMMEDIATE_KILL[y * 8 + x + 1] |= positionToSingleBit((y + 1) * 8 + x);
				if (isInsideBoard(x, y + 1)) KING_IMMEDIATE_KILL[y * 8 + x + 1] |= positionToSingleBit((y + 1) * 8 + x + 1);
			}
		}
	}
};