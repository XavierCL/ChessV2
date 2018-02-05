#include "../../../Headers/Moves/Factories/BitBoardMoveConstants.h"

std::vector<Move const *> BitBoardMoveConstants::PAWN_MOVES[2][65][65] = {};
Move const * BitBoardMoveConstants::PAWN_IMMEDIATE_CAPTURE_DATA[2][65][65] = {};

std::vector<Move const *> BitBoardMoveConstants::ROOK_UPWARD_CAPTURES[65][2][65] = {};
std::vector<Move const *> BitBoardMoveConstants::ROOK_DOWNWARD_CAPTURES[65][2][65] = {};
std::vector<Move const *> BitBoardMoveConstants::ROOK_LEFTWARD_CAPTURES[65][2][65] = {};
std::vector<Move const *> BitBoardMoveConstants::ROOK_RIGHTWARD_CAPTURES[65][2][65] = {};

std::vector<Move const *> BitBoardMoveConstants::BISHOP_UP_LEFT_WARD_CAPTURES[65][2][65] = {};
std::vector<Move const *> BitBoardMoveConstants::BISHOP_UP_RIGHT_WARD_CAPTURES[65][2][65] = {};
std::vector<Move const *> BitBoardMoveConstants::BISHOP_DOWN_LEFT_WARD_CAPTURES[65][2][65] = {};
std::vector<Move const *> BitBoardMoveConstants::BISHOP_DOWN_RIGHT_WARD_CAPTURES[65][2][65] = {};

Move const * BitBoardMoveConstants::KNIGHT_IMMEDIATE_KILL_DATA[65][65] = {};

std::vector<Move const *> BitBoardMoveConstants::QUEEN_UPWARD_CAPTURES[65][2][65] = {};
std::vector<Move const *> BitBoardMoveConstants::QUEEN_DOWNWARD_CAPTURES[65][2][65] = {};
std::vector<Move const *> BitBoardMoveConstants::QUEEN_LEFTWARD_CAPTURES[65][2][65] = {};
std::vector<Move const *> BitBoardMoveConstants::QUEEN_RIGHTWARD_CAPTURES[65][2][65] = {};
std::vector<Move const *> BitBoardMoveConstants::QUEEN_UP_LEFT_WARD_CAPTURES[65][2][65] = {};
std::vector<Move const *> BitBoardMoveConstants::QUEEN_UP_RIGHT_WARD_CAPTURES[65][2][65] = {};
std::vector<Move const *> BitBoardMoveConstants::QUEEN_DOWN_LEFT_WARD_CAPTURES[65][2][65] = {};
std::vector<Move const *> BitBoardMoveConstants::QUEEN_DOWN_RIGHT_WARD_CAPTURES[65][2][65] = {};

Move const * BitBoardMoveConstants::KING_IMMEDIATE_KILL_DATA[65][65] = {};

Move const * BitBoardMoveConstants::CASTLES_MOVES[4] = {};