#ifndef HEURISTIC_SELECTIVE_ARTIFICIAL_H
#define HEURISTIC_SELECTIVE_ARTIFICIAL_H

#include "HeuristicSelectiveGameTree.h"

#include "../Artificial.h"

#include "../../utils/Logger.hpp"
#include "../../utils/RandomEngineGenerator.h"

#include <ctime>
#include <string>

class HeuristicSelectiveArtificial : public Artificial
{
public:
	HeuristicSelectiveArtificial(const unsigned long long &msSelfTime, const size_t &maxNodeCount, RandomEngineGenerator &engineGenerator, FixedUnorderedMap<Board, std::shared_ptr<std::vector<Move const *>>, BoardHash> &legalCache)
		: _msSelfTime(msSelfTime),
		_maxNodeCount(maxNodeCount),
		_gameTree(GameSet(legalCache), engineGenerator.next(), FixedUnorderedMap<Board, HeuristicSelectiveGameNode*, BoardHash>(1000000), legalCache)
	{}

	Move const * getMove(const GameSet& gameSet) override
	{
		if (gameSet.currentBoard() == Board())
		{
			Logger::info("Starting\n");
		}

		const clock_t startTime = clock();

		_gameTree.playMove(gameSet);

		_gameTree.developUntil([this, &startTime]() {

			const clock_t promptedTime = clock();
			const size_t msSinceStart = (1000 * (promptedTime - startTime)) / CLOCKS_PER_SEC;

			return msSinceStart >= _msSelfTime || _gameTree.realSize() > _maxNodeCount || _gameTree.root()->isTerminal();
		});
		printDebugInfo();
		return _gameTree.playMove();
	}

private:
	const unsigned long long _msSelfTime;
	const size_t _maxNodeCount;

	HeuristicSelectiveGameTree _gameTree;

	void printDebugInfo()
	{
		Logger::info("Is white: " + std::to_string(_gameTree.root()->gameSet().isWhiteTurn()) + "\n"
			+ "Whole tree: " + std::to_string(_gameTree.treeSize()) + "\n"
			+ "Total node: " + std::to_string(_gameTree.realSize()) + "\n"
			+ "Utility : " + std::to_string(_gameTree.root()->biaisedGameScore().utility()) + "\n\n");
	}
};

#endif