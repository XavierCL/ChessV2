#pragma once

#include "../../Game/GameSet.h"

#include <functional>

class GameScore
{
public:
	GameScore(const GameSet& gameSet)
		: _depth(1),
		_score(getScore(gameSet)),
		_dephasedScore(0),
		_relativeProbability(1),
		_dephasedProbabilityScore(0),
		_probabilityTerminal(gameSet.getStatus() != GameStatus::LIVE)
	{
		_averageScore = _score;
	}

	GameScore(const GameSet& gameSet, const GameScore& parentScore)
		: _depth(parentScore._depth + 1),
		_score(getScore(gameSet)),
		_dephasedScore(parentScore._dephasedScore + parentScore._depth * (parentScore._score - _score)),
		_relativeProbability(1),
		_probabilityTerminal(gameSet.getStatus() != GameStatus::LIVE)
	{
		_averageScore = _score;
		_dephasedProbabilityScore = _dephasedScore;
	}

	template<typename _ContainerType>
	const void gatherChildren(const _ContainerType& gameScoreHolders)
	{
		double probabilitySum = gameScoreHolders[0]->gameScore()._score;
		size_t scoreIndex = 0;
		std::function<bool(const GameScore&, const GameScore&)> firstScoreWin = [](const GameScore& first, const GameScore& second) {
			return first.whiteWinsOver(second);
		};
		std::function<bool(const GameScore&, const double&, const GameScore&, const double&)> firstProbabilityWin = [](const GameScore& first, const double& firstProbability, const GameScore& second, const double& secondProbability) {
			return first.betterWhiteProbability(firstProbability, secondProbability, second);
		};
		if (_depth % 2 == 0)
		{
			firstScoreWin = [](const GameScore& first, const GameScore& second) {
				return first.blackWinsOver(second);
			};
			firstProbabilityWin = [](const GameScore& first, const double& firstProbability, const GameScore& second, const double& secondProbability) {
				return first.betterBlackProbability(firstProbability, secondProbability, second);
			};
		}

		size_t relativeProbabilityIndex;
		for (relativeProbabilityIndex = 0; relativeProbabilityIndex < gameScoreHolders.size(); ++relativeProbabilityIndex)
		{
			if (!gameScoreHolders[relativeProbabilityIndex]->gameScore()._probabilityTerminal)
			{
				break;
			}
		}

		if (relativeProbabilityIndex < gameScoreHolders.size())
		{
			double currentProbability = _depth % 2 == 1
				? (gameScoreHolders[relativeProbabilityIndex]->gameScore()._score + 1) / 2
				: (-gameScoreHolders[relativeProbabilityIndex]->gameScore()._score + 1) / 2;
			_relativeProbability = currentProbability * gameScoreHolders[relativeProbabilityIndex]->gameScore()._relativeProbability;
			_dephasedProbabilityScore = gameScoreHolders[relativeProbabilityIndex]->gameScore()._dephasedProbabilityScore;
		}
		_score = gameScoreHolders[0]->gameScore()._score;
		_averageScore = gameScoreHolders[0]->gameScore()._averageScore;
		_dephasedScore = gameScoreHolders[0]->gameScore()._dephasedScore;

		for (size_t gameScoreCounter = 1; gameScoreCounter < gameScoreHolders.size(); ++gameScoreCounter)
		{
			if (!gameScoreHolders[gameScoreCounter]->gameScore()._probabilityTerminal)
			{
				double currentProbability = _depth % 2 == 1
					? (gameScoreHolders[gameScoreCounter]->gameScore()._score + 1) / 2
					: (-gameScoreHolders[gameScoreCounter]->gameScore()._score + 1) / 2;
				double currentRelativeProbability = currentProbability * gameScoreHolders[gameScoreCounter]->gameScore()._relativeProbability;
				if (firstProbabilityWin(gameScoreHolders[gameScoreCounter]->gameScore(), currentRelativeProbability, gameScoreHolders[relativeProbabilityIndex]->gameScore(), _relativeProbability))
				{
					relativeProbabilityIndex = gameScoreCounter;
					_relativeProbability = currentRelativeProbability;
					_dephasedProbabilityScore = gameScoreHolders[gameScoreCounter]->gameScore()._dephasedProbabilityScore;
				}
				probabilitySum += currentProbability;
			}
			if (firstScoreWin(gameScoreHolders[gameScoreCounter]->gameScore(), gameScoreHolders[scoreIndex]->gameScore()))
			{
				_score = gameScoreHolders[gameScoreCounter]->gameScore()._score;
				_dephasedScore = gameScoreHolders[gameScoreCounter]->gameScore()._dephasedScore;
				scoreIndex = gameScoreCounter;
			}
			_averageScore += gameScoreHolders[gameScoreCounter]->gameScore()._averageScore;
		}
		_averageScore /= gameScoreHolders.size();
		if (relativeProbabilityIndex < gameScoreHolders.size())
		{
			_relativeProbability /= probabilitySum;
			_probabilityTerminal = false;
		}
		else
		{
			_probabilityTerminal = true;
		}
	}

