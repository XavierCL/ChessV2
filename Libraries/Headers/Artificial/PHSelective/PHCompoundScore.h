#pragma once

#include <vector>
#include <numeric>

class PHCompoundScore
{
public:

	PHCompoundScore(const double &boardScore) :
		_boardScore(boardScore),
		_dephasedBoardScore(0)
	{}

	PHCompoundScore(const double& boardScore, const double& dephasedBoardScore) :
		_boardScore(boardScore),
		_dephasedBoardScore(dephasedBoardScore)
	{}

	static PHCompoundScore fromScoreAndParent(const double &boardScore, const PHCompoundScore &parentCompoundScore, const double &parentDepth)
	{
		// formula for calculating dephased score, so that (1 and 4) == (2 and 3)
		return PHCompoundScore(
			boardScore,
			parentCompoundScore._dephasedBoardScore + parentDepth * (parentCompoundScore._boardScore - boardScore)
		);
	}

	static PHCompoundScore fromBestWhiteOf(const std::vector<PHCompoundScore> &compoundScores)
	{
		if (compoundScores.size() < 1)
		{
			throw std::exception("Cannot compute best white score of no values");
		}

		return *std::max_element(compoundScores.begin(), compoundScores.end(), [](const PHCompoundScore& a, const PHCompoundScore& b)
		{
			return b.whiteWinsOver(a);
		});
	}

	static PHCompoundScore fromBestBlackOf(const std::vector<PHCompoundScore> &compoundScores)
	{
		if (compoundScores.size() < 1)
		{
			throw std::exception("Cannot compute best black score of no values");
		}

		return *std::max_element(compoundScores.begin(), compoundScores.end(), [](const PHCompoundScore& a, const PHCompoundScore& b)
		{
			return b.blackWinsOver(a);
		});
	}

	static PHCompoundScore fromAverageOf(const std::vector<PHCompoundScore> &compoundScores)
	{
		if (compoundScores.size() < 1)
		{
			throw std::exception("Cannot compute average score of no values");
		}

		const auto summedBoardScores(std::accumulate(
			compoundScores.begin(),
			compoundScores.end(),
			0.0,
			[](const double& accumulated, const PHCompoundScore& next) { return accumulated + next._boardScore; }
		));

		const auto summedDephasedScore(std::accumulate(
			compoundScores.begin(),
			compoundScores.end(),
			0.0,
			[](const double& accumulated, const PHCompoundScore& next) { return accumulated + next._dephasedBoardScore; }
		));

		return PHCompoundScore(summedBoardScores / compoundScores.size(), summedDephasedScore / compoundScores.size());
	}

	PHCompoundScore& operator=(const PHCompoundScore& other)
	{
		_boardScore = other._boardScore;
		_dephasedBoardScore = other._dephasedBoardScore;

		return *this;
	}

	double utility() const
	{
		return _boardScore;
	}

	bool whiteWinsOver(const PHCompoundScore &other) const
	{
		if (_boardScore > other._boardScore)
		{
			return true;
		}
		else if (_boardScore < other._boardScore)
		{
			return false;
		}
		else
		{
			if (_dephasedBoardScore > other._dephasedBoardScore)
			{
				return true;
			}
			else if (_dephasedBoardScore < other._dephasedBoardScore)
			{
				return false;
			}
			else
			{
				return false;
			}
		}
	}

	bool blackWinsOver(const PHCompoundScore &other) const
	{
		if (_boardScore < other._boardScore)
		{
			return true;
		}
		else if (_boardScore > other._boardScore)
		{
			return false;
		}
		else
		{
			if (_dephasedBoardScore < other._dephasedBoardScore)
			{
				return true;
			}
			else if (_dephasedBoardScore > other._dephasedBoardScore)
			{
				return false;
			}
			else
			{
				return false;
			}
		}
	}

	bool operator==(const PHCompoundScore& other) const
	{
		return _boardScore == other._boardScore
			&& _dephasedBoardScore == other._dephasedBoardScore;
	}

	bool operator!=(const PHCompoundScore& other) const
	{
		return !operator==(other);
	}

private:
	double _boardScore;
	double _dephasedBoardScore;
};