#pragma once

#include "PHCompoundScore.h"

#include <vector>
#include <algorithm>

class PHBiaisedGameScore
{
public:

	PHBiaisedGameScore(const double &currentBoardScore)
		: _depth(1),
		_score(currentBoardScore),
		_averageScore(currentBoardScore)
	{}

	PHBiaisedGameScore(const double &currentBoardScore, const PHBiaisedGameScore &parent)
		: _depth(parent._depth + 1),
		_score(PHCompoundScore::fromScoreAndParent(currentBoardScore, parent._score, parent._depth)),
		_averageScore(currentBoardScore)
	{}

	void setScores(const std::vector<PHBiaisedGameScore> &children, const bool &isWhiteTurn)
	{
		_score = isWhiteTurn
			? PHCompoundScore::fromBestWhiteOf(toScores(children))
			: PHCompoundScore::fromBestBlackOf(toScores(children));

		_averageScore = PHCompoundScore::fromAverageOf(toScores(children));
	}

	bool biaisedWinOver(const bool &isWhiteTurn, const PHBiaisedGameScore &other) const
	{
		return isWhiteTurn
			? biaisedWhiteWinsOver(other)
			: biaisedBlackWinsOver(other);
	}

	bool scoresEquals(const PHBiaisedGameScore &other) const
	{
		return _averageScore == other._averageScore
			&& _score == other._score;
	}

	double biaisedUtility() const
	{
		const double averageUtility = _averageScore.utility();
		return _score.utility() + 0.5 * (averageUtility / (1 + abs(averageUtility)));
	}

private:

	bool biaisedWhiteWinsOver(const PHBiaisedGameScore &other) const
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

	bool biaisedBlackWinsOver(const PHBiaisedGameScore &other) const
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

	std::vector<PHCompoundScore> toScores(const std::vector<PHBiaisedGameScore> gameScores)
	{
		std::vector<PHCompoundScore> scores;

		std::transform(gameScores.begin(), gameScores.end(), std::back_inserter(scores), [](const PHBiaisedGameScore &gameScore)
		{
			return gameScore._score;
		});

		return scores;
	}

	const size_t _depth;

	PHCompoundScore _score;
	PHCompoundScore _averageScore;
};
