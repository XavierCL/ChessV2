#pragma once

#include "HeuristicSelectiveGameNode.h"

#include <iostream>

class HeuristicSelectiveGameTree
{
public:
	HeuristicSelectiveGameTree(const GameSet &gameSet)
		: _root(new HeuristicSelectiveGameNode(NodeGameSet(gameSet)))
	{}

	~HeuristicSelectiveGameTree()
	{
		std::cout << "Ratio : " << 1 / averageRatio << "\n";
		_root->remove();
		delete _root;
	}

	Move const * const playMove()
	{
		HeuristicSelectiveGameNode const * const bestChild = _root->chosenOne();
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

	double averageRatio = 0;
	size_t averageCount = 0;

private:
	void updateNewRoot(const GameSet& gameSet)
	{
		const Board& board = gameSet.currentBoard();
		if (_root->gameSet().currentBoard() == board)
		{
			std::cout << "Starting!" << "\n\n";
			_root->develop();
			return;
		}
		else
		{
			_root->develop();
			//cout << "Is white: " << _root->gameSet().isWhiteTurn() << "\n";
			//cout << "Whole tree: " << _root->size() << "\n";
			//cout << "Child count: " << _root->children().size() << "\n";
			HeuristicSelectiveGameNode* newRoot(nullptr);
			for (auto * child : _root->children())
			{
				if (child->gameSet().currentBoard() == board)
				{
					//cout << "Selected tree: " << child->size() << "\n";
					//cout << "Relative size: " << (child->size() * _root->children().size()) / (double)_root->size() << "\n";
					averageRatio = (averageRatio * averageCount + (double)_root->size() / (child->size() * _root->children().size())) / (averageCount + 1);
					++averageCount;
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
			//cout << "Utility: " << _root->utility() << "\n\n";
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