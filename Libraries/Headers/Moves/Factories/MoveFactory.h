#pragma once

#include "../../Boards/BoardHash.h"

#include "PawnMoveFactory.h"
#include "RookMoveFactory.h"
#include "BishopMoveFactory.h"
#include "KnightMoveFactory.h"
#include "QueenMoveFactory.h"
#include "KingMoveFactory.h"
#include "CastleMoveFactory.h"

#include "../../utils/FixedUnorderedMap.hpp"

#include <memory>

class MoveFactory
{
public:
	MoveFactory(const Board& currentBoard, const Option<Board>& lastBoard)
		: _currentBoard(currentBoard),
		_lastBoard(lastBoard)
	{}

	const std::shared_ptr<std::vector<Move const *>> getLegalMoves() const
	{
		return _legalMap.get(_currentBoard).getOrElse([this]() {
			std::shared_ptr<std::vector<Move const *>> legalMoves(std::make_shared<std::vector<Move const *>>());

			FastMath::pushAllPointer(legalMoves, PawnMoveFactory(_currentBoard, _lastBoard).getLegalMoves());
			FastMath::pushAllPointer(legalMoves, KnightMoveFactory(_currentBoard).getLegalMoves());
			FastMath::pushAllPointer(legalMoves, BishopMoveFactory(_currentBoard).getLegalMoves());
			FastMath::pushAllPointer(legalMoves, RookMoveFactory(_currentBoard).getLegalMoves());
			FastMath::pushAllPointer(legalMoves, QueenMoveFactory(_currentBoard).getLegalMoves());
			FastMath::pushAllPointer(legalMoves, KingMoveFactory(_currentBoard).getLegalMoves());
			FastMath::pushAllPointer(legalMoves, CastleMoveFactory(_currentBoard).getLegalMoves());

			_legalMap.set(_currentBoard, legalMoves);

			return legalMoves;
		});
	}

private:
	static FixedUnorderedMap<Board, std::shared_ptr<std::vector<Move const *>>, BoardHash> _legalMap;

	const Board _currentBoard;
	const Option<Board> _lastBoard;
};