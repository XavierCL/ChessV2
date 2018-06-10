#pragma once

#include "ProbabilityHeuristicSelectiveGameNode.h"

#include <iostream>
#include <thread>
#include <functional>

class ProbabilityHeuristicSelectiveGameTree
{
public:
	ProbabilityHeuristicSelectiveGameTree(const GameSet &gameSet)
		: _root(new ProbabilityHeuristicSelectiveGameNode(gameSet))
	{}

	~ProbabilityHeuristicSelectiveGameTree()
	{
		_root->remove();
		delete _root;
	}

	Move const * const playMove()
	{
		ProbabilityHeuristicSelectiveGameNode const * const bestChild = _root->biaisedChosenOne();
		auto const * const bestMove = getMoveFromNodeAndNextBoard(*_root, bestChild->gameSet().currentBoard());
		updateNewRoot(_root->gameSet().playMove(*bestMove));
		return bestMove;
	}

	void playMove(const GameSet &gameSet)
	{
		updateNewRoot(gameSet);
	}

	template <typename _PredicateType>
	void developUntil(const _PredicateType& shouldStop)
	{
		while (!shouldStop() && !_root->isTerminal())
		{
			_root->developBias();
			_root->develop();
			_root->develop();
		}
	}

	template <typename _PredicateType>
	void developEnemyTurn(const _PredicateType &shouldStop)
	{
		_enemyThread = std::thread(&ProbabilityHeuristicSelectiveGameTree::developThreadEnemyTurn, this, shouldStop);
	}

	void waitForEnemyStop()
	{
		_enemyThread.join();
	}

	const size_t size() const
	{
		return _root->size();
	}

	ProbabilityHeuristicSelectiveGameNode* const root() const
	{
		return _root;
	}

private:

	void developThreadEnemyTurn(const std::function<bool()>& shouldStop)
	{
		while (!_toBeRemoved.empty())
		{
			_toBeRemoved.back()->removeParent(_root);
			if (!_toBeRemoved.back()->hasParent())
			{
				delete _toBeRemoved.back();
			}
			_toBeRemoved.pop_back();
		}
		while (!shouldStop() && !_root->isTerminal())
		{
			_root->developBias();
			_root->develop();
			_root->develop();
		}
	}

	void updateNewRoot(const GameSet& gameSet)
	{
		_root->develop();
		const Board& board = gameSet.currentBoard();
		if (_root->gameSet().currentBoard() != board)
		{
			_root->develop();

			ProbabilityHeuristicSelectiveGameNode* newRoot(nullptr);
			for (auto * child : _root->children())
			{
				if (child->gameSet().currentBoard() == board)
				{
					newRoot = child;
				}
				else
				{
					_toBeRemoved.push_back(child);
				}
			}
			delete _root;
			_root = newRoot;
			_root->setRoot();
		}
	}

	Move const * const getMoveFromNodeAndNextBoard(const ProbabilityHeuristicSelectiveGameNode& currentNode, const Board& nextBoard)
	{
		Move const * bestMove = nullptr;
		for (size_t i = 0; i < currentNode.gameSet().getLegals()->size(); ++i)
		{
			if (currentNode.gameSet().playMove(*(*currentNode.gameSet().getLegals())[i]).currentBoard() == nextBoard)
			{
				bestMove = (*currentNode.gameSet().getLegals())[i];
				break;
			}
		}
		return bestMove;
	}

	ProbabilityHeuristicSelectiveGameNode* _root;
	std::vector<ProbabilityHeuristicSelectiveGameNode*> _toBeRemoved;
	std::thread _enemyThread;
};