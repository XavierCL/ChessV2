#pragma once

#include "Artificial.h"

class BruteRecurseWithLegalsArtificial: public Artificial
{
public:
	BruteRecurseWithLegalsArtificial(const unsigned char& depth, std::minstd_rand0* generator)
		:_depth(depth),
		_generator(generator)
	{}

	Move const * getMove(const GameSet& gameSet) const override
	{
		if (_depth == 0)
		{
			std::uniform_int_distribution<int> aDistributor(0, gameSet.getLegals().size() - 1);
			return gameSet.getLegals()[aDistributor(*_generator)];
		}
		else
		{
			std::vector<Move const *> bestMoves(1, gameSet.getLegals()[0]);
			if (gameSet.isWhiteTurn())
			{
				double maxScore = revaluate(gameSet.playMove(*gameSet.getLegals()[0]), _depth - 1, -1000);
				for (size_t i = 1; i < gameSet.getLegals().size(); ++i)
				{
					double currentScore = revaluate(gameSet.playMove(*gameSet.getLegals()[i]), _depth - 1, maxScore);
					if (maxScore < currentScore)
					{
						maxScore = currentScore;
						bestMoves.clear();
						bestMoves.push_back(gameSet.getLegals()[i]);
					}
					else if (maxScore <= currentScore + ACCEPTED_RATIO)
					{
						bestMoves.push_back(gameSet.getLegals()[i]);
					}
				}
			}
			else
			{
				double minScore = revaluate(gameSet.playMove(*gameSet.getLegals()[0]), _depth - 1, 1000);
				for (size_t i = 1; i < gameSet.getLegals().size(); ++i)
				{
					double currentScore = revaluate(gameSet.playMove(*gameSet.getLegals()[i]), _depth - 1, minScore);
					if (minScore > currentScore)
					{
						minScore = currentScore;
						bestMoves.clear();
						bestMoves.push_back(gameSet.getLegals()[i]);
					}
					else if (minScore > currentScore - ACCEPTED_RATIO)
					{
						bestMoves.push_back(gameSet.getLegals()[i]);
					}
				}
			}
			std::uniform_int_distribution<int> aDistributor(0, bestMoves.size() - 1);
			return bestMoves[aDistributor(*_generator)];
		}
	}
private:
	const double AVERAGE_RATIO = 0.00001;
	const double LEGAL_RATIO = 0.001;
	const double ACCEPTED_RATIO = 0.0000001;

	const unsigned char _depth;
	std::minstd_rand0* const _generator;

	const double revaluate(const GameSet& gameSet, const unsigned char& depth, const double& lastBound) const
	{
		if (shouldStopSearch(gameSet.getStatus()))
		{
			return getScoreByStatus(gameSet.getStatus());
		}
		else if (depth == 0)
		{
			return evaluate(gameSet);
		}
		else
		{
			double average = 0;
			if (gameSet.isWhiteTurn())
			{
				double maxScore = revaluate(gameSet.playMove(*gameSet.getLegals()[0]), depth - 1, -1000);
				average += maxScore;
				size_t i = 1;
				for (; i < gameSet.getLegals().size() && lastBound >(maxScore + (average / i) * AVERAGE_RATIO) / (1 + AVERAGE_RATIO); ++i)
				{
					double currentScore = revaluate(gameSet.playMove(*gameSet.getLegals()[i]), depth - 1, maxScore);
					average += currentScore;
					if (maxScore < currentScore)
					{
						maxScore = currentScore;
					}
				}
				return (maxScore + (average / i) * AVERAGE_RATIO) / (1 + AVERAGE_RATIO);
			}
			else
			{
				double minScore = revaluate(gameSet.playMove(*gameSet.getLegals()[0]), depth - 1, 1000);
				average += minScore;
				size_t i = 1;
				for (; i < gameSet.getLegals().size() && lastBound < (minScore + (average / i) * AVERAGE_RATIO) / (1 + AVERAGE_RATIO); ++i)
				{
					double currentScore = revaluate(gameSet.playMove(*gameSet.getLegals()[i]), depth - 1, minScore);
					average += currentScore;
					if (minScore > currentScore)
					{
						minScore = currentScore;
					}
				}
				return (minScore + (average / i) * AVERAGE_RATIO) / (1 + AVERAGE_RATIO);
			}
		}
	}

	const double evaluate(const GameSet& gameSet) const
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

		double legalMovePoints = (gameSet.isWhiteTurn() * 2 - 1) * (double)gameSet.getLegals().size() * LEGAL_RATIO;

		return (whiteScore / (blackScore + 1)) - (blackScore / (whiteScore + 1)) + legalMovePoints;
	}

	const bool shouldStopSearch(const GameStatus& gameStatus) const
	{
		return gameStatus != GameStatus::LIVE;
	}

	const double getScoreByStatus(const GameStatus& gameStatus) const
	{
		if (gameStatus == GameStatus::FIFTY_MOVE || gameStatus == GameStatus::NO_LEGAL_MOVE || gameStatus == GameStatus::THREEFOLD_REPETITION)
			return 0;
		else if (gameStatus == GameStatus::WHITE_WIN)
			return 100;
		else if (gameStatus == GameStatus::BLACK_WIN)
			return -100;
		else
			throw std::invalid_argument("Analysed static score of a still live gamestatus.");
	}
};