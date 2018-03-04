#pragma once

#include "HeuristicSelectiveGameNodeND.h"

#include <iostream>

class HeuristicSelectiveGameTreeND
{
public:
	HeuristicSelectiveGameTreeND(const GameSet &gameSet)
		: _root(new HeuristicSelectiveGameNodeND(NodeGameSetND(gameSet)))
	{}

	~HeuristicSelectiveGameTreeND()
	{
		_root->remove();
		delete _root;
	}

	Move const * const playMove()
	{
		HeuristicSelectiveGameNodeND const * const bestChild = _root->chosenOne();
		auto const * const bestMove = getMoveFromNodeAndNextBoard(*_root, bestChild->gameSet().currentBoard());
		updateNewRoot(_root->gameSet().rootGameSet().playMove(*bestMove));
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
		}
	}

	const size_t size() const
	{
		return _root->size();
	}

	HeuristicSelectiveGameNodeND* const root() const
	{
		return _root;
	}

	// DEBUG
	double averageRatio = 0;
	size_t averageCount = 0;
	// END DEBUG

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

			HeuristicSelectiveGameNodeND* newRoot(nullptr);
			for (auto * child : _root->children())
			{
				if (child->gameSet().currentBoard() == board)
				{
					// DEBUG
					averageRatio = (averageRatio * averageCount + (double)_root->size() / (child->size() * _root->children().size())) / (averageCount + 1);
					++averageCount;
					// END DEBUG

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

	Move const * const getMoveFromNodeAndNextBoard(const HeuristicSelectiveGameNodeND& currentNode, const Board& nextBoard)
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

	HeuristicSelectiveGameNodeND* _root;
};