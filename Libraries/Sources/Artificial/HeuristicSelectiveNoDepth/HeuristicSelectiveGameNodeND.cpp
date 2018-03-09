#include "../../../Headers/Artificial/HeuristicSelectiveNoDepth/HeuristicSelectiveGameNodeND.h"

std::minstd_rand0* HeuristicSelectiveGameNodeND::GENERATOR = nullptr;
FixedUnorderedMap<Board, HeuristicSelectiveGameNodeND*, BoardHash> HeuristicSelectiveGameNodeND::NODES(1);