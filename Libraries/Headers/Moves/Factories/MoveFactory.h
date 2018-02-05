#pragma once

#include "PawnMoveFactory.h"
#include "RookMoveFactory.h"
#include "BishopMoveFactory.h"
#include "KnightMoveFactory.h"
#include "QueenMoveFactory.h"
#include "KingMoveFactory.h"
#include "CastleMoveFactory.h"

class MoveFactory
{
public:
	MoveFactory(const Board& currentBoard, const Option<Board>& lastBoard)
		: _currentBoard(currentBoard),
		_lastBoard(lastBoard)
	{}

	const std::vector<Move const *> getLegalMoves() const
	{
		std::vector<Move const *> legalMoves(PawnMoveFactory(_currentBoard, _lastBoard).getLegalMoves()); // TODO: en passant
		// TODO: can only promote to queen on capture and user can always only promote to queen
		append(legalMoves, RookMoveFactory(_currentBoard).getLegalMoves());
		append(legalMoves, BishopMoveFactory(_currentBoard).getLegalMoves());
		append(legalMoves, KnightMoveFactory(_currentBoard).getLegalMoves());
		append(legalMoves, QueenMoveFactory(_currentBoard).getLegalMoves());
		append(legalMoves, KingMoveFactory(_currentBoard).getLegalMoves());
		append(legalMoves, CastleMoveFactory(_currentBoard).getLegalMoves());

		return legalMoves;
	}

private:

	template <typename _VectorType>
	static void append(std::vector<_VectorType>& a, const std::vector<_VectorType>& b)
	{
		a.insert(a.end(), b.cbegin(), b.cend());
	}

	const Board _currentBoard;
	const Option<Board> _lastBoard;
};