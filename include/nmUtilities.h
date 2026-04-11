#ifndef nmUTILITIES_H
#define nmUTILITIES_H

#include "nmNeuron.h"

namespace nm
{
	
	//!
	//! Returns the maximum node ID in [nodes]
	int getNodeIDMax(const std::vector<Node>& nodes);

	//!
	//! Populates input references of nodeIDMap and nodeID2childMap based on input nodes
	void populateMaps(const std::vector<const Node*> nodes, std::unordered_map<int, const Node*>& nodeIDMap, std::unordered_map<int, std::vector<const Node*>>& nodeID2childMap);
	
	//!
	//! Returns a vector of ptrs to tip nodes in [nodes] based on input node
	std::vector<const Node*> getTipNodes(const std::vector<Node>& nodes, const std::unordered_map<int, const Node*>& nodeIDMap, const std::unordered_map<int, std::vector<const Node*>>& nodeID2childMap);

	//!
	//! Compute total length of [nodes]
	double getNodesLength(const std::vector<const Node*> nodes);

	//!
	//! Returns a vector of vectors of ptrs to spike nodes
	std::vector<std::vector<const Node*>> getSpikeNodes(const Neuron& neuron, double spikeLengthThreshold);

	//!
	//! Interpolates nodes in the neuron at the specified interval
	std::vector<Node> interpolateNodes(Neuron& neuron, double interval = 1.0);

}

#endif