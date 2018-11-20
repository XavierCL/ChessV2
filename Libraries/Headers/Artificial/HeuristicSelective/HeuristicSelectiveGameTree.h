#pragma once

#include "HeuristicSelectiveGameNode.h"

#include <thread>
#include <atomic>

class HeuristicSelectiveGameTree
{
public:
	HeuristicSelectiveGameTree(const GameSet &gameSet, const size_t maxNodeCount, std::minstd_rand0 randomGenerator, FixedUnorderedMap<Board, std::shared_ptr<HeuristicSelectiveGameNode>, BoardHash> nodeRepository, FixedUnorderedMap<Board, std::shared_ptr<std::vector<Move const *>>, BoardHash> &legalCache)
		: _maxNodeCount(maxNodeCount),
		_root(new HeuristicSelectiveGameNode(gameSet)),
		_realNodeCount(1),
		_thinkingThread(nullptr),
		_interrupted(false),
		_randomGenerator(randomGenerator),
		_nodeRepository(nodeRepository),
		_legalCache(legalCache)
	{}

	~HeuristicSelectiveGameTree()
	{
		interruptThinking();
		waitForThinkingDone();
	}

	Move const * const playMove()
	{
		interruptThinking();
		waitForThinkingDone();

		// We don't want no thinking at all to occur due to thread fussyness, forcing it here:
		develop();

		const std::shared_ptr<HeuristicSelectiveGameNode> bestChild = _root->biaisedChosenOne(_randomGenerator);
		auto const * const bestMove = getMoveFromNodeAndNextBoard(*_root, bestChild->gameSet().currentBoard());

		_root = bestChild;

		return bestMove;
	}

	void playMove(const GameSet &gameSet)
	{
		interruptThinking();
		waitForThinkingDone();

		updateNewRoot(gameSet);
	}

	std::shared_ptr<HeuristicSelectiveGameNode> const root() const
	{
		return _root;
	}

	template <typename PredicateType>
	void thinkUntil(const PredicateType &shouldStop)
	{
		_interrupted = false;
		_thinkingThread = std::make_unique<std::thread>(&HeuristicSelectiveGameTree::developUntil<PredicateType>, this, shouldStop);
	}

	void waitForThinkingDone()
	{
		if (_thinkingThread)
		{
			_thinkingThread->join();
			_thinkingThread.reset();
		}
	}

	void interruptThinking()
	{
		_interrupted = true;
	}

	size_t realNodeCount()
	{
		return _realNodeCount;
	}

private:

	template <typename _PredicateType>
	void developUntil(const _PredicateType &shouldStop)
	{
		while (!shouldStop() && !_interrupted && !_root->isTerminal())
		{
			develop();
		}
	}

	void develop()
	{
		_realNodeCount += _root->develop(_randomGenerator, _nodeRepository, _legalCache);
	}

	void updateNewRoot(const GameSet& gameSet)
	{
		develop();

		const Board& board = gameSet.currentBoard();
		if (_root->gameSet().currentBoard() != board)
		{
			_root = _root->findChild(board);
		}

		develop();
	}

	Move const * const getMoveFromNodeAndNextBoard(const HeuristicSelectiveGameNode& currentNode, const Board& nextBoard)
	{
		for (const auto*& move : *currentNode.gameSet().getLegals())
		{
			if (move->play(currentNode.gameSet().currentBoard()) == nextBoard)
			{
				return move;
			}
		}
		throw std::exception("No move produced the given next board");
	}

	const size_t _maxNodeCount;

	std::shared_ptr<HeuristicSelectiveGameNode> _root;

	size_t _realNodeCount;

	std::atomic<bool> _interrupted;
	std::unique_ptr<std::thread> _thinkingThread;

	FixedUnorderedMap<Board, std::shared_ptr<HeuristicSelectiveGameNode>, BoardHash> _nodeRepository;
	FixedUnorderedMap<Board, std::shared_ptr<std::vector<Move const *>>, BoardHash> _legalCache;

	std::minstd_rand0 _randomGenerator;
};