#pragma once

const unsigned long long isolateMostSignificantBit(unsigned long long n);

const unsigned long long isolateLeastSignificantBit(const unsigned long long& n);

const unsigned char singleBitToPosition(const unsigned long long& n);

const unsigned char positionOfMostSignificantBit(unsigned long long n);

const unsigned char positionOfLeastSignificantBit(const unsigned long long& n);

const unsigned long long positionToSingleBit(const unsigned char& n);

const unsigned char bitBoardPopulationCount(unsigned long long n);

template <typename _FunctionType>
void foreachBit(unsigned long long bitBoard, const _FunctionType& foreachFunction)
{
	while (bitBoard)
	{
		const unsigned long long singleBit = isolateLeastSignificantBit(bitBoard);
		bitBoard ^= singleBit;
		foreachFunction(singleBitToPosition(singleBit));
	}
}