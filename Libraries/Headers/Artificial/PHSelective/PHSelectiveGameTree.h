#pragma once

#include "PHSelectiveGameNode.h"

#include <thread>
#include <atomic>

class PHSelectiveGameTree
{
public:
	PHSelectiveGameTree(const GameSet &gameSet, const size_t maxNodeCount, std::minstd_rand0 randomGenerator, FixedUnorderedMap<Board, std::shared_ptr<PHSelectiveGameNode>, BoardHash> nodeRepository, FixedUnorderedMap<Board, std::shared_ptr<std::vector<Move const *>>, BoardHash> &legalCache)
		: _maxNodeCount(maxNodeCount),
		_root(new PHSelectiveGameNode(gameSet)),
		_realNodeCount(1),
		_thinkingThread(nullptr),
		_interrupted(false),
		_randomGenerator(randomGenerator),
		_nodeRepository(nodeRepository),
		_legalCache(legalCache)
	{}

	~PHSelectiveGameTree()
	{
		interruptThinking();
		waitForThinkingDone();
	}

	Move const * const playMove()
	{
		interruptThinking();
		waitForThinkingDone();

		// We don't want no thinking at all to occur due to thread fussyness, forcing it here
		develop();

		const auto bestNode(_root->biaisedChosenOne(_randomGenerator));
		const GameSet bestGameSet(bestNode->gameSet());
		const auto bestMove(getMoveFromNodeAndNextBoard(*_root, bestGameSet.currentBoard()));

		updateNewRoot(bestGameSet);

		return bestMove;
	}

	void playMove(const GameSet &gameSet)
	{
		interruptThinking();
		waitForThinkingDone();

		updateNewRoot(gameSet);
	}

	std::shared_ptr<PHSelectiveGameNode> const root() const
	{
		return _root;
	}

	template <typename PredicateType>
	void thinkUntil(const PredicateType &shouldStop)
	{
		_interrupted = false;
		_thinkingThread = std::make_unique<std::thread>(&PHSelectiveGameTree::developUntil<PredicateType>, this, shouldStop);
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
		while (!shouldStop() && !_interrupted && !_root->isTerminal() && _maxNodeCount >= _realNodeCount)
		{
			develop();
		}
	}

	void develop()
	{
		_realNodeCount += _root->develop(_randomGenerator, _nodeRepository, _legalCache);
		_realNodeCount += _root->develop(_randomGenerator, _nodeRepository, _legalCache);
		_realNodeCount += _root->develop(_randomGenerator, _nodeRepository, _legalCache);
		_realNodeCount += _root->develop(_randomGenerator, _nodeRepository, _legalCache);
		_realNodeCount += _root->developBiaised(_randomGenerator, _nodeRepository, _legalCache);
	}

	void updateNewRoot(const GameSet& gameSet)
	{
		// We don't want no thinking at all to occur due to thread fussyness, forcing it here
		develop();

		const Board& board = gameSet.currentBoard();
		if (_root->gameSet().currentBoard() != board)
		{
			const auto nextRoot = _root->findChild(board);

			_realNodeCount -= _root->removeAllBut(nextRoot, _nodeRepository);
			_root = nextRoot;
			_root->makeRoot();
		}
	}

	Move const * const getMoveFromNodeAndNextBoard(const PHSelectiveGameNode& currentNode, const Board& nextBoard)
	{
		for (const auto &move : *currentNode.gameSet().getLegals())
		{
			if (move->play(currentNode.gameSet().currentBoard()) == nextBoard)
			{
				return move;
			}
		}
		throw std::exception("No move produced the given next board");
	}

	const size_t _maxNodeCount;

	std::shared_ptr<PHSelectiveGameNode> _root;

	size_t _realNodeCount;

	std::atomic<bool> _interrupted;
	std::unique_ptr<std::thread> _thinkingThread;

	FixedUnorderedMap<Board, std::shared_ptr<PHSelectiveGameNode>, BoardHash> _nodeRepository;
	FixedUnorderedMap<Board, std::shared_ptr<std::vector<Move const *>>, BoardHash> _legalCache;

	std::minstd_rand0 _randomGenerator;
};
