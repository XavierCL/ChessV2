#pragma once

#include "CompoundScore.h"

#include <vector>
#include <algorithm>

class BiaisedGameScore
{
public:

	BiaisedGameScore(const double &currentBoardScore)
		: _depth(1),
		_score(currentBoardScore),
		_averageScore(currentBoardScore)
	{}

	BiaisedGameScore(const double &currentBoardScore, const BiaisedGameScore &parent)
		: _depth(parent._depth + 1),
		_score(CompoundScore::fromScoreAndParent(currentBoardScore, parent._score, parent._depth)),
		_averageScore(currentBoardScore)
	{}

	void setScores(const std::vector<BiaisedGameScore> &children, const bool &isWhiteTurn)
	{
		_score = isWhiteTurn
			? CompoundScore::fromBestWhiteOf(toScores(children))
			: CompoundScore::fromBestBlackOf(toScores(children));

		_averageScore = CompoundScore::fromAverageOf(toScores(children));
	}

	bool biaisedWinOver(const bool &isWhiteTurn, const BiaisedGameScore &other) const
	{
		return isWhiteTurn
			? biaisedWhiteWinsOver(other)
			: biaisedBlackWinsOver(other);
	}

	bool scoresEquals(const BiaisedGameScore &other) const
	{
		return _averageScore == other._averageScore
			&& _score == other._score;
	}

	double utility() const
	{
		return _score.utility();
	}

private:

	bool biaisedWhiteWinsOver(const BiaisedGameScore &other) const
	{
		if (_score.whiteWinsOver(other._score))
		{
			return true;
		}
		else if (_score.blackWinsOver(other._score))
		{
			return false;
		}
		else if (_averageScore.whiteWinsOver(other._averageScore))
		{
			return true;
		}
		else if (_averageScore.blackWinsOver(other._averageScore))
		{
			return false;
		}
		else
		{
			return false;
		}
	}

	bool biaisedBlackWinsOver(const BiaisedGameScore &other) const
	{
		if (_score.blackWinsOver(other._score))
		{
			return true;
		}
		else if (_score.whiteWinsOver(other._score))
		{
			return false;
		}
		else if (_averageScore.blackWinsOver(other._averageScore))
		{
			return true;
		}
		else if (_averageScore.whiteWinsOver(other._averageScore))
		{
			return false;
		}
		else
		{
			return false;
		}
	}

	std::vector<CompoundScore> toScores(const std::vector<BiaisedGameScore> gameScores)
	{
		std::vector<CompoundScore> scores;

		std::transform(gameScores.begin(), gameScores.end(), std::back_inserter(scores), [](const BiaisedGameScore &gameScore)
		{
			return gameScore._score;
		});

		return scores;
	}

	const size_t _depth;

	CompoundScore _score;
	CompoundScore _averageScore;
};