	const bool hasSameProbability(const GameScore& other) const
	{
		return _relativeProbability == other._relativeProbability
			&& _dephasedProbabilityScore == other._dephasedProbabilityScore;
	}

	const bool hasSameBiaisedScore(const GameScore& other) const
	{
		return _score == other._score
			&& _dephasedScore == other._dephasedScore
			&& _averageScore == other._averageScore;
	}

	const double utility() const
	{
		return (_score + _averageScore * AVERAGE_RATIO) / (1 + AVERAGE_RATIO);
	}

private:

	static const double getScore(const GameSet& gameSet)
	{
		if (gameSet.getStatus() == GameStatus::FIFTY_MOVE || gameSet.getStatus() == GameStatus::NO_LEGAL_MOVE || gameSet.getStatus() == GameStatus::THREEFOLD_REPETITION)
			return 0;
		else if (gameSet.getStatus() == GameStatus::WHITE_WIN)
			return 1;
		else if (gameSet.getStatus() == GameStatus::BLACK_WIN)
			return -1;
		else
		{
			double whiteScore = 0;
			whiteScore += 1 * (double)gameSet.currentBoard().bitBoards().populationCount(PieceType::PAWN, true);
			whiteScore += 3 * (double)gameSet.currentBoard().bitBoards().populationCount(PieceType::KNIGHT, true);
			whiteScore += 3 * (double)gameSet.currentBoard().bitBoards().populationCount(PieceType::BISHOP, true);
			whiteScore += 5 * (double)gameSet.currentBoard().bitBoards().populationCount(PieceType::ROOK, true);
			whiteScore += 9 * (double)gameSet.currentBoard().bitBoards().populationCount(PieceType::QUEEN, true);

			double blackScore = 0;
			blackScore += 1 * (double)gameSet.currentBoard().bitBoards().populationCount(PieceType::PAWN, false);
			blackScore += 3 * (double)gameSet.currentBoard().bitBoards().populationCount(PieceType::KNIGHT, false);
			blackScore += 3 * (double)gameSet.currentBoard().bitBoards().populationCount(PieceType::BISHOP, false);
			blackScore += 5 * (double)gameSet.currentBoard().bitBoards().populationCount(PieceType::ROOK, false);
			blackScore += 9 * (double)gameSet.currentBoard().bitBoards().populationCount(PieceType::QUEEN, false);

			const double score = (whiteScore / (blackScore + 1)) - (blackScore / (whiteScore + 1));

			return FastMath::sigmoid(score);
		}
	}

	static const double AVERAGE_RATIO;

	double _score;
	double _averageScore;
	double _relativeProbability;
	size_t _depth;
	int _dephasedScore;
	int _dephasedProbabilityScore;
	bool _probabilityTerminal;

	const bool betterWhiteProbability(const double& thisRelativeProbability, const double& otherRelativeProbability, const GameScore& other) const
	{
		if (thisRelativeProbability > otherRelativeProbability)
		{
			return true;
		}
		else if (thisRelativeProbability < otherRelativeProbability)
		{
			return false;
		}
		else
		{
			if (_dephasedProbabilityScore > other._dephasedProbabilityScore)
			{
				return true;
			}
			else if (_dephasedProbabilityScore < other._dephasedProbabilityScore)
			{
				return false;
			}
			else
			{
				return false;
			}
		}
	}

	const bool betterBlackProbability(const double& thisRelativeProbability, const double& otherRelativeProbability, const GameScore& other) const
	{
		if (thisRelativeProbability > otherRelativeProbability)
		{
			return true;
		}
		else if (thisRelativeProbability < otherRelativeProbability)
		{
			return false;
		}
		else
		{
			if (_dephasedProbabilityScore < other._dephasedProbabilityScore)
			{
				return true;
			}
			else if (_dephasedProbabilityScore > other._dephasedProbabilityScore)
			{
				return false;
			}
			else
			{
				return false;
			}
		}
	}

	const bool whiteWinsOver(const GameScore& other) const
	{
		if (_score > other._score)
		{
			return true;
		}
		else if (_score < other._score)
		{
			return false;
		}
		else
		{
			if (_dephasedScore > other._dephasedScore)
			{
				return true;
			}
			else if (_dephasedScore < other._dephasedScore)
			{
				return false;
			}
			else
			{
				if (_averageScore > other._averageScore)
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

	const bool blackWinsOver(const GameScore& other) const
	{
		if (_score < other._score)
		{
			return true;
		}
		else if (_score > other._score)
		{
			return false;
		}
		else
		{
			if (_dephasedScore < other._dephasedScore)
			{
				return true;
			}
			else if (_dephasedScore > other._dephasedScore)
			{
				return false;
			}
			else
			{
				if (_averageScore < other._averageScore)
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
};