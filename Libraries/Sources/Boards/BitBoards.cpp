#include "../../Headers/Boards/BitBoard.h"

const unsigned char BitBoards::INDEXED_PIECE_TYPE_TO_BOARD[] = { PAWN_INDEX, KNIGHT_INDEX, BISHOP_INDEX, ROOK_INDEX, QUEEN_INDEX, KING_INDEX };
const std::hash<unsigned long long> BitBoards::_bitBoardHash;