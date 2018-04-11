#ifndef HEURISTIC_SELECTIVE_ARTIFICIAL_H
#define HEURISTIC_SELECTIVE_ARTIFICIAL_H

#include "HeuristicSelectiveGameTree.h"

#include "../Artificial.h"

#include "../../utils/Logger.hpp"

#include <ctime>
#include <string>

class HeuristicSelectiveArtificial : public Artificial
{
public:
	HeuristicSelectiveArtificial(const unsigned long long &msSelfTime, const unsigned char& threadCount, const size_t &maxNodeCount, std::minstd_rand0* generator)
		: _msSelfTime(msSelfTime),
		_threadCount(threadCount),
		_maxNodeCount(maxNodeCount),
		_gameTree(GameSet())
	{
		HeuristicSelectiveGameNode::GENERATOR = generator;
		HeuristicSelectiveGameNode::NODES.clear();
	}

	Move const * getMove(const GameSet& gameSet) override
	{

		if (gameSet.currentBoard() == Board())
		{
			Logger::info("Starting\n");
		}

		_gameTree.playMove(gameSet);

		// Timing should start when function is first called,
		// but here I don't yet handle the thread deletion logic,
		// so I'm only counting the new development
		const clock_t begin_time = clock();

		_gameTree.developUntil([this, &begin_time]() {
			const clock_t end_time = clock();
			bool shouldStop = (1000 * (end_time - begin_time)) / CLOCKS_PER_SEC >= _msSelfTime || _gameTree.size() * sizeof(HeuristicSelectiveGameNode) >= _maxNodeCount;
			return shouldStop;
		});
		printDebugInfo();
		return _gameTree.playMove();
	}

private:
	const unsigned long long MS_DEBUG_TIME = 1000;

	const unsigned long long _msSelfTime;
	const unsigned char _threadCount;
	const size_t _maxNodeCount;

	HeuristicSelectiveGameTree _gameTree;

	void printDebugInfo()
	{
		Logger::info("Is white: " + std::to_string(_gameTree.root()->gameSet().isWhiteTurn()) + "\n"
			+ "Whole tree: " + std::to_string(_gameTree.root()->size()) + "\n"
			+ "Utility : " + std::to_string(_gameTree.root()->gameScore().utility()) + "\n\n");
	}
};

#endif