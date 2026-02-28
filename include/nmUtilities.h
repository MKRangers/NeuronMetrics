#ifndef nmUTILITIES_H
#define nmUTILITIES_H

#include "nmNode.h"

#include <vector>
#include <unordered_map>

namespace nm
{
	const double CCF_MIDDLE_25TH = 228;
	const double CCF_MIDDLE = 5700;

	void populateNodeID2locChildLocMaps(std::vector<const Node*> nodes, std::unordered_map<int, int>& nodeID2LocMap, std::unordered_map<int, std::vector<int>>& nodeID2childLocMap);
	
	double getNodesLength(std::vector<const Node*> nodes);

}

#endif