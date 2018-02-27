#pragma once

#include "GameSet.h"

class GameSetHash
{
public:
	const size_t operator()(const GameSet& gameSet)
	{
		return gameSet.hash();
	}
};