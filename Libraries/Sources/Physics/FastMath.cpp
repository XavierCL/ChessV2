#include "../../Headers/Physics/FastMath.h"


const unsigned long long isolateMostSignificantBit(unsigned long long n)
{
	n |= (n >> 1);
	n |= (n >> 2);
	n |= (n >> 4);
	n |= (n >> 8);
	n |= (n >> 16);
	n |= (n >> 32);
	return n & ~(n >> 1);
}

const unsigned long long isolateLeastSignificantBit(const unsigned long long& n)
{
	return n & (0 - n);
}

const unsigned char debruijn64Table[64] = {
	0,  1, 48,  2, 57, 49, 28,  3,
	61, 58, 50, 42, 38, 29, 17,  4,
	62, 55, 59, 36, 53, 51, 43, 22,
	45, 39, 33, 30, 24, 18, 12,  5,
	63, 47, 56, 27, 60, 41, 37, 16,
	54, 35, 52, 21, 44, 32, 23, 11,
	46, 26, 40, 15, 34, 20, 31, 10,
	25, 14, 19,  9, 13,  8,  7,  6
};

const unsigned char singleBitToPosition(const unsigned long long& n)
{
	return debruijn64Table[(n * 0x03f79d71b4cb0a89) >> 58] + (unsigned char)(n != 0);
}

const unsigned char positionOfMostSignificantBit(unsigned long long n)
{
	return singleBitToPosition(isolateMostSignificantBit(n));
}

const unsigned char positionOfLeastSignificantBit(const unsigned long long& n)
{
	return singleBitToPosition(isolateLeastSignificantBit(n));
}

const unsigned long long positionToSingleBit(const unsigned char& n)
{
	return (unsigned long long)1 << n-1;
}

const unsigned char bitBoardPopulationCount(unsigned long long n)
{
	n = n - ((n >> 1) & 0x5555555555555555);
	n = ((n >> 2) & 0x3333333333333333) + (n & 0x3333333333333333);
	n = ((n >> 4) + n) & 0x0F0F0F0F0F0F0F0F;
	n = ((n >> 8) + n) & 0x00FF00FF00FF00FF;
	n = ((n >> 16) + n) & 0x0000FFFF0000FFFF;
	return ((n >> 32) + n) & 0x00000000000000FF;
}