#include "../../../Headers/Artificial/HeuristicSelective/HeuristicSelectiveGameNode.h"

std::minstd_rand0* HeuristicSelectiveGameNode::GENERATOR = nullptr;
FixedUnorderedMap<Board, HeuristicSelectiveGameNode*, BoardHash> HeuristicSelectiveGameNode::NODES(1000000);