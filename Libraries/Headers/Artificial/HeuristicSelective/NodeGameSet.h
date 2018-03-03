#pragma once

#include "../../Game/GameSet.h"

class NodeGameSet
{
public:
	NodeGameSet(const GameSet& gameSet)
		: _parent(nullptr),
		_rootGameSet(gameSet),
		_currentBoard(gameSet.currentBoard()),
		_remainingNonCapturingCount(gameSet.remainingNonCapturingCount()),
		_legalMoves(gameSet.getLegals()),
		_gameStatus(gameSet.getStatus())
	{}

	NodeGameSet(NodeGameSet const* parent, const Board& currentBoard, const unsigned char& remainingNonCapturingCount)
		: _parent(parent),
		_rootGameSet(),
		_currentBoard(currentBoard),
		_remainingNonCapturingCount(remainingNonCapturingCount)
	{
		_gameStatusNoMatterLegalMoves = generateGameStatusNoMatterLegalMoves();
		_legalMoves = generateLegalMoves();
		_gameStatus = generateGameStatus();
	}

	const NodeGameSet playMove(const Move &move) const
	{
		const Board newBoard(move.play(_currentBoard));

		return NodeGameSet(
			this,
			newBoard,
			isFiftyMoveReset(_currentBoard, newBoard) ? ALLOWED_NON_CAPTURING_COUNT : _remainingNonCapturingCount - 1
		);
	}

	const std::shared_ptr<std::vector<Move const *>> getLegals() const
	{
		return _legalMoves;
	}

	const GameStatus& getStatus() const
	{
		return _gameStatus;
	}

	const bool& isWhiteTurn()const
	{
		return _currentBoard.isWhiteTurn();
	}

	const Piece get(const unsigned char& x, const unsigned char& y) const
	{
		return _currentBoard.bitBoards().getPiece(x, y);
	}

	const Board& currentBoard() const
	{
		return _currentBoard;
	}

	void setRoot(const GameSet& gameSet)
	{
		_rootGameSet = Option<GameSet>(gameSet);
		_parent = nullptr;
	}

	const GameSet rootGameSet() const
	{
		return _rootGameSet.getOrElse([]() {
			throw std::exception("Asked non root for the root gameSet");
			return GameSet();
		});
	}

private:

	const static unsigned char ALLOWED_NON_CAPTURING_COUNT = 50;
	const static unsigned char ALLOWED_REPETITIONS = 2;

	const bool isFiftyMoveReset(const Board& lastBoard, const Board& currentBoard) const
	{
		return !currentBoard.bitBoards().equalsSinglePieceType(PieceType::PAWN, lastBoard.bitBoards())
			|| !currentBoard.bitBoards().pieceCountEquals(lastBoard.bitBoards());
	}

	const bool isFiftyMoveDraw() const
	{
		return _remainingNonCapturingCount == 0;
	}

	const bool isThreeFoldRepetitionDraw(const Board& board) const
	{
		return _rootGameSet.map<unsigned char>([&board](const GameSet& gameSet) {
			return gameSet.gameCount(board);
		}).getOrElse([this, &board]() {
			return (board == _currentBoard ? 1 : 0) + _parent->isThreeFoldRepetitionDraw(board);
		}) > ALLOWED_REPETITIONS;
	}

	const Option<GameStatus> generateGameStatusNoMatterLegalMoves() const
	{
		if (isFiftyMoveDraw())
		{
			return Option<GameStatus>(GameStatus::FIFTY_MOVE);
		}
		else if (isThreeFoldRepetitionDraw(_currentBoard))
		{
			return Option<GameStatus>(GameStatus::THREEFOLD_REPETITION);
		}
		else
		{
			return Option<GameStatus>();
		}
	}

	const std::shared_ptr<std::vector<Move const *>> generateLegalMoves() const
	{
		if (_gameStatusNoMatterLegalMoves == Option<GameStatus>(GameStatus::LIVE) || _gameStatusNoMatterLegalMoves == Option<GameStatus>())
		{
			return MoveFactory(
				_currentBoard
			).getLegalMoves();
		}
		else
		{
			return std::make_shared<std::vector<Move const *>>();
		}
	}

	const GameStatus generateGameStatus() const
	{
		return _gameStatusNoMatterLegalMoves.getOrElse([this]() {
			if (_currentBoard.bitBoards().isKingChecked(_currentBoard.isWhiteTurn()) && _legalMoves->empty())
			{
				if (_currentBoard.isWhiteTurn())
				{
					return GameStatus::BLACK_WIN;
				}
				else
				{
					return GameStatus::WHITE_WIN;
				}
			}
			else if (_legalMoves->empty())
			{
				return GameStatus::NO_LEGAL_MOVE;
			}
			else
			{
				return GameStatus::LIVE;
			}
		});
	}

	const Board _currentBoard;
	const unsigned char _remainingNonCapturingCount;
	Option<GameStatus> _gameStatusNoMatterLegalMoves;
	std::shared_ptr<std::vector<Move const *>> _legalMoves;
	GameStatus _gameStatus;
	Option<GameSet> _rootGameSet;
	NodeGameSet const * _parent;
};