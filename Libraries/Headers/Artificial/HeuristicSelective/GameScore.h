#pragma once

class GameScore
{
public:

	GameScore()
		: _boardScore(0),
		_defasedBoardScore(0),
		_depth(1)
	{}

	GameScore(const double &currentBoardScore, const GameScore& parent)
		: _boardScore(currentBoardScore),
		_defasedBoardScore(computeDefasedBoardScoreFromParent(currentBoardScore, parent)),
		_depth(parent._depth + 1)
	{}

	const bool whiteWinsOver(const GameScore &other) const
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
				if (_depth > other._depth)
				{
					return false;
				}
				else if (_depth < other._depth)
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

	const bool blackWinsOver(const GameScore &other) const
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
				if (_depth > other._depth)
				{
					return false;
				}
				else if (_depth < other._depth)
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

	const bool operator==(const GameScore& other) const
	{
		return _boardScore == other._boardScore
			&& _defasedBoardScore == other._defasedBoardScore
			&& _depth == other._depth;
	}

	const bool operator!=(const GameScore &other) const
	{
		return !operator==(other);
	}

private:

	static const double computeDefasedBoardScoreFromParent(const double &currentBoardScore, const GameScore& parent)
	{
		return parent._defasedBoardScore + parent._depth * (parent._boardScore - currentBoardScore);
	}

	static const double AVERAGE_BIAIS;
	static const double DEFASED_BIAIS;

	double _boardScore;
	double _defasedBoardScore;
	size_t _depth;
};