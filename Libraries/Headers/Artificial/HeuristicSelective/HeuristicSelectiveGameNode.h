#pragma once

#include "../../Game/GameSet.h"
#include "GameScore.h"
#include "BiaisedGameScore.h"

#include "../../utils/Hash.hpp"

#include <random>
#include <unordered_set>
#include <numeric>

// This implementation doesn't support cyclic references
// This is fine because supported board games can't last an infinite number of moves.

class HeuristicSelectiveGameNode
{
public:

	HeuristicSelectiveGameNode(const GameSet &gameSet)
		: _gameSet(gameSet),
		_parents(),
		_isLeaf(true),
		_isTerminal(gameSet.getStatus() != GameStatus::LIVE),
		_size(1),
		_gameScore(estimateScore(gameSet)),
		_biaisedGameScore(estimateScore(gameSet))
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

	~HeuristicSelectiveGameNode()
	{
		for (const auto child : _children)
		{
			child->removeParent(this);
		}
	}

	void addParent(HeuristicSelectiveGameNode* parent)
	{
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

	void removeParent(HeuristicSelectiveGameNode* parent)
	{
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

	size_t size()
	{
		return _size;
	}

	double biaisedGameScoreUtility() const
	{
		return _biaisedGameScore.utility();
	}

	template <typename EngineType>
	size_t develop(EngineType& randomEngine, FixedUnorderedMap<Board, std::shared_ptr<HeuristicSelectiveGameNode>, BoardHash> &repository, FixedUnorderedMap<Board, std::shared_ptr<std::vector<Move const *>>, BoardHash> &legalCache)
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

	std::shared_ptr<HeuristicSelectiveGameNode> findChild(const Board& board)
	{
		for (auto child : _children)
		{
			if (child->gameSet().currentBoard() == board)
			{
				return child;
			}
		}
		throw std::exception("No child matched the next move");
	}

	GameSet gameSet() const
	{
		return _gameSet;
	}

	template <typename EngineType>
	std::shared_ptr<HeuristicSelectiveGameNode> biaisedChosenOne(EngineType& randomEngine) const
	{
		std::vector<std::shared_ptr<HeuristicSelectiveGameNode>> chosens{ _children[0] };
		chosens.reserve(_children.size());

		std::for_each(_children.begin() + 1, _children.end(), [this, &chosens](const std::shared_ptr<HeuristicSelectiveGameNode> &nextCandidate)
		{
			if (nextCandidate->_biaisedGameScore.biaisedWinOver(_gameSet.isWhiteTurn(), chosens.front()->_biaisedGameScore))
			{
				chosens = { nextCandidate };
			}
			else if (nextCandidate->_biaisedGameScore == chosens.front()->_biaisedGameScore)
			{
				chosens.push_back(nextCandidate);
			}
		});

		std::uniform_int_distribution<int> aDistributor(0, chosens.size() - 1);
		return chosens[aDistributor(randomEngine)];
	}

	bool isTerminal() const
	{
		return _isTerminal;
	}

private:

	template <typename EngineType>
	std::shared_ptr<HeuristicSelectiveGameNode> chosenOne(EngineType& randomEngine) const
	{
		std::vector<std::shared_ptr<HeuristicSelectiveGameNode>> chosens;
		chosens.reserve(_children.size());

		for (auto child : _children)
			if (child->_gameScore == _gameScore)
			{
				chosens.push_back(child);
			}

		std::uniform_int_distribution<int> aDistributor(0, chosens.size() - 1);
		return chosens[aDistributor(randomEngine)];
	}

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
			for (auto parent : _parents)
			{
				parent->backPropagateUtility();
			}
		}
	}

	size_t developImmediateChildren(FixedUnorderedMap<Board, std::shared_ptr<HeuristicSelectiveGameNode>, BoardHash> &repository, FixedUnorderedMap<Board, std::shared_ptr<std::vector<Move const *>>, BoardHash> &legalCache)
	{
		size_t realNewNodeCount = 0;
		_children.reserve(_gameSet.getLegals()->size());
		for (Move const * const move : *_gameSet.getLegals())
		{
			const GameSet child(_gameSet.playMove(*move, legalCache));

			std::shared_ptr<HeuristicSelectiveGameNode> childNode(repository.get(child.currentBoard()).filter([&child](const std::shared_ptr<HeuristicSelectiveGameNode> &foundNode) {
				return foundNode->gameSet() == child;
			}).getOrElse([this, &child, &repository, &realNewNodeCount]() {
				const auto node = std::make_shared<HeuristicSelectiveGameNode>(child, _gameScore, _biaisedGameScore);
				++realNewNodeCount;
				repository.set(child.currentBoard(), node);
				return node;
			}));

			childNode->addParent(this);
			_children.push_back(childNode);
		}

		_isLeaf = false;

		backPropagateUtility();

		return realNewNodeCount;
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
		auto nonTerminalChildren(_children);
		std::remove_if(nonTerminalChildren.begin(), nonTerminalChildren.end(), [](const std::shared_ptr<HeuristicSelectiveGameNode>& child)
		{
			return child->isTerminal();
		});

		std::vector<GameScore> nonTerminalGameScores;
		std::transform(nonTerminalChildren.begin(), nonTerminalChildren.end(), nonTerminalGameScores.begin(), [](const std::shared_ptr<HeuristicSelectiveGameNode>& nonTerminalChild)
		{
			return nonTerminalChild->_gameScore;
		});

		std::vector<BiaisedGameScore> biaisedGameScores;
		std::transform(_children.begin(), _children.end(), biaisedGameScores.begin(), [](const std::shared_ptr<HeuristicSelectiveGameNode>& nonTerminalChild)
		{
			return nonTerminalChild->_biaisedGameScore;
		});

		_isTerminal = !nonTerminalChildren.empty();
		if (!_isTerminal)
		{
			_gameScore = GameScore(nonTerminalGameScores, _gameSet.isWhiteTurn());
		}
		_biaisedGameScore = BiaisedGameScore(biaisedGameScores, _gameSet.isWhiteTurn());
	}

	const GameSet _gameSet;

	BiaisedGameScore _biaisedGameScore;
	GameScore _gameScore;

	bool _isLeaf;
	bool _isTerminal;
	size_t _size;
	std::unordered_set<HeuristicSelectiveGameNode*, PointerHash<HeuristicSelectiveGameNode>> _parents;
	std::vector<std::shared_ptr<HeuristicSelectiveGameNode>> _children;
};