#pragma once

#include "../../Game/GameSet.h"
#include "ProbabilityGameScore.h"

#include <iostream>
#include <random>
#include <algorithm>

class ProbabilityHeuristicSelectiveGameNode
{
public:

	ProbabilityHeuristicSelectiveGameNode(const GameSet &gameSet)
		: _gameSet(gameSet),
		_parents(),
		_isLeaf(true),
		_size(1),
		_gameScore(gameSet)
	{}

	ProbabilityHeuristicSelectiveGameNode(const GameSet &gameSet, const ProbabilityGameScore &parentGameScore)
		: _gameSet(gameSet),
		_parents(),
		_isLeaf(true),
		_size(1),
		_gameScore(gameSet, parentGameScore)
	{}

	~ProbabilityHeuristicSelectiveGameNode()
	{
		NODES.get(gameSet().currentBoard()).filter([this](ProbabilityHeuristicSelectiveGameNode * const foundNode) {
			return this == foundNode;
		}).foreach([this](ProbabilityHeuristicSelectiveGameNode * const foundNode) {
			NODES.remove(gameSet().currentBoard());
		});
	}

	void addParent(ProbabilityHeuristicSelectiveGameNode * const &parent)
	{
		_parents.push_back(parent);
	}

	const size_t hasParent() const
	{
		return _parents.size() > 0;
	}

	void removeParent(ProbabilityHeuristicSelectiveGameNode const * const &parent)
	{
		_parents.erase(std::remove(_parents.begin(), _parents.end(), parent), _parents.end());
		if (!hasParent())
		{
			remove();
		}
	}

	void remove()
	{
		for (auto * child : _children)
		{
			child->removeParent(this);
			if (!child->hasParent())
			{
				delete child;
			}
		}
	}

	void develop()
	{
		if (!isTerminal())
		{
			if (!_isLeaf)
			{
				chosenOne()->develop();
			}
			else
			{
				developDepth1();
			}
		}
	}

	void setRoot(const GameSet& gameSet)
	{
		_parents.resize(0);
	}

	std::vector<ProbabilityHeuristicSelectiveGameNode*> children()
	{
		return _children;
	}

	const GameSet gameSet() const
	{
		return _gameSet;
	}

	ProbabilityHeuristicSelectiveGameNode * const biaisedChosenOne() const
	{
		std::vector<ProbabilityHeuristicSelectiveGameNode*> chosens;
		chosens.reserve(_children.size());
		chosens.push_back(_children[0]);
		if (_gameSet.isWhiteTurn())
		{
			for (size_t childCounter = 1; childCounter < _children.size(); ++childCounter)
			{
				auto* const child = _children[childCounter];
				if (child->gameScore().whiteWinsOver(chosens[0]->gameScore()))
				{
					chosens.resize(1);
					chosens[0] = child;
				}
				else if (!child->gameScore().blackWinsOver(chosens[0]->gameScore()))
				{
					chosens.push_back(child);
				}
			}
		}
		else
		{
			for (size_t childCounter = 1; childCounter < _children.size(); ++childCounter)
			{
				auto* const child = _children[childCounter];
				if (child->gameScore().blackWinsOver(chosens[0]->gameScore()))
				{
					chosens.resize(1);
					chosens[0] = child;
				}
				else if (!child->gameScore().whiteWinsOver(chosens[0]->gameScore()))
				{
					chosens.push_back(child);
				}
			}
		}
		std::uniform_int_distribution<int> aDistributor(0, chosens.size() - 1);
		return chosens[aDistributor(*GENERATOR)];
	}

	ProbabilityHeuristicSelectiveGameNode * const chosenOne() const
	{
		std::vector<ProbabilityHeuristicSelectiveGameNode*> chosens;
		size_t childCounter;
		for (childCounter = 0; childCounter < _children.size(); ++childCounter)
		{
			if (!_children[childCounter]->isTerminal())
			{
				break;
			}
		}
		chosens.reserve(_children.size() - childCounter);
		chosens.push_back(_children[childCounter]);
		if (_gameSet.isWhiteTurn())
		{
			for (; childCounter < _children.size(); ++childCounter)
			{
				auto* child = _children[childCounter];
				if (!child->isTerminal())
				{
					if(child->gameScore().betterWhiteProbability(chosens[0]->gameScore()))
					{
						chosens.resize(1);
						chosens[0] = child;
					}
					else if (!child->gameScore().betterBlackProbability(chosens[0]->gameScore()))
					{
						chosens.push_back(child);
					}
				}
			}
		}
		else
		{
			for (; childCounter < _children.size(); ++childCounter)
			{
				auto* child = _children[childCounter];
				if (!child->isTerminal())
				{
					if (child->gameScore().betterBlackProbability(chosens[0]->gameScore()))
					{
						chosens.resize(1);
						chosens[0] = child;
					}
					else if (!child->gameScore().betterWhiteProbability(chosens[0]->gameScore()))
					{
						chosens.push_back(child);
					}
				}
			}
		}
		std::uniform_int_distribution<int> aDistributor(0, chosens.size() - 1);
		return chosens[aDistributor(*GENERATOR)];
	}

	const bool isTerminal() const
	{
		return _gameScore.terminal();
	}

	const size_t size() const
	{
		return _size;
	}

	const ProbabilityGameScore gameScore() const
	{
		return _gameScore;
	}

	static std::minstd_rand0* GENERATOR;
	static FixedUnorderedMap<Board, ProbabilityHeuristicSelectiveGameNode*, BoardHash> NODES;

private:

	void backPropagateUtility()
	{
		const ProbabilityGameScore previousGameScore = _gameScore;
		gatherChildrenUtility();
		if (!_gameScore.isSameScore(previousGameScore))
		{
			for (auto* parent : _parents)
			{
				parent->backPropagateUtility();
			}
		}
	}

	void backPropagateSize(const size_t addedSize)
	{
		_size += addedSize;
		for (auto* parent : _parents)
		{
			parent->backPropagateSize(addedSize);
		}
	}

	void developDepth1()
	{
		_children.reserve(_gameSet.getLegals()->size());
		size_t addedSize = 0;
		for (Move const * const move : *_gameSet.getLegals())
		{
			const GameSet child(_gameSet.playMove(*move));
			ProbabilityHeuristicSelectiveGameNode* childNode(NODES.get(child.currentBoard()).filter([&child](ProbabilityHeuristicSelectiveGameNode* const foundNode) {
				return foundNode->gameSet() == child;
			}).getOrElse([&child, this]() {
				auto* node = new ProbabilityHeuristicSelectiveGameNode(child, _gameScore);
				NODES.set(child.currentBoard(), node);
				return node;
			}));
			childNode->addParent(this);
			_children.push_back(childNode);
			addedSize += childNode->size();
		}
		backPropagateSize(addedSize);
		backPropagateUtility();
	}

	void gatherChildrenUtility()
	{
		_gameScore.gatherChildren(_children);
		_isLeaf = false;
	}

	GameSet _gameSet;
	ProbabilityGameScore _gameScore;

	bool _isLeaf;
	size_t _size;
	std::vector<ProbabilityHeuristicSelectiveGameNode*> _parents;
	std::vector<ProbabilityHeuristicSelectiveGameNode*> _children;
};