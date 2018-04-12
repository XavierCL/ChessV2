#include "../../../Headers/Artificial/ProbabilityHeuristicSelective/ProbabilityHeuristicSelectiveGameNode.h"

std::minstd_rand0* ProbabilityHeuristicSelectiveGameNode::GENERATOR = nullptr;
FixedUnorderedMap<Board, ProbabilityHeuristicSelectiveGameNode*, BoardHash> ProbabilityHeuristicSelectiveGameNode::NODES(1000000);