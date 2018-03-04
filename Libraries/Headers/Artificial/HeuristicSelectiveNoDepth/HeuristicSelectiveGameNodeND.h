#pragma once

#include "NodeGameSetND.h"

#include <iostream>
#include <random>
#include <algorithm>

class HeuristicSelectiveGameNodeND
{
public:
	HeuristicSelectiveGameNodeND(const NodeGameSetND &gameSet)
		: _gameSet(gameSet),
		_parents(),
		_isLeaf(true),
		_isTerminal(_gameSet.getStatus() != GameStatus::LIVE),
		_size(1)
	{
		_utility = estimatedUtility();
		_averageUtility = _utility;
		_nonBiaisedUtility = _utility;
	}

	void addParent(HeuristicSelectiveGameNodeND * const &parent)
	{
		_parents.push_back(parent);
	}

	const size_t hasParent() const
	{
		return _parents.size() > 0;
	}

	void removeParent(HeuristicSelectiveGameNodeND const * const &parent)
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

	const double utility() const
	{
		if (_isLeaf)
		{
			return _utility;
		}
		else
		{
			return (_utility + (_averageUtility / _children.size()) * 0.00001) / 1.00001;
		}
	}

	const double nonBiaisedUtility() const
	{
		return _nonBiaisedUtility;
	}

