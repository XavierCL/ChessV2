#pragma once

#include "Board.h"

class BoardHash
{
public:
	const size_t operator()(const Board & element) const
	{
		return element.hash();
	}
};