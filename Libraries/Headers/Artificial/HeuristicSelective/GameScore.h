#pragma once

#include "CompoundScore.h"

#include <vector>
#include <algorithm>

class GameScore
{
public:

	GameScore(const double &currentBoardScore)
		: _depth(1),
		_score(currentBoardScore)
	{}

	GameScore(const double &currentBoardScore, const GameScore& parent)
		: _depth(parent._depth + 1),
		_score(CompoundScore::fromScoreAndParent(currentBoardScore, parent._score, parent._depth))
	{}

	GameScore(const std::vector<GameScore> &children, const bool &isWhiteTurn)
		: _depth(children.front()._depth - 1),
		_score(isWhiteTurn
			? CompoundScore::fromBestWhiteOf(toScores(children))
			: CompoundScore::fromBestBlackOf(toScores(children)))
	{}

	void setScore(const std::vector<GameScore> &children, const bool &isWhiteTurn)
	{
		_score = isWhiteTurn
			? CompoundScore::fromBestWhiteOf(toScores(children))
			: CompoundScore::fromBestBlackOf(toScores(children));
	}

	bool winsOver(const bool &isWhiteTurn, const GameScore& other) const
	{
		return isWhiteTurn
			? _score.whiteWinsOver(other._score)
			: _score.blackWinsOver(other._score);
	}

	bool scoreEquals(const GameScore &other) const
	{
		return _score == other._score;
	}

	std::vector<CompoundScore> toScores(const std::vector<GameScore> &gameScores)
	{
		std::vector<CompoundScore> scores;

		std::transform(gameScores.begin(), gameScores.end(), std::back_inserter(scores), [](const GameScore &gameScore)
		{
			return gameScore._score;
		});

		return scores;
	}

private:

	const size_t _depth;

	CompoundScore _score;
};