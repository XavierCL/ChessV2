#pragma once

#include "PHCompoundScore.h"

#include <vector>
#include <algorithm>

class PHGameScore
{
public:

	PHGameScore(const double &currentBoardScore, const bool &isWhiteTurn)
		: _probability(scoreToProbability(currentBoardScore, isWhiteTurn)),
		_accumulatedChildProbability(scoreToProbability(currentBoardScore, isWhiteTurn))
	{}

	void setScore(const std::vector<PHGameScore> &children, const bool &isWhiteTurn)
	{
		_probability = -1;
		_accumulatedChildProbability = 0;
		for (const auto& child : children)
		{
			if (child.winsOver(*this))
			{
				_probability = child._probability;
			}
			_accumulatedChildProbability += child._accumulatedChildProbability;
		}
		_probability /= _accumulatedChildProbability;
	}

	bool winsOver(const PHGameScore& other) const
	{
		return _probability > other._probability;
	}

	bool probabilityEquals(const PHGameScore& other) const
	{
		return _probability == other._probability;
	}

private:

	static double scoreToProbability(const double &score, const bool &isWhiteTurn)
	{
		return isWhiteTurn
			? (score + 1) / 2.0
			: 1 - (score + 1) / 2.0;
	}

	double _probability;
	double _accumulatedChildProbability;
};
