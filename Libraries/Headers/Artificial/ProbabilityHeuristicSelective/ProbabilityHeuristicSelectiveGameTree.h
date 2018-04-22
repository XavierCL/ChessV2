#pragma once

#include "ProbabilityHeuristicSelectiveGameNode.h"

#include <iostream>
#include <thread>

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

	template<typename _PredicateType>
	void developChildren(const std::vector<ProbabilityHeuristicSelectiveGameNode*> &children, const _PredicateType& shouldStop)
	{
		ProbabilityHeuristicSelectiveGameNode newRoot(_root->gameSet(), children);
		while (!shouldStop() && !newRoot.isTerminal())
		{
			newRoot.developBias();
			newRoot.develop();
		}
	}

	template <typename _PredicateType>
	void developUntil(const _PredicateType& shouldStop, const size_t &threadCount)
	{
		std::vector<ProbabilityHeuristicSelectiveGameNode*> newChildren;
		double totalProbability = 0;
		size_t nonTerminalChildrenCount = 0;
		for (auto const * const & child : _root->children())
		{
			if (!child->isTerminal())
			{
				totalScore += child->gameScore().probability()
					++nonTerminalChildrenCount;
			}
		}
		double probabilityPerThread = totalProbability / threadCount;
		size_t currentChildIndex = 0;
		while (currentChildIndex < _root->children().size() && _root->children()[currentChildIndex]->isTerminal())
		{
			++currentChildIndex;
		}
		_developmentThreads.clear();
		for (size_t currentThreadCount = 0; currentThreadCount < threadCount && currentChildIndex < _root->children.size(); ++currentThreadCount)
		{
			std::vector<ProbabilityHeuristicSelectiveGameNode*> threadChildren(1, _root->children()[currentChildIndex]);
			double currentProbability = _root->children()[currentChildIndex]->gameScore().probability();
			++currentChildIndex;
			while (currentChildIndex < _root->children().size() && _root->children()[currentChildIndex]->isTerminal())
			{
				++currentChildIndex;
			}
			while (currentChildIndex < _root->children().size()
				&& FastMath::absDiff(currentProbability + _root->children()[currentChildIndex]->gameScore().probability() + 1, probabilityPerThread)
			> FastMath::absDiff(currentProbability, currentProbability))
			{
				threadChildren.push_back(_root->children()[currentChildIndex]);
				currentScore += _root->children()[currentChildIndex]->gameScore().probability();
				++currentChildIndex;
				while (currentChildIndex < _root->children().size() && _root->children()[currentChildIndex]->isTerminal())
				{
					++currentChildIndex;
				}
			}
			_developmentThreads.emplace_back(ProbabilityHeuristicSelectiveGameTree::developChildren, threadChildren, shouldStop);
		}
	}

	void waitForDeveloppementEnd()
	{
		for (auto &t : _developmentThreads)
		{
			t.join();
		}
		_developmentThreads.clear();
		auto rootGameSet = _root->gameSet();
		auto rootChildren = _root->children();
		delete _root;
		_root = new ProbabilityHeuristicSelectiveGameNode(rootGameSet, rootChildren);
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
	std::vector<std::thread> _developmentThreads;
};