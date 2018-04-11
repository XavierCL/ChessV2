#pragma once

#include "HeuristicSelectiveGameNode.h"

#include <iostream>

class HeuristicSelectiveGameTree
{
public:
	HeuristicSelectiveGameTree(const GameSet &gameSet)
		: _root(new HeuristicSelectiveGameNode(gameSet, GameScore(gameSet)))
	{}

	~HeuristicSelectiveGameTree()
	{
		_root->remove();
		delete _root;
	}

	Move const * const playMove()
	{
		HeuristicSelectiveGameNode const * const bestChild = _root->biaisedChosenOne();
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
			_root->develop();
			_root->develop();
			_root->develop();
		}
	}

	const size_t size() const
	{
		return _root->size();
	}

	HeuristicSelectiveGameNode* const root() const
	{
		return _root;
	}

private:
	void updateNewRoot(const GameSet& gameSet)
	{
		const Board& board = gameSet.currentBoard();
		if (_root->gameSet().currentBoard() == board)
		{
			_root->develop();
		}
		else
		{
			_root->develop();

			HeuristicSelectiveGameNode* newRoot(nullptr);
			for (auto * child : _root->children())
			{
				if (child->gameSet().currentBoard() == board)
				{
					newRoot = child;
				}
				else
				{
					// mark for removal and remove in a thread while waiting for the enemy's turn
					child->removeParent(_root);
					if (!child->hasParent())
					{
						delete child;
					}
				}
			}
			delete _root;
			_root = newRoot;
			_root->setRoot(gameSet);
		}
	}

	Move const * const getMoveFromNodeAndNextBoard(const HeuristicSelectiveGameNode& currentNode, const Board& nextBoard)
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

	HeuristicSelectiveGameNode* _root;
};