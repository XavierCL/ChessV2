#ifndef HEURISTIC_SELECTIVE_ARTIFICIAL_H
#define HEURISTIC_SELECTIVE_ARTIFICIAL_H

#include "Artificial.h"

class HeuristicSelectiveArtificial: public Artificial
{
public:
	HeuristicSelectiveArtificial(const unsigned char& depth, const unsigned char& threadCount, std::minstd_rand0* generator)
		:_depth(depth),
		_threadCount(threadCount),
		_generator(generator)
	{}

	Move const * getMove(const GameSet& gameSet) const override
	{
		return gameSet.getLegals()[0];
	}
private:
	const unsigned char _depth;
	const unsigned char _threadCount;
	std::minstd_rand0* const _generator;

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

#endif