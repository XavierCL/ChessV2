#pragma once

#include "../../Boards/BoardHash.h"

#include "PawnMoveFactory.h"
#include "RookMoveFactory.h"
#include "BishopMoveFactory.h"
#include "KnightMoveFactory.h"
#include "QueenMoveFactory.h"
#include "KingMoveFactory.h"
#include "CastleMoveFactory.h"

#include <unordered_map>

class MoveFactory
{
public:
	MoveFactory(const Board& currentBoard, const Option<Board>& lastBoard)
		: _currentBoard(currentBoard),
		_lastBoard(lastBoard)
	{}

	const std::vector<Move const *> getLegalMoves() const
	{
		const auto alreadyComputedLegals = _legalMap.find(_currentBoard);
		if (alreadyComputedLegals != _endOfLegalMap)
		{
			return alreadyComputedLegals->second;
		}
		else
		{
			std::vector<Move const *> legalMoves;
			
			append(legalMoves, PawnMoveFactory(_currentBoard, _lastBoard).getLegalMoves());
			append(legalMoves, KnightMoveFactory(_currentBoard).getLegalMoves());
			append(legalMoves, BishopMoveFactory(_currentBoard).getLegalMoves());
			append(legalMoves, RookMoveFactory(_currentBoard).getLegalMoves());
			append(legalMoves, QueenMoveFactory(_currentBoard).getLegalMoves());
			append(legalMoves, KingMoveFactory(_currentBoard).getLegalMoves());
			append(legalMoves, CastleMoveFactory(_currentBoard).getLegalMoves());

			_legalMap[_currentBoard] = legalMoves;

			return legalMoves;
		}
	}

private:
	static std::unordered_map<Board, std::vector<Move const *>, BoardHash> _legalMap;
	static const std::unordered_map<Board, std::vector<Move const *>, BoardHash>::const_iterator _endOfLegalMap;

	template <typename _VectorType>
	static void append(std::vector<_VectorType>& a, const std::vector<_VectorType>& b)
	{
		a.insert(a.end(), b.cbegin(), b.cend());
	}

	const Board _currentBoard;
	const Option<Board> _lastBoard;
};