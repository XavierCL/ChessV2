#pragma once

#include "GenericMoveFactory.h"
#include "BitBoardMoveConstants.h"

class CastleMoveFactory: public GenericMoveFactory
{
public:
	CastleMoveFactory(const Board& currentBoard)
		: GenericMoveFactory(2, currentBoard)
	{}

protected:
	void generateLegalMoves() override
	{
		if (_currentBoard.isWhiteTurn())
		{
			_currentBoard.bitBoards().foreachBoardBit(true, PieceType::KING, [this](const unsigned char& position) {
				if (_currentBoard.castleFlags().canWhiteCastleKingSide() && _currentBoard.bitBoards().arePiecesEmpty(WHITE_KING_SIDE_FIELD))
				{
					if (!_currentBoard.bitBoards().isKingChecked(true))
					{
						BitBoards kingFirstMove = BitBoardMoveConstants::KING_IMMEDIATE_KILL_DATA[position][position - 1]->playPieces(_currentBoard.bitBoards(), true);
						if (!kingFirstMove.isKingChecked(true))
						{
							BitBoards kingSecondMove = BitBoardMoveConstants::KING_IMMEDIATE_KILL_DATA[position - 1][position - 2]->playPieces(kingFirstMove, true);
							if (!kingSecondMove.isKingChecked(true))
							{
								appendAny(BitBoardMoveConstants::CASTLES_MOVES[0]);
							}
						}
					}
				}
				if (_currentBoard.castleFlags().canWhiteCastleQueenSide() && _currentBoard.bitBoards().arePiecesEmpty(WHITE_QUEEN_SIDE_FIELD))
				{
					if (!_currentBoard.bitBoards().isKingChecked(true))
					{
						BitBoards kingFirstMove = BitBoardMoveConstants::KING_IMMEDIATE_KILL_DATA[position][position + 1]->playPieces(_currentBoard.bitBoards(), true);
						if (!kingFirstMove.isKingChecked(true))
						{
							BitBoards kingSecondMove = BitBoardMoveConstants::KING_IMMEDIATE_KILL_DATA[position + 1][position + 2]->playPieces(kingFirstMove, true);
							if (!kingSecondMove.isKingChecked(true))
							{
								appendAny(BitBoardMoveConstants::CASTLES_MOVES[1]);
							}
						}
					}
				}
			});
		}
		else
		{
			_currentBoard.bitBoards().foreachBoardBit(false, PieceType::KING, [this](const unsigned char& position) {
				if (_currentBoard.castleFlags().canBlackCastleKingSide() && _currentBoard.bitBoards().arePiecesEmpty(BLACK_KING_SIDE_FIELD))
				{
					if (!_currentBoard.bitBoards().isKingChecked(false))
					{
						BitBoards kingFirstMove = BitBoardMoveConstants::KING_IMMEDIATE_KILL_DATA[position][position - 1]->playPieces(_currentBoard.bitBoards(), false);
						if (!kingFirstMove.isKingChecked(false))
						{
							BitBoards kingSecondMove = BitBoardMoveConstants::KING_IMMEDIATE_KILL_DATA[position - 1][position - 2]->playPieces(kingFirstMove, false);
							if (!kingSecondMove.isKingChecked(false))
							{
								appendAny(BitBoardMoveConstants::CASTLES_MOVES[2]);
							}
						}
					}
				}
				if (_currentBoard.castleFlags().canBlackCastleQueenSide() && _currentBoard.bitBoards().arePiecesEmpty(BLACK_QUEEN_SIDE_FIELD))
				{
					if (!_currentBoard.bitBoards().isKingChecked(false))
					{
						BitBoards kingFirstMove = BitBoardMoveConstants::KING_IMMEDIATE_KILL_DATA[position][position + 1]->playPieces(_currentBoard.bitBoards(), false);
						if (!kingFirstMove.isKingChecked(false))
						{
							BitBoards kingSecondMove = BitBoardMoveConstants::KING_IMMEDIATE_KILL_DATA[position + 1][position + 2]->playPieces(kingFirstMove, false);
							if (!kingSecondMove.isKingChecked(false))
							{
								appendAny(BitBoardMoveConstants::CASTLES_MOVES[3]);
							}
						}
					}
				}
			});
		}
	}

private:
	static const unsigned long long WHITE_KING_SIDE_FIELD = 0x0000000000000006;
	static const unsigned long long WHITE_QUEEN_SIDE_FIELD = 0x0000000000000070;
	static const unsigned long long BLACK_KING_SIDE_FIELD = 0x0600000000000000;
	static const unsigned long long BLACK_QUEEN_SIDE_FIELD = 0x7000000000000000;
};