	void develop()
	{
		if (!_isTerminal)
		{
			if (!_isLeaf)
			{
				nonBiaisedChosenOne()->develop();
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
		_gameSet.setRoot(gameSet);
	}

	std::vector<HeuristicSelectiveGameNodeND*> children()
	{
		return _children;
	}

	const NodeGameSetND gameSet() const
	{
		return _gameSet;
	}

	HeuristicSelectiveGameNodeND* const chosenOne() const
	{
		std::vector<HeuristicSelectiveGameNodeND*> chosens;
		chosens.reserve(_children.size());
		for (auto* child : _children)
		{
			if (_utility == child->utility())
			{
				chosens.push_back(child);
			}
		}
		std::uniform_int_distribution<int> aDistributor(0, chosens.size() - 1);
		return chosens[aDistributor(*GENERATOR)];
	}

	HeuristicSelectiveGameNodeND* const nonBiaisedChosenOne() const
	{
		std::vector<HeuristicSelectiveGameNodeND*> chosens;
		chosens.reserve(_children.size());
		for (auto* child : _children)
		{
			if (_nonBiaisedUtility == child->nonBiaisedUtility() && !child->isTerminal())
			{
				chosens.push_back(child);
			}
		}
		std::uniform_int_distribution<int> aDistributor(0, chosens.size() - 1);
		return chosens[aDistributor(*GENERATOR)];
	}

	const bool isTerminal() const
	{
		return _isTerminal;
	}

	const size_t size() const
	{
		return _size;
	}

	static std::minstd_rand0* GENERATOR;
	static FixedUnorderedMap<Board, HeuristicSelectiveGameNodeND*, BoardHash> NODES;

private:

	void backPropagateUtility()
	{
		const double previousUtility = _utility;
		const double previousNonBiaisedUtility = _nonBiaisedUtility;
		const double previousAverageUtility = _averageUtility;
		gatherChildrenUtility();
		if (_utility != previousUtility
			|| _nonBiaisedUtility != previousNonBiaisedUtility
			|| _averageUtility != previousAverageUtility)
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
		for (Move const * const move : *_gameSet.getLegals())
		{
			const NodeGameSetND child(_gameSet.playMove(*move));
			HeuristicSelectiveGameNodeND* childNode(new HeuristicSelectiveGameNodeND(child));
			childNode->addParent(this);
			_children.push_back(childNode);
		}
		backPropagateSize(_gameSet.getLegals()->size());
		backPropagateUtility();
	}

	const double estimatedUtility()
	{
		if (_gameSet.getStatus() == GameStatus::FIFTY_MOVE || _gameSet.getStatus() == GameStatus::NO_LEGAL_MOVE || _gameSet.getStatus() == GameStatus::THREEFOLD_REPETITION)
			return 0;
		else if (_gameSet.getStatus() == GameStatus::WHITE_WIN)
			return 100;
		else if (_gameSet.getStatus() == GameStatus::BLACK_WIN)
			return -100;
		else
		{
			double whiteScore = 0;
			whiteScore += 1 * (double)_gameSet.currentBoard().bitBoards().populationCount(PieceType::PAWN, true);
			whiteScore += 3 * (double)_gameSet.currentBoard().bitBoards().populationCount(PieceType::KNIGHT, true);
			whiteScore += 3 * (double)_gameSet.currentBoard().bitBoards().populationCount(PieceType::BISHOP, true);
			whiteScore += 5 * (double)_gameSet.currentBoard().bitBoards().populationCount(PieceType::ROOK, true);
			whiteScore += 9 * (double)_gameSet.currentBoard().bitBoards().populationCount(PieceType::QUEEN, true);

			double blackScore = 0;
			blackScore += 1 * (double)_gameSet.currentBoard().bitBoards().populationCount(PieceType::PAWN, false);
			blackScore += 3 * (double)_gameSet.currentBoard().bitBoards().populationCount(PieceType::KNIGHT, false);
			blackScore += 3 * (double)_gameSet.currentBoard().bitBoards().populationCount(PieceType::BISHOP, false);
			blackScore += 5 * (double)_gameSet.currentBoard().bitBoards().populationCount(PieceType::ROOK, false);
			blackScore += 9 * (double)_gameSet.currentBoard().bitBoards().populationCount(PieceType::QUEEN, false);

			const double score = (whiteScore / (blackScore + 1)) - (blackScore / (whiteScore + 1));

			return score;
		}
	}

	void gatherChildrenUtility()
	{
		_isLeaf = false;
		_averageUtility = 0;
		if (_gameSet.isWhiteTurn())
		{
			double maxUtility = -101;
			double maxNonBiaisedUtility = -101;
			for (auto* &node : _children)
			{
				const double nodeUtility = node->utility();
				const double nodeNonBiaisedUtility = node->nonBiaisedUtility();
				_averageUtility += nodeUtility;
				if (nodeNonBiaisedUtility > maxNonBiaisedUtility)
				{
					_isTerminal = node->_isTerminal;
					maxNonBiaisedUtility = nodeNonBiaisedUtility;
				}
				else if (nodeNonBiaisedUtility == maxNonBiaisedUtility)
				{
					_isTerminal = _isTerminal && node->_isTerminal;
				}
				if (nodeUtility > maxUtility)
				{
					maxUtility = nodeUtility;
				}
			}
			_utility = maxUtility;
			_nonBiaisedUtility = maxNonBiaisedUtility;
		}
		else
		{
			double minUtility = 101;
			double minNonBiaisedUtility = 101;
			for (auto* &node : _children)
			{
				const double nodeUtility = node->utility();
				const double nodeNonBiaisedUtility = node->nonBiaisedUtility();
				_averageUtility += nodeUtility;
				if (nodeNonBiaisedUtility < minNonBiaisedUtility)
				{
					_isTerminal = node->_isTerminal;
					minNonBiaisedUtility = nodeNonBiaisedUtility;
				}
				else if (nodeNonBiaisedUtility == minNonBiaisedUtility)
				{
					_isTerminal = _isTerminal && node->_isTerminal;
				}
				if (nodeUtility < minUtility)
				{
					minUtility = nodeUtility;
				}
			}
			_utility = minUtility;
			_nonBiaisedUtility = minNonBiaisedUtility;
		}
	}

	double _utility;
	double _nonBiaisedUtility;
	double _averageUtility;

	NodeGameSetND _gameSet;

	bool _isLeaf;
	bool _isTerminal;
	size_t _size;
	std::vector<HeuristicSelectiveGameNodeND*> _parents;
	std::vector<HeuristicSelectiveGameNodeND*> _children;
};