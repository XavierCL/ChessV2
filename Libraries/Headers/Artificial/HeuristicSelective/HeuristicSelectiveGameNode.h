#pragma once

#include "../../Game/GameSet.h"
#include "GameScore.h"
#include "BiaisedGameScore.h"

#include <iostream>
#include <random>
#include <algorithm>

class HeuristicSelectiveGameNode
{
public:
	HeuristicSelectiveGameNode(const GameSet &gameSet, const GameScore &parentGameScore, const BiaisedGameScore &parentBiaisedGameScore)
		: _gameSet(gameSet),
		_parents(),
		_isLeaf(true),
		_isTerminal(gameSet.getStatus() != GameStatus::LIVE),
		_size(1),
		_gameScore(GameScore(estimateScore(gameSet), parentGameScore)),
		_biaisedGameScore(BiaisedGameScore(estimateScore(gameSet), parentBiaisedGameScore))
	{}

	void addParent(HeuristicSelectiveGameNode * const &parent)
	{
		_parents.push_back(parent);
	}

	const size_t hasParent() const
	{
		return _parents.size() > 0;
	}

	void removeParent(HeuristicSelectiveGameNode const * const &parent)
	{
		_parents.erase(std::remove(_parents.begin(), _parents.end(), parent), _parents.end());
		if (!hasParent())
		{
			remove();
		}
	}

	void remove()
	{
		NODES.get(gameSet().currentBoard()).filter([this](HeuristicSelectiveGameNode * const foundNode) {
			return this == foundNode;
		}).foreach([this](HeuristicSelectiveGameNode * const foundNode) {
			NODES.remove(gameSet().currentBoard());
		});
		for (auto * child : _children)
		{
			child->removeParent(this);
			if (!child->hasParent())
			{
				delete child;
			}
		}
	}

	const BiaisedGameScore biaisedGameScore() const
	{
		return _biaisedGameScore;
	}

