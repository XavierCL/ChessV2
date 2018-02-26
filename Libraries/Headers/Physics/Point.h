#pragma once

#include "../utils/FastMath.hpp"

class Point
{
public:

	Point()
		:_x(0),
		_y(0)
	{}

	Point(const unsigned long long& n)
		: _x((FastMath::singleBitToPosition(n) - 1) % 8),
		_y((FastMath::singleBitToPosition(n) - 1) / 8)
	{}

	Point(const unsigned char& x, const unsigned char& y)
		:_x(x),
		_y(y)
	{}

	Point& operator=(const Point& point)
	{
		_x = point._x;
		_y = point._y;
		return *this;
	}

	const unsigned char x() const
	{
		return _x;
	}

	const unsigned char y() const
	{
		return _y;
	}

	const bool operator==(const Point& point)
	{
		return _x == point._x && _y == point._y;
	}

	const bool operator!=(const Point& point)
	{
		return !operator==(point);
	}

	const unsigned long long singleBitBoard() const
	{
		return FastMath::positionToSingleBit(_x + _y * 8 + 1);
	}
private:
	unsigned char _x;
	unsigned char _y;
};