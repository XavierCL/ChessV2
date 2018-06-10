#pragma once

#include "HeuristicSelectiveGameNode.h"

#include <thread>

class HeuristicSelectiveGameTree
{
public:
	HeuristicSelectiveGameTree(const GameSet &gameSet, std::minstd_rand0 randomGenerator, FixedUnorderedMap<Board, HeuristicSelectiveGameNode*, BoardHash> nodeRepository, FixedUnorderedMap<Board, std::shared_ptr<std::vector<Move const *>>, BoardHash> &legalCache)
		: _root(new HeuristicSelectiveGameNode(gameSet)),
		_realNodeCount(1),
		_thinkingThread(nullptr),
		_destructing(false),
		_randomGenerator(randomGenerator),
		_nodeRepository(nodeRepository),
		_legalCache(legalCache)
	{}

	~HeuristicSelectiveGameTree()
	{
		_destructing = true;
		waitForThinkingDone();
		_root->removeRecursive(_nodeRepository, _deleter);
		_deleter.deleteAll();
	}

	Move const * const playMove()
	{
		develop();
		HeuristicSelectiveGameNode const * const bestChild = _root->biaisedChosenOne(_randomGenerator);
		auto const * const bestMove = getMoveFromNodeAndNextBoard(*_root, bestChild->gameSet().currentBoard());
		updateNewRoot(bestChild->gameSet());
		return bestMove;
	}

	void playMove(const GameSet &gameSet)
	{
		updateNewRoot(gameSet);
	}

	const size_t treeSize() const
	{
		return _root->size();
	}

	const size_t realSize() const
	{
		return _realNodeCount;
	}

	HeuristicSelectiveGameNode* const root() const
	{
		return _root;
	}

	template <typename PredicateType>
	void thinkUntil(const PredicateType &shouldStop)
	{
		_thinkingThread = new std::thread(&HeuristicSelectiveGameTree::developUntil<PredicateType>, this, shouldStop);
	}

	void waitForThinkingDone()
	{
		std::lock_guard<std::mutex> lock(_destructionMutex);
		if (_thinkingThread)
		{
			_thinkingThread->join();
			delete _thinkingThread;
			_thinkingThread = nullptr;
		}
	}

private:

	template <typename _PredicateType>
	void developUntil(const _PredicateType &shouldStop)
	{
		while (!shouldStop() && !_destructing)
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
			HeuristicSelectiveGameNode* newRoot(nullptr);
			for (auto * child : _root->children())
			{
				child->removeParent(_root);
				if (child->gameSet().currentBoard() == board)
				{
					newRoot = child;
				}
				else if(!child->hasParent())
				{
					_realNodeCount -= child->removeRecursive(_nodeRepository, _deleter);
				}
			}

			_realNodeCount -= _root->removeSingle(_nodeRepository, _deleter);
			_root = newRoot;

			_deleter.deleteAll();
		}
	}

	Move const * const getMoveFromNodeAndNextBoard(const HeuristicSelectiveGameNode& currentNode, const Board& nextBoard)
	{
		Move const * bestMove = nullptr;
		for (const auto*& move : *currentNode.gameSet().getLegals())
		{
			if (move->play(currentNode.gameSet().currentBoard()) == nextBoard)
			{
				bestMove = move;
				break;
			}
		}
		return bestMove;
	}

	HeuristicSelectiveGameNode* _root;

	size_t _realNodeCount;
	std::thread* _thinkingThread;
	bool _destructing;
	std::mutex _destructionMutex;
	Deleter<HeuristicSelectiveGameNode> _deleter;
	std::minstd_rand0 _randomGenerator;
	FixedUnorderedMap<Board, HeuristicSelectiveGameNode*, BoardHash> _nodeRepository;
	FixedUnorderedMap<Board, std::shared_ptr<std::vector<Move const *>>, BoardHash> _legalCache;
};