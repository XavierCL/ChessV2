#ifndef ARTIFICIAL_H
#define ARTIFICIAL_H

#include "../Game/GameSet.h"

#include <random>

class Artificial
{
public:
	virtual Move const * getMove(const GameSet& gameSet) = 0;
};

#endif