#pragma once

#include "PHCompoundScore.h"

#include <vector>
#include <algorithm>

class PHGameScore
{
public:

	PHGameScore(const double &currentBoardScore)
		: _depth(1),
		_score(currentBoardScore)
	{}

	PHGameScore(const double &currentBoardScore, const PHGameScore& parent)
		: _depth(parent._depth + 1),
		_score(PHCompoundScore::fromScoreAndParent(currentBoardScore, parent._score, parent._depth))
	{}

	PHGameScore(const std::vector<PHGameScore> &children, const bool &isWhiteTurn)
		: _depth(children.front()._depth - 1),
		_score(isWhiteTurn
			? PHCompoundScore::fromBestWhiteOf(toScores(children))
			: PHCompoundScore::fromBestBlackOf(toScores(children)))
	{}

	void setScore(const std::vector<PHGameScore> &children, const bool &isWhiteTurn)
	{
		_score = isWhiteTurn
			? PHCompoundScore::fromBestWhiteOf(toScores(children))
			: PHCompoundScore::fromBestBlackOf(toScores(children));
	}

	bool winsOver(const bool &isWhiteTurn, const PHGameScore& other) const
	{
		return isWhiteTurn
			? _score.whiteWinsOver(other._score)
			: _score.blackWinsOver(other._score);
	}

	bool scoreEquals(const PHGameScore &other) const
	{
		return _score == other._score;
	}

	std::vector<PHCompoundScore> toScores(const std::vector<PHGameScore> &gameScores)
	{
		std::vector<PHCompoundScore> scores;

		std::transform(gameScores.begin(), gameScores.end(), std::back_inserter(scores), [](const PHGameScore &gameScore)
		{
			return gameScore._score;
		});

		return scores;
	}

private:

	const size_t _depth;

	PHCompoundScore _score;
};