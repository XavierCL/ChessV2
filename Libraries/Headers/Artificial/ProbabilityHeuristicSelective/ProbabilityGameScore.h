#pragma once

#include "../../Game/GameSet.h"

#include <functional>

class ProbabilityGameScore
{
public:
	ProbabilityGameScore(const GameSet& gameSet)
		: _depth(1),
		_score(getScore(gameSet)),
		_dephasedScore(0),
		_relativeProbability(1),
		_dephasedProbabilityScore(0),
		_probabilityTerminal(gameSet.getStatus() != GameStatus::LIVE)
	{
		_probability = computeProbability();
		_averageScore = _score;
	}

	ProbabilityGameScore(const GameSet& gameSet, const ProbabilityGameScore& parentScore)
		: _depth(parentScore._depth + 1),
		_score(getScore(gameSet)),
		_dephasedScore(parentScore._dephasedScore + parentScore._depth * (parentScore._score - _score)),
		_relativeProbability(1),
		_probabilityTerminal(gameSet.getStatus() != GameStatus::LIVE)
	{
		_probability = computeProbability();
		_averageScore = _score;
		_dephasedProbabilityScore = _dephasedScore;
	}

	template<typename _ContainerType>
	const void gatherChildren(const _ContainerType& gameScoreHolders)
	{
		size_t scoreIndex = 0;
		std::function<bool(const ProbabilityGameScore&, const ProbabilityGameScore&)> firstScoreWin = [](const ProbabilityGameScore& first, const ProbabilityGameScore& second) {
			return first.whiteWinsOver(second);
		};
		std::function<bool(const ProbabilityGameScore&, const ProbabilityGameScore&)> firstProbabilityWin = [](const ProbabilityGameScore& first, const ProbabilityGameScore& second) {
			return first.betterWhiteProbability(second);
		};
		if (_depth % 2 == 0)
		{
			firstScoreWin = [](const ProbabilityGameScore& first, const ProbabilityGameScore& second) {
				return first.blackWinsOver(second);
			};
			firstProbabilityWin = [](const ProbabilityGameScore& first, const ProbabilityGameScore& second) {
				return first.betterBlackProbability(second);
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
			_relativeProbability = gameScoreHolders[relativeProbabilityIndex]->gameScore()._probability;
			_dephasedProbabilityScore = gameScoreHolders[relativeProbabilityIndex]->gameScore()._dephasedProbabilityScore;
		}

		double probabilitySum = 0;
		if (relativeProbabilityIndex == 0)
		{
			probabilitySum = gameScoreHolders[0]->gameScore()._score;
		}
		// else loops starts at one

		_score = gameScoreHolders[0]->gameScore()._score;
		_averageScore = gameScoreHolders[0]->gameScore()._averageScore;
		_dephasedScore = gameScoreHolders[0]->gameScore()._dephasedScore;

		for (size_t gameScoreCounter = 1; gameScoreCounter < gameScoreHolders.size(); ++gameScoreCounter)
		{
			if (!gameScoreHolders[gameScoreCounter]->gameScore()._probabilityTerminal)
			{
				double currentRelativeProbability = gameScoreHolders[gameScoreCounter]->gameScore()._probability;
				if (firstProbabilityWin(gameScoreHolders[gameScoreCounter]->gameScore(), gameScoreHolders[relativeProbabilityIndex]->gameScore()))
				{
					relativeProbabilityIndex = gameScoreCounter;
					_relativeProbability = currentRelativeProbability;
					_dephasedProbabilityScore = gameScoreHolders[gameScoreCounter]->gameScore()._dephasedProbabilityScore;
				}
				probabilitySum += currentRelativeProbability;
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
		if (probabilitySum > 0)
		{
			_relativeProbability /= probabilitySum;
			_probabilityTerminal = false;
		}
		else
		{
			_probabilityTerminal = true;
		}
		_probability = computeProbability();
	}

	const bool isSameScore(const ProbabilityGameScore& other) const
	{
		return _averageScore == other._averageScore
			&& _probability == other._probability
			&& _dephasedScore == other._dephasedScore
			&& _dephasedProbabilityScore == other._dephasedProbabilityScore
			&& _probabilityTerminal == other._probabilityTerminal;
	}

	const double utility() const
	{
		return (_score + _averageScore * AVERAGE_RATIO) / (1 + AVERAGE_RATIO);
	}

	const bool terminal() const
	{
		return _probabilityTerminal;
	}

	const bool betterWhiteProbability(const ProbabilityGameScore& other) const
	{
		if (_probability > other._probability)
		{
			return true;
		}
		else if (_probability < other._probability)
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

	const bool betterBlackProbability(const ProbabilityGameScore& other) const
	{
		if (_probability > other._probability)
		{
			return true;
		}
		else if (_probability < other._probability)
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

	const bool whiteWinsOver(const ProbabilityGameScore& other) const
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

	const bool blackWinsOver(const ProbabilityGameScore& other) const
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
	double _probability;
	size_t _depth;
	int _dephasedScore;
	int _dephasedProbabilityScore;
	bool _probabilityTerminal;

	const double computeProbability() const
	{
		const double normalizedScore = _depth % 2 == 1
			? (_score + 1) / 2
			: (-_score + 1) / 2;
		return _relativeProbability * normalizedScore;
	}
};