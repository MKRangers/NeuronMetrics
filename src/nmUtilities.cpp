#include "nmUtilities.h"
#include "nmExceptions.h"

#include <iostream>
#include <map>
#include <unordered_set>

using namespace std;

namespace nm
{

	int getNodeIDMax(const vector<Node>& nodes)
	{
		int maxID = -1;
		for (auto& node : nodes)
		{
			if (node.getID() > maxID)
				maxID = node.getID();
		}
		return maxID;
	}

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
			stringstream s("No data in input node maps.");
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
		if (neuron.mNodeIDMap.empty() || neuron.mNodeID2ChildMap.empty())
		{
			stringstream s("No data in node maps of input neuron.");
			throw NeuronNodeMapsHaveNoDataException(s);
		}
	
		map<int, pair<vector<const Node*>, double>> spikeMap;
		vector<const Node*> tipNodes = nm::getTipNodes(neuron.getNodes(), neuron.mNodeIDMap, neuron.mNodeID2ChildMap);
		for (auto& tipNode : tipNodes)
		{
			vector<const Node*> spikeNodes = { tipNode };
			const Node* currNode = tipNode;
			const Node* paNode = neuron.mNodeIDMap.at(currNode->getParentID());
			while (neuron.mNodeID2ChildMap.at(paNode->getID()).size() == 1)
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

	vector<Node> interpolateNodes(Neuron& neuron, double interval)
	{
		vector<Node> outputNodes;
		if (neuron.mNodeIDMap.empty() || neuron.mNodeID2ChildMap.empty())
		{
			try
			{
				neuron.populateNodeMaps();
			}
			catch (const NeuronHasNoNodesException& e)
			{
				cout << e.what() << " No nodes in neuron to interpolate.";
				return outputNodes;
			}
		}

		unordered_map<int, int> id2NewPaIDMap; // old parent node ID -> new parent node ID after interpolation, used for updating parent IDs of child nodes after new nodes are inserted between the old parent and child nodes
		unordered_map<int, size_t> paNodeLocMap;
		vector<const Node*> tipNodes = nm::getTipNodes(neuron.getNodes(), neuron.mNodeIDMap, neuron.mNodeID2ChildMap);
		int idMax = nm::getNodeIDMax(neuron.getNodes());
		for (auto& it : neuron.mNodeID2ChildMap)
		{
			const Node* paNode = neuron.mNodeIDMap.at(it.first);
			outputNodes.push_back(*paNode);
			paNodeLocMap[paNode->getID()] = outputNodes.size() - 1;
			for (auto& childNode : it.second)
			{
				double dist = sqrt((paNode->getX() - childNode->getX()) * (paNode->getX() - childNode->getX()) +
								   (paNode->getY() - childNode->getY()) * (paNode->getY() - childNode->getY()) +
								   (paNode->getZ() - childNode->getZ()) * (paNode->getZ() - childNode->getZ()));
				if (dist > interval)
				{
					int numNodesToInsert = static_cast<int>(floor(dist / interval));
					numNodesToInsert = (static_cast<int>(dist) % static_cast<int>(interval) == 0) ? --numNodesToInsert : numNodesToInsert; // if the distance is an exact multiple of the interval, we need to insert one less node
					double xStep = (childNode->getX() - paNode->getX()) / (numNodesToInsert + 1);
					double yStep = (childNode->getY() - paNode->getY()) / (numNodesToInsert + 1);
					double zStep = (childNode->getZ() - paNode->getZ()) / (numNodesToInsert + 1);
					int prevID = paNode->getID();
					for (int i = 1; i <= numNodesToInsert; ++i)
					{
						outputNodes.emplace_back(paNode->getX() + i * xStep, paNode->getY() + i * yStep, paNode->getZ() + i * zStep, ++idMax, prevID, childNode->getType());
						prevID = outputNodes.back().getID();
					}
					id2NewPaIDMap[childNode->getID()] = prevID;

					// If the child node is a tip node, we need to update its parent ID to the last new node inserted between the old parent and child nodes. 
					if (find(tipNodes.begin(), tipNodes.end(), childNode) != tipNodes.end())
						outputNodes.emplace_back(childNode->getX(), childNode->getY(), childNode->getZ(), childNode->getID(), prevID, childNode->getType());
				}
			}
		}

		for (auto& it : id2NewPaIDMap)
		{
			if (paNodeLocMap.find(it.first) != paNodeLocMap.end())
			{
				outputNodes[paNodeLocMap[it.first]].setParentID(it.second);
			}
		}

		return outputNodes;
	}

}