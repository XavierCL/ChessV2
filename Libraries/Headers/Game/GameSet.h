#ifndef GAMESET_H
#define GAMESET_H

#include "../Moves/Factories/MoveFactory.h"

#include "../Boards/BoardHash.h"

#include "../Pieces/Piece.h"

#include "GameStatus.h"

#include <unordered_map>

class GameSet
{
public:

	GameSet()
		: _currentBoard(),
		_lastBoard(),
		_history{ { _currentBoard , 1 } },
		_remainingNonCapturingCount(ALLOWED_NON_CAPTURING_COUNT),
		_gameStatusNoMatterLegalMoves(generateGameStatusNoMatterLegalMoves()),
		_legalMoves(generateLegalMoves()),
		_gameStatus(generateGameStatus())
	{}

	GameSet(const Board& currentBoard, const Board& lastBoard, const std::unordered_map<Board, unsigned char, BoardHash>& history, const unsigned char& remainingNonCapturingCount)
		: _currentBoard(currentBoard),
		_lastBoard(lastBoard),
		_history(history),
		_remainingNonCapturingCount(remainingNonCapturingCount),
		_gameStatusNoMatterLegalMoves(generateGameStatusNoMatterLegalMoves()),
		_legalMoves(generateLegalMoves()),
		_gameStatus(generateGameStatus())
	{}

	const GameSet playMove(const Move &move) const
	{
		const Board newBoard(move.play(_currentBoard));
		std::unordered_map<Board, unsigned char, BoardHash> newHistory(_history);
		newHistory[newBoard] = newHistory[newBoard] + 1;

		return GameSet(
			newBoard,
			_currentBoard,
			newHistory,
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

	const Board currentBoard() const
	{
		return _currentBoard;
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

	const bool isThreeFoldRepetitionDraw() const
	{
		return _history.at(_currentBoard) > ALLOWED_REPETITIONS;
	}

	const Option<GameStatus> generateGameStatusNoMatterLegalMoves() const
	{
		if (isFiftyMoveDraw())
		{
			return Option<GameStatus>(GameStatus::FIFTY_MOVE);
		}
		else if (isThreeFoldRepetitionDraw())
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
				_currentBoard,
				_lastBoard
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
	const Option<Board> _lastBoard;
	const std::unordered_map<Board, unsigned char, BoardHash> _history;
	const unsigned char _remainingNonCapturingCount;
	const Option<GameStatus> _gameStatusNoMatterLegalMoves;
	const std::shared_ptr<std::vector<Move const *>> _legalMoves;
	const GameStatus _gameStatus;
};

#endif