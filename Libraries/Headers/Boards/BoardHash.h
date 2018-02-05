#pragma once

#include "Board.h"

class BoardHash
{
public:
	size_t operator()(const Board & element) const
	{
		return element.hash();
	}
};