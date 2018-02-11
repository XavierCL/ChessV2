#include "../../../Headers/Moves/Factories/MoveFactory.h"

std::unordered_map<Board, std::vector<Move const *>, BoardHash> MoveFactory::_legalMap;

const std::unordered_map<Board, std::vector<Move const *>, BoardHash>::const_iterator MoveFactory::_endOfLegalMap(MoveFactory::_legalMap.cend());