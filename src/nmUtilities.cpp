#include "nmUtilities.h"
#include "nmExceptions.h"

#include <iostream>
#include <map>

using namespace std;

namespace nm
{

	void populateMaps(const vector<const Node*> nodes, unordered_map<int, const Node*>& nodeIDMap, unordered_map<int, vector<const Node*>>& nodeID2childMap)
	{
		nodeIDMap.clear();
		nodeID2childMap.clear();
		nodeIDMap.reserve(nodes.size());
		nodeID2childMap.reserve(nodes.size());

		for (int i = 0; i < nodes.size(); ++i)
		{
			const Node* node = nodes[i];
			nodeIDMap.emplace(node->getID(), node);
			if (node->getParentID() != -1)
				nodeID2childMap[node->getParentID()].push_back(node);
		}
	}

	vector<const Node*> getTipNodes(const vector<Node>& nodes, const unordered_map<int, const Node*>& nodeIDMap, const unordered_map<int, vector<const Node*>>& nodeID2childMap)
	{
		if (nodeIDMap.empty() || nodeID2childMap.empty())
		{
			stringstream s("No data in node maps.");
			throw NeuronNodeMapsHaveNoDataException(s);
		}

		vector<const Node*> outputNodes;
		for (auto& node : nodes)
		{
			if (nodeID2childMap.find(node.getID()) == nodeID2childMap.end())
				outputNodes.push_back(&node);
		}

		return outputNodes;
	}

	double getNodesLength(const vector<const Node*> nodes)
	{
		unordered_map<int, const Node*> nodeIDMap;
		unordered_map<int, std::vector<const Node*>> nodeID2childMap;
		nm::populateMaps(nodes, nodeIDMap, nodeID2childMap);

		double totalLength = 0;
		for (auto& node : nodeID2childMap)
		{
			// [nodes] is most likely cut out from a region. Some nodes' parent nodes may not be in [nodes] because they're in different regions.
			if (nodeIDMap.find(node.first) != nodeIDMap.end())
			{
				const Node* paNode = nodeIDMap.at(node.first);
				for (auto& childNode : node.second)
				{
					double dist = sqrt((paNode->getX() - childNode->getX()) * (paNode->getX() - childNode->getX()) +
						(paNode->getY() - childNode->getY()) * (paNode->getY() - childNode->getY()) +
						(paNode->getZ() - childNode->getZ()) * (paNode->getZ() - childNode->getZ()));
					totalLength += dist;
				}
			}
		}

		return totalLength;
	}

	vector<vector<const Node*>> getSpikeNodes(const Neuron& neuron, double spikeLengthThreshold)
	{
		if (neuron.mNodeIDMap.empty() || neuron.mNodeID2childMap.empty())
		{
			stringstream s("No data in node maps.");
			throw NeuronNodeMapsHaveNoDataException(s);
		}
	
		map<int, pair<vector<const Node*>, double>> spikeMap;
		vector<const Node*> tipNodes = nm::getTipNodes(neuron.getNodes(), neuron.mNodeIDMap, neuron.mNodeID2childMap);
		for (auto& tipNode : tipNodes)
		{
			vector<const Node*> spikeNodes = { tipNode };
			const Node* currNode = tipNode;
			const Node* paNode = neuron.mNodeIDMap.at(currNode->getParentID());
			while (neuron.mNodeID2childMap.at(paNode->getID()).size() == 1)
			{
				currNode = paNode;
				spikeNodes.push_back(currNode);
				paNode = neuron.mNodeIDMap.at(currNode->getParentID());
			}
			spikeNodes.push_back(paNode);

			double length = nm::getNodesLength(spikeNodes) * 25;
			if (length <= spikeLengthThreshold)
			{
				for (auto& spike : spikeMap)
				{
					if ((*(spike.second.first.end() - 1))->getID() == (*(spikeNodes.end() - 1))->getID())
					{
						if (spike.second.second > length)
						{
							spikeMap.erase(spikeMap.find(spike.first));
							spikeMap.emplace((*spikeNodes.begin())->getID(), pair<vector<const Node*>, double>(spikeNodes, length));
							break;
						}
						else
							goto NEXT_TIP;
					}
				}
				spikeMap.emplace((*spikeNodes.begin())->getID(), pair<vector<const Node*>, double>(spikeNodes, length));
			}

		NEXT_TIP:
			continue;
		}

		vector<vector<const Node*>> outputSpikes;
		for (auto& spike : spikeMap)
		{
			vector<const Node*> spikeNodes;
			spikeNodes.insert(spikeNodes.begin(), spike.second.first.begin(), spike.second.first.end() - 1);
			outputSpikes.push_back(spikeNodes);
		}

		return outputSpikes;
	}
}