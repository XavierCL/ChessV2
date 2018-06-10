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
	MoveFactory(const Board& currentBoard)
		: _currentBoard(currentBoard)
	{}

	const std::shared_ptr<std::vector<Move const *>> getLegalMoves() const
	{
		std::shared_ptr<std::vector<Move const *>> legalMoves(std::make_shared<std::vector<Move const *>>());

		FastMath::pushAllPointer(legalMoves, PawnMoveFactory(_currentBoard).getLegalMoves()); // check en passant
		FastMath::pushAllPointer(legalMoves, KnightMoveFactory(_currentBoard).getLegalMoves());
		FastMath::pushAllPointer(legalMoves, BishopMoveFactory(_currentBoard).getLegalMoves());
		FastMath::pushAllPointer(legalMoves, RookMoveFactory(_currentBoard).getLegalMoves());
		FastMath::pushAllPointer(legalMoves, QueenMoveFactory(_currentBoard).getLegalMoves());
		FastMath::pushAllPointer(legalMoves, KingMoveFactory(_currentBoard).getLegalMoves());
		FastMath::pushAllPointer(legalMoves, CastleMoveFactory(_currentBoard).getLegalMoves());

		return legalMoves;
	}

	const std::shared_ptr<std::vector<Move const *>> getLegalMoves(FixedUnorderedMap<Board, std::shared_ptr<std::vector<Move const *>>, BoardHash>& cache) const
	{
		return cache.get(_currentBoard).getOrElse([this, &cache]() {
			auto legalMoves(getLegalMoves());

			cache.set(_currentBoard, legalMoves);

			return legalMoves;
		});
	}

private:
	const Board _currentBoard;
};