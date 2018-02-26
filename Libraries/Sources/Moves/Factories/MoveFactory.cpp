#include "../../../Headers/Moves/Factories/MoveFactory.h"

FixedUnorderedMap<Board, std::shared_ptr<std::vector<Move const *>>, BoardHash> MoveFactory::_legalMap(1800000);