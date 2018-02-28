#pragma once

#include "../../Game/GameSetHash.h"

#include "../../utils/Option.hpp"

#include <cmath>
#include <random>

class HeuristicSelectiveGameNode
{
public:
	HeuristicSelectiveGameNode(const GameSet &gameSet, Move const * const &move, HeuristicSelectiveGameNode * const &parent)
		: _gameSet(gameSet),
		_move(move),
		_parent(parent),
		_isLeaf(true),
		_isTerminal(_gameSet.getStatus() != GameStatus::LIVE),
		_size(1)
	{
		_utility = estimatedUtility();
		_averageUtility = _utility;
		_nonBiaisedUtility = _utility;
	}

	void remove()
	{
		for (auto* child : _children)
		{
			child->remove();
			delete child;
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
				const double oldUtility = utility();
				const double oldNonBiaisedUtility = nonBiaisedUtility();
				developDepth2();
				if (_parent)
				{
					_parent->backPropagateUtility(this, oldUtility);
					_parent->backPropagateNonBiaisedUtility(this, oldNonBiaisedUtility);
				}
			}
		}
	}

	void setRoot()
	{
		_parent = nullptr;
	}

	std::vector<HeuristicSelectiveGameNode*> children()
	{
		return _children;
	}

	const GameSet& gameSet() const
	{
		return _gameSet;
	}

	Move const * const move() const
	{
		return _move;
	}

	HeuristicSelectiveGameNode * const chosenOne() const
	{
		std::vector<HeuristicSelectiveGameNode*> chosens;
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

	HeuristicSelectiveGameNode * const nonBiaisedChosenOne() const
	{
		std::vector<HeuristicSelectiveGameNode*> chosens;
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

	void developDepth1()
	{
		if (!_isTerminal)
		{
			_children.reserve(_gameSet.getLegals()->size());
			for (Move const * const move : *_gameSet.getLegals())
			{
				const GameSet child(_gameSet.playMove(*move));
				HeuristicSelectiveGameNode* childNode(new HeuristicSelectiveGameNode(child, move, this));
				_children.push_back(childNode);
			}
			gatherChildrenUtility();
			_size += _gameSet.getLegals()->size();
		}
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

private:

	void backPropagateUtility(HeuristicSelectiveGameNode* updatedChild, const double &oldUtility)
	{
		if (updatedChild->utility() != oldUtility)
		{
			const double previousUtility = utility();
			gatherChildrenUtility();
			if (_parent)
			{
				_parent->backPropagateUtility(this, previousUtility);
			}
		}
	}

	void backPropagateNonBiaisedUtility(HeuristicSelectiveGameNode* updatedChild, const double &oldUtility)
	{
		if (updatedChild->nonBiaisedUtility() != oldUtility)
		{
			const double previousUtility = utility();
			gatherChildrenUtility();
			if (_parent)
			{
				_parent->backPropagateNonBiaisedUtility(this, previousUtility);
			}
		}
	}

	void backPropagateSize(const size_t addedSize)
	{
		_size += addedSize;
		if (_parent)
		{
			_parent->backPropagateSize(addedSize);
		}
	}

	void developDepth2()
	{
		size_t addedSize = 0;
		_children.reserve(_gameSet.getLegals()->size());
		for (Move const * const move : *_gameSet.getLegals())
		{
			const GameSet child(_gameSet.playMove(*move));
			HeuristicSelectiveGameNode* childNode(new HeuristicSelectiveGameNode(child, move, this));
			childNode->developDepth1();
			addedSize += childNode->_size;
			_children.push_back(childNode);
		}
		gatherChildrenUtility();
		backPropagateSize(addedSize);
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
			if (maxNonBiaisedUtility == -101)
			{
				int temp1 = 0;
			}
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
			if (minNonBiaisedUtility == -101)
			{
				int temp1 = 0;
			}
			_nonBiaisedUtility = minNonBiaisedUtility;
		}
	}

	double _utility;
	double _nonBiaisedUtility;
	double _averageUtility;

	// LikelyHood would depend on root depth - parent depth,
	//but since a node can have multiple parents i'll just use the utility for the likelyHood too.
	//double _likelyHood;

	const GameSet _gameSet;
	Move const * const _move;

	bool _isLeaf;
	bool _isTerminal;
	size_t _size;
	HeuristicSelectiveGameNode* _parent;
	std::vector<HeuristicSelectiveGameNode*> _children;
};