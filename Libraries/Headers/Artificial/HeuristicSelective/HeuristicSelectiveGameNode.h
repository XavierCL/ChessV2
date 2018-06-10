#pragma once

#include "../../Game/GameSet.h"
#include "GameScore.h"
#include "BiaisedGameScore.h"
#include "Deleter.h"

#include "../../utils/Hash.hpp"
#include "../../utils/Logger.hpp"

#include <random>
#include <unordered_set>
#include <string>

// This implementation doesn't support cyclic references
// This is fine because such board games can't last an infinite number of moves.

class HeuristicSelectiveGameNode
{
public:

	HeuristicSelectiveGameNode(const GameSet &gameSet)
		: _gameSet(gameSet),
		_parents(),
		_isLeaf(true),
		_isTerminal(gameSet.getStatus() != GameStatus::LIVE),
		_size(1),
		_gameScore(GameScore(estimateScore(gameSet), GameScore())),
		_biaisedGameScore(BiaisedGameScore(estimateScore(gameSet), BiaisedGameScore()))
	{}

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
		Logger::info("Node " + std::to_string(reinterpret_cast<size_t>(this)) + " added parent " + std::to_string(reinterpret_cast<size_t>(parent)) + "\n");
		auto thisParent = _parents.find(parent);
		if (thisParent == _parents.cend())
		{
			_parents.insert(parent);
		}
		else
		{
			throw std::exception("Parent Node already existed.");
		}
		
	}

	const bool hasParent() const
	{
		return !_parents.empty();
	}

	void removeParent(HeuristicSelectiveGameNode * const &parent)
	{
		Logger::info("Node " + std::to_string(reinterpret_cast<size_t>(this)) + " removed parent " + std::to_string(reinterpret_cast<size_t>(parent)) + "\n");
		auto thisParent = _parents.find(parent);
		if (thisParent != _parents.cend())
		{
			_parents.erase(thisParent);
		}
		else
		{
			throw std::exception("Node parent did not exist or was already removed");
		}
	}

	const size_t removeRecursive(FixedUnorderedMap<Board, HeuristicSelectiveGameNode*, BoardHash> &repository, Deleter<HeuristicSelectiveGameNode>& deleter)
	{
		size_t removedCount = 0;
		for (auto * child : _children)
		{
			child->removeParent(this);
			if (!child->hasParent())
			{
				Logger::info("Node " + std::to_string(reinterpret_cast<size_t>(child)) + " removed recursive: no parent\n");
				removedCount += child->removeRecursive(repository, deleter);
			}
		}
		removedCount += removeSingle(repository, deleter);
		return removedCount;
	}

	const size_t removeSingle(FixedUnorderedMap<Board, HeuristicSelectiveGameNode*, BoardHash> &repository, Deleter<HeuristicSelectiveGameNode>& deleter)
	{
		removeThisFromRepository(repository);
		deleter.scheduleDeletion(this);
		Logger::info("Node " + std::to_string(reinterpret_cast<size_t>(this)) + " deleted\n");
		return 1;
	}

	const BiaisedGameScore biaisedGameScore() const
	{
		return _biaisedGameScore;
	}

	template <typename EngineType>
	const size_t develop(EngineType& randomEngine, FixedUnorderedMap<Board, HeuristicSelectiveGameNode*, BoardHash> &repository, FixedUnorderedMap<Board, std::shared_ptr<std::vector<Move const *>>, BoardHash> &legalCache)
	{
		if (_isTerminal)
		{
			return 0;
		}
		else
		{
			if (_isLeaf)
			{
				return developImmediateChildren(repository, legalCache);
			}
			else
			{
				return chosenOne(randomEngine)->develop(randomEngine, repository, legalCache);
			}
		}
	}

	std::vector<HeuristicSelectiveGameNode*> children()
	{
		return _children;
	}

	const GameSet gameSet() const
	{
		return _gameSet;
	}

	template <typename EngineType>
	HeuristicSelectiveGameNode * const biaisedChosenOne(EngineType& randomEngine) const
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
		return chosens[aDistributor(randomEngine)];
	}

	template <typename EngineType>
	HeuristicSelectiveGameNode * const chosenOne(EngineType& randomEngine) const
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
		return chosens[aDistributor(randomEngine)];
	}

	const bool isTerminal() const
	{
		return _isTerminal;
	}

	const size_t size() const
	{
		return _size;
	}

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

	const size_t developImmediateChildren(FixedUnorderedMap<Board, HeuristicSelectiveGameNode*, BoardHash> &repository, FixedUnorderedMap<Board, std::shared_ptr<std::vector<Move const *>>, BoardHash> &legalCache)
	{
		_children.reserve(_gameSet.getLegals()->size());
		size_t addedSize = 0;
		size_t realAddedSize = 0;
		for (Move const * const move : *_gameSet.getLegals())
		{
			const GameSet child(_gameSet.playMove(*move, legalCache));
			HeuristicSelectiveGameNode* childNode(repository.get(child.currentBoard()).filter([&child](HeuristicSelectiveGameNode* const foundNode) {
				const bool isRightGameSet = foundNode->gameSet() == child;
				if (isRightGameSet)
				{
					Logger::info("Node " + std::to_string(reinterpret_cast<size_t>(foundNode)) + " reused\n");
				}
				return isRightGameSet;
			}).getOrElse([this, &child, &repository, &realAddedSize]() {
				++realAddedSize;
				auto* node = new HeuristicSelectiveGameNode(child, _gameScore, _biaisedGameScore);
				repository.set(child.currentBoard(), node);
				Logger::info("Node " + std::to_string(reinterpret_cast<size_t>(node)) + " created\n");
				return node;
			}));
			childNode->addParent(this);
			_children.push_back(childNode);
			addedSize += childNode->size();
		}
		backPropagateSize(addedSize);
		backPropagateUtility();
		return realAddedSize;
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

	void removeThisFromRepository(FixedUnorderedMap<Board, HeuristicSelectiveGameNode*, BoardHash> &repository) const
	{
		repository.get(gameSet().currentBoard()).filter([this](HeuristicSelectiveGameNode * const foundNode) {
			return this == foundNode;
		}).foreach([this, &repository](HeuristicSelectiveGameNode * const foundNode) {
			repository.remove(gameSet().currentBoard());
		});
	}

	GameSet _gameSet;
	GameScore _gameScore;
	BiaisedGameScore _biaisedGameScore;

	bool _isLeaf;
	bool _isTerminal;
	size_t _size;
	std::unordered_set<HeuristicSelectiveGameNode*, PointerHash<HeuristicSelectiveGameNode>> _parents;
	std::vector<HeuristicSelectiveGameNode*> _children;
};