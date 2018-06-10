#include "../../Headers/Game/GameSet.h"

const std::hash<unsigned char> GameSet::CHAR_HASH;

const std::hash<size_t> GameSet::SIZE_HASH;

const std::shared_ptr<std::vector<Move const *>> GameSet::EMPTY_MOVES(std::make_shared<std::vector<Move const *>>());