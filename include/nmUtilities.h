#ifndef nmUTILITIES_H
#define nmUTILITIES_H

#include "nmNeuron.h"

#include <vector>
#include <unordered_map>

namespace nm
{
	const double CCF_MIDDLE_25TH = 228;
	const double CCF_MIDDLE = 5700;

	//!
	//! Populates input references of nodeIDMap and nodeID2childMap based on input nodes
	void populateMaps(const std::vector<const Node*> nodes, std::unordered_map<int, const Node*>& nodeIDMap, std::unordered_map<int, std::vector<const Node*>>& nodeID2childMap);
	
	std::vector<const Node*> getTipNodes(const std::vector<Node>& nodes, const std::unordered_map<int, const Node*>& nodeIDMap, const std::unordered_map<int, std::vector<const Node*>>& nodeID2childMap);

	//!
	//! Compute total length of [nodes]
	double getNodesLength(const std::vector<const Node*> nodes);

	//!
	//! Returns a vector of vectors of ptrs to spike nodes
	std::vector<std::vector<const Node*>> getSpikeNodes(const Neuron& neuron, double spikeLengthThreshold);

}

#endif