#pragma once

#include "../Effectors/Move.h"

#include <algorithm>

class GenericMoveFactory
{
public:
	GenericMoveFactory(const size_t& maximumSize, const Board& board)
		: _moves(maximumSize),
		_currentBoard(board)
	{
		_moves.resize(0);
	}

	const std::vector<Move const *>& getLegalMoves()
	{
		generateLegalMoves();
		return _moves;
	}

protected:
	const Board& _currentBoard;

	void append(const std::vector<Move const *>& b)
	{
		const size_t size = b.size();
		for (size_t i = 0; i < b.size(); ++i)
		{
			if (!b[i]->wouldKingBeChecked(_currentBoard)) _moves.push_back(b[i]);
		}
	}

	void checklessAppend(Move const * b)
	{
		_moves.push_back(b);
	}

	virtual void generateLegalMoves() = 0;
private:
	std::vector<Move const *> _moves;
};