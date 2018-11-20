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

	GameScore& operator=(const GameScore &other)
	{
		_score = other._score;
		return *this;
	}

	bool winsOver(const bool &isWhiteTurn, const GameScore& other) const
	{
		return isWhiteTurn
			? _score.whiteWinsOver(other._score)
			: _score.blackWinsOver(other._score);
	}

	bool operator==(const GameScore &other) const
	{
		return _depth == other._depth
			&& _score == other._score;
	}

	bool operator!=(const GameScore &other) const
	{
		return !operator==(other);
	}

	std::vector<CompoundScore> toScores(const std::vector<GameScore> &gameScores)
	{
		std::vector<CompoundScore> scores;

		std::transform(gameScores.begin(), gameScores.end(), scores.begin(), [](const GameScore &gameScore)
		{
			return gameScore._score;
		});

		return scores;
	}

private:

	const size_t _depth;

	CompoundScore _score;
};