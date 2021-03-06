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

	HeuristicSelectiveArtificial(const unsigned long long &msSelfTime, const size_t &maxNodeCount, std::minstd_rand0 &engine, FixedUnorderedMap<Board, std::shared_ptr<HeuristicSelectiveGameNode>, BoardHash> &nodeRepository, FixedUnorderedMap<Board, std::shared_ptr<std::vector<Move const *>>, BoardHash> &legalCache)
		: _msSelfTime(msSelfTime),
		_gameTree(GameSet(legalCache), maxNodeCount, engine, nodeRepository, legalCache)
	{
		printDebugInfo("Constructor");

		_gameTree.thinkUntil([] { return false; });
	}

	virtual ~HeuristicSelectiveArtificial()
	{
		_gameTree.interruptThinking();
		_gameTree.waitForThinkingDone();

		printDebugInfo("Destructor");
	}

	Move const * getMove(const GameSet& gameSet) override
	{
		if (gameSet.currentBoard() == Board())
		{
			Logger::info("Starting\n");
		}

		_gameTree.interruptThinking();
		_gameTree.waitForThinkingDone();
		printDebugInfo("Thought during enemy turn");

		_gameTree.playMove(gameSet);
		printDebugInfo("Played enemy move");

		// The clock is this far in the play because else the AI could miss branch early and the game wouldn't be fun
		const clock_t startTime = clock();

		_gameTree.thinkUntil([this, &startTime]() {

			const clock_t promptedTime = clock();
			const size_t msSinceStart = (1000 * (promptedTime - startTime)) / CLOCKS_PER_SEC;

			return msSinceStart >= _msSelfTime;
		});

		_gameTree.waitForThinkingDone();
		printDebugInfo("Thought during my turn");

		auto selectedMove(_gameTree.playMove());
		printDebugInfo("Played my move");

		_gameTree.thinkUntil([] { return false; });

		return selectedMove;
	}

private:
	const unsigned long long _msSelfTime;

	HeuristicSelectiveGameTree _gameTree;

	void printDebugInfo(const std::string title)
	{
		Logger::info(std::to_string((size_t)this) + ": " + title + "\n"
			+ "Is white: " + std::to_string(_gameTree.root()->gameSet().isWhiteTurn()) + "\n"
			+ "Utility : " + std::to_string(_gameTree.root()->biaisedGameScoreUtility()) + "\n"
			+ "Checked node count : " + std::to_string(_gameTree.root()->size()) + "\n"
			+ "Real node count : " + std::to_string(_gameTree.realNodeCount()) + "\n\n");
	}
};

#endif
