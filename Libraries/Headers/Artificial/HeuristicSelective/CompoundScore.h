#pragma once

#include <vector>
#include <numeric>

class CompoundScore
{
public:

	CompoundScore(const double &boardScore) :
		_boardScore(boardScore),
		_dephasedBoardScore(0)
	{}

	CompoundScore(const double& boardScore, const double& dephasedBoardScore) :
		_boardScore(boardScore),
		_dephasedBoardScore(dephasedBoardScore)
	{}

	static CompoundScore fromScoreAndParent(const double &boardScore, const CompoundScore &parentCompoundScore, const double &parentDepth)
	{
		// formula for calculating dephased score, so that (1 and 4) == (2 and 3)
		return CompoundScore(
			boardScore,
			parentCompoundScore._dephasedBoardScore + parentDepth * (parentCompoundScore._boardScore - boardScore)
		);
	}

	static CompoundScore fromBestWhiteOf(const std::vector<CompoundScore> &compoundScores)
	{
		if (compoundScores.size() < 1)
		{
			throw std::exception("Cannot compute best white score of no values");
		}

		return *std::max_element(compoundScores.begin(), compoundScores.end(), [](const CompoundScore& a, const CompoundScore& b)
		{
			return b.whiteWinsOver(a);
		});
	}

	static CompoundScore fromBestBlackOf(const std::vector<CompoundScore> &compoundScores)
	{
		if (compoundScores.size() < 1)
		{
			throw std::exception("Cannot compute best black score of no values");
		}

		return *std::max_element(compoundScores.begin(), compoundScores.end(), [](const CompoundScore& a, const CompoundScore& b)
		{
			return b.blackWinsOver(a);
		});
	}

	static CompoundScore fromAverageOf(const std::vector<CompoundScore> &compoundScores)
	{
		if (compoundScores.size() < 1)
		{
			throw std::exception("Cannot compute average score of no values");
		}

		const auto summedBoardScores(std::accumulate(
			compoundScores.begin(),
			compoundScores.end(),
			0.0,
			[](const double& accumulated, const CompoundScore& next) { return accumulated + next._boardScore; }
		));

		const auto summedDephasedScore(std::accumulate(
			compoundScores.begin(),
			compoundScores.end(),
			0.0,
			[](const double& accumulated, const CompoundScore& next) { return accumulated + next._dephasedBoardScore; }
		));

		return CompoundScore(summedBoardScores / compoundScores.size(), summedDephasedScore / compoundScores.size());
	}

	CompoundScore& operator=(const CompoundScore& other)
	{
		_boardScore = other._boardScore;
		_dephasedBoardScore = other._dephasedBoardScore;

		return *this;
	}

	double utility() const
	{
		return _boardScore;
	}

	bool whiteWinsOver(const CompoundScore &other) const
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

	bool blackWinsOver(const CompoundScore &other) const
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

	bool operator==(const CompoundScore& other) const
	{
		return _boardScore == other._boardScore
			&& _dephasedBoardScore == other._dephasedBoardScore;
	}

	bool operator!=(const CompoundScore& other) const
	{
		return !operator==(other);
	}

private:
	double _boardScore;
	double _dephasedBoardScore;
};