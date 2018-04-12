#pragma once

class BiaisedGameScore
{
public:

	BiaisedGameScore()
		: _boardScore(0),
		_defasedBoardScore(0),
		_depth(1),
		_averageUtility(0)
	{}

	BiaisedGameScore(const double &currentBoardScore, const BiaisedGameScore& parent)
		: _boardScore(currentBoardScore),
		_defasedBoardScore(computeDefasedBoardScoreFromParent(currentBoardScore, parent)),
		_depth(parent._depth + 1),
		_averageUtility(currentBoardScore)
	{}

	BiaisedGameScore(const double &currentBoardScore, const BiaisedGameScore& parent, const double &averageUtility)
		: _boardScore(currentBoardScore),
		_defasedBoardScore(computeDefasedBoardScoreFromParent(currentBoardScore, parent)),
		_depth(parent._depth + 1),
		_averageUtility(averageUtility)
	{}

	const bool biaisedWhiteWinsOver(const BiaisedGameScore &other) const
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
			if (_defasedBoardScore > other._defasedBoardScore)
			{
				return true;
			}
			else if (_defasedBoardScore < other._defasedBoardScore)
			{
				return false;
			}
			else
			{
				if (_averageUtility > other._averageUtility)
				{
					return true;
				}
				else
				{
					return false;
				}
			}
		}
	}

	const bool biaisedBlackWinsOver(const BiaisedGameScore &other) const
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
			if (_defasedBoardScore < other._defasedBoardScore)
			{
				return true;
			}
			else if (_defasedBoardScore > other._defasedBoardScore)
			{
				return false;
			}
			else
			{
				if (_averageUtility < other._averageUtility)
				{
					return true;
				}
				else
				{
					return false;
				}
			}
		}
	}

	const bool operator==(const BiaisedGameScore& other) const
	{
		return _boardScore == other._boardScore
			&& _defasedBoardScore == other._defasedBoardScore
			&& _depth == other._depth
			&& _averageUtility == other._averageUtility;
	}

	const bool operator!=(const BiaisedGameScore &other) const
	{
		return !operator==(other);
	}

	const double averageUtility() const
	{
		return _averageUtility;
	}

	void averageUtility(const double &averageUtility)
	{
		_averageUtility = averageUtility;
	}

	const double utility() const
	{
		return (_boardScore + _defasedBoardScore * DEFASED_BIAIS + _averageUtility * AVERAGE_BIAIS) / (1 + DEFASED_BIAIS + AVERAGE_BIAIS);
	}

private:

	static const double computeDefasedBoardScoreFromParent(const double &currentBoardScore, const BiaisedGameScore& parent)
	{
		return parent._defasedBoardScore + parent._depth * (parent._boardScore - currentBoardScore);
	}

	static const double AVERAGE_BIAIS;
	static const double DEFASED_BIAIS;

	double _boardScore;
	double _defasedBoardScore;
	size_t _depth;

	double _averageUtility;
};