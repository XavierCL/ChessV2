#pragma once

#include "HeuristicSelectiveGameNode.h"

#include <iostream>

class HeuristicSelectiveGameTree
{
public:
	HeuristicSelectiveGameTree(const GameSet &gameSet)
		: _root(new HeuristicSelectiveGameNode(gameSet, nullptr, nullptr))
	{}

	~HeuristicSelectiveGameTree()
	{
		_root->remove();
		delete _root;
		std::cout << "Ratio : " << averageRatio << "\n";
	}

	Move const * const playMove()
	{
		HeuristicSelectiveGameNode const * const bestChild = _root->chosenOne();
		updateNewRoot([&bestChild](const HeuristicSelectiveGameNode * child) {
			return child == bestChild;
		});
		return _root->move();
	}

	void playMove(const GameSet &gameSet)
	{
		updateNewRoot([&gameSet](const HeuristicSelectiveGameNode * child) {
			return child->gameSet() == gameSet;
		});
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
	template <typename _PredicateType>
	void updateNewRoot(const _PredicateType& newRootPredicate)
	{
		if (newRootPredicate(_root))
		{
			//cout << "Starting!" << "\n\n";
			_root->developDepth1();
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
				if (newRootPredicate(child))
				{
					//cout << "Selected tree: " << child->size() << "\n";
					//cout << "Relative size: " << (child->size() * _root->children().size()) / (double)_root->size() << "\n\n";
					averageRatio = (averageRatio * averageCount + sqrt((child->size() * _root->children().size()) / (double)_root->size())) / (averageCount + 1);
					++averageCount;
					newRoot = child;
				}
				else
				{
					// mark for removal and remove in a thread while waiting for the enemy's turn
					child->remove();
					delete child;
				}
			}
			delete _root;
			_root = newRoot;
			_root->setRoot();
		}
	}

	HeuristicSelectiveGameNode* _root;
};