	void develop()
	{
		if (!_isTerminal)
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

	std::vector<HeuristicSelectiveGameNode*> children()
	{
		return _children;
	}

	const GameSet gameSet() const
	{
		return _gameSet;
	}

	HeuristicSelectiveGameNode * const biaisedChosenOne() const
	{
		std::vector<HeuristicSelectiveGameNode*> chosens;
		chosens.reserve(_children.size());
		BiaisedGameScore bestBiaisedGameScore(_children[0]->_biaisedGameScore);
		chosens.push_back(_children[0]);
		if (_gameSet.isWhiteTurn())
		{
			for (size_t i = 1; i < _children.size(); ++i)
			{
				auto* const child = _children[i];
				if (child->_biaisedGameScore.biaisedWhiteWinsOver(bestBiaisedGameScore))
				{
					chosens.clear();
					chosens.push_back(child);
					bestBiaisedGameScore = child->_biaisedGameScore;
				}
				else if (!child->_biaisedGameScore.biaisedBlackWinsOver(bestBiaisedGameScore))
				{
					chosens.push_back(child);
				}
			}
		}
		else
		{
			for (size_t i = 1; i < _children.size(); ++i)
			{
				auto* const child = _children[i];
				if (child->_biaisedGameScore.biaisedBlackWinsOver(bestBiaisedGameScore))
				{
					chosens.clear();
					chosens.push_back(child);
					bestBiaisedGameScore = child->_biaisedGameScore;
				}
				else if (!child->_biaisedGameScore.biaisedWhiteWinsOver(bestBiaisedGameScore))
				{
					chosens.push_back(child);
				}
			}
		}
		std::uniform_int_distribution<int> aDistributor(0, chosens.size() - 1);
		return chosens[aDistributor(*GENERATOR)];
	}

	HeuristicSelectiveGameNode * const chosenOne() const
	{
		std::vector<HeuristicSelectiveGameNode*> chosens;
		chosens.reserve(_children.size());
		for (auto* const child : _children)
		{
			if (_gameScore == child->_gameScore && !child->isTerminal())
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
	static FixedUnorderedMap<Board, HeuristicSelectiveGameNode*, BoardHash> NODES;

private:

	void backPropagateUtility()
	{
		const GameScore previousGameScore = _gameScore;
		const BiaisedGameScore previousBiaisedGameScore = _biaisedGameScore;
		const bool previousIsTerminal = _isTerminal;
		gatherChildrenUtility();
		if (_gameScore != previousGameScore
			|| _biaisedGameScore != previousBiaisedGameScore
			|| _isTerminal != previousIsTerminal)
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
			HeuristicSelectiveGameNode* childNode(NODES.get(child.currentBoard()).filter([&child](HeuristicSelectiveGameNode* const foundNode) {
				return foundNode->gameSet() == child;
			}).getOrElse([&child, this]() {
				auto* node = new HeuristicSelectiveGameNode(child, _gameScore, _biaisedGameScore);
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

	static const double estimateScore(const GameSet &gameSet)
	{
		if (gameSet.getStatus() == GameStatus::FIFTY_MOVE || gameSet.getStatus() == GameStatus::NO_LEGAL_MOVE || gameSet.getStatus() == GameStatus::THREEFOLD_REPETITION)
			return 0;
		else if (gameSet.getStatus() == GameStatus::WHITE_WIN)
			return 100;
		else if (gameSet.getStatus() == GameStatus::BLACK_WIN)
			return -100;
		else
		{
			double whiteScore = 0;
			whiteScore += 1 * (double)gameSet.currentBoard().bitBoards().populationCount(PieceType::PAWN, true);
			whiteScore += 3 * (double)gameSet.currentBoard().bitBoards().populationCount(PieceType::KNIGHT, true);
			whiteScore += 3 * (double)gameSet.currentBoard().bitBoards().populationCount(PieceType::BISHOP, true);
			whiteScore += 5 * (double)gameSet.currentBoard().bitBoards().populationCount(PieceType::ROOK, true);
			whiteScore += 9 * (double)gameSet.currentBoard().bitBoards().populationCount(PieceType::QUEEN, true);

			double blackScore = 0;
			blackScore += 1 * (double)gameSet.currentBoard().bitBoards().populationCount(PieceType::PAWN, false);
			blackScore += 3 * (double)gameSet.currentBoard().bitBoards().populationCount(PieceType::KNIGHT, false);
			blackScore += 3 * (double)gameSet.currentBoard().bitBoards().populationCount(PieceType::BISHOP, false);
			blackScore += 5 * (double)gameSet.currentBoard().bitBoards().populationCount(PieceType::ROOK, false);
			blackScore += 9 * (double)gameSet.currentBoard().bitBoards().populationCount(PieceType::QUEEN, false);

			const double score = (whiteScore / (blackScore + 1)) - (blackScore / (whiteScore + 1));

			return score;
		}
	}

	void gatherChildrenUtility()
	{
		double averageUtility = _children[0]->_biaisedGameScore.averageUtility();
		if (_gameSet.isWhiteTurn())
		{
			_gameScore = _children[0]->_gameScore;
			_biaisedGameScore = _children[0]->_biaisedGameScore;
			_isTerminal = _children[0]->_isTerminal;
			for (size_t i = 1; i < _children.size(); ++i)
			{
				auto* const node = _children[i];
				GameScore nodeGameScore(node->_gameScore);
				BiaisedGameScore nodeBiaisedGameScore(node->_biaisedGameScore);

				averageUtility += nodeBiaisedGameScore.averageUtility();
				if (nodeGameScore == _gameScore)
				{
					_isTerminal = _isTerminal && node->_isTerminal;
				}
				else if (nodeGameScore.whiteWinsOver(_gameScore))
				{
					_isTerminal = node->_isTerminal;
					_gameScore = nodeGameScore;
				}
				if (nodeBiaisedGameScore.biaisedWhiteWinsOver(_biaisedGameScore))
				{
					_biaisedGameScore = nodeBiaisedGameScore;
				}
			}
		}
		else
		{
			_gameScore = _children[0]->_gameScore;
			_biaisedGameScore = _children[0]->_biaisedGameScore;
			_isTerminal = _children[0]->_isTerminal;
			for (size_t i = 1; i < _children.size(); ++i)
			{
				auto* const node = _children[i];
				GameScore nodeGameScore(node->_gameScore);
				BiaisedGameScore nodeBiaisedGameScore(node->_biaisedGameScore);

				averageUtility += nodeBiaisedGameScore.averageUtility();
				if (nodeGameScore == _gameScore)
				{
					_isTerminal = _isTerminal && node->_isTerminal;
				}
				else if (nodeGameScore.blackWinsOver(_gameScore))
				{
					_isTerminal = node->_isTerminal;
					_gameScore = nodeGameScore;
				}
				if (nodeBiaisedGameScore.biaisedBlackWinsOver(_biaisedGameScore))
				{
					_biaisedGameScore = nodeBiaisedGameScore;
				}
			}
		}
		if (_isLeaf)
		{
			_isLeaf = false;
			_biaisedGameScore.averageUtility(averageUtility / _children.size());
		}
	}

	GameSet _gameSet;
	GameScore _gameScore;
	BiaisedGameScore _biaisedGameScore;

	bool _isLeaf;
	bool _isTerminal;
	size_t _size;
	std::vector<HeuristicSelectiveGameNode*> _parents;
	std::vector<HeuristicSelectiveGameNode*> _children;
};