#ifndef nmUTILITIES_H
#define nmUTILITIES_H

#include "nmNode.h"

#include <vector>
#include <unordered_map>

namespace nm
{
	const double CCF_MIDDLE_25TH = 228;
	const double CCF_MIDDLE = 5700;

	void populateNodeID2locChildLocMaps(const std::vector<const Node*> nodes, std::unordered_map<int, int>& nodeID2LocMap, std::unordered_map<int, std::vector<int>>& nodeID2childLocMap);
	
	//!
	//! Compute total length of [nodes]
	double getNodesLength(const std::vector<const Node*> nodes);

	//!
	//! Returns a vector of vectors of spike node locations in [nodes]
	//std::vector<std::vector<int>> getSpikes(const std::vector<const Node*> nodes, std::unordered_map<int, int>& nodeID2LocMap, std::unordered_map<int, std::vector<int>>& nodeID2childLocMap);

}

#endif