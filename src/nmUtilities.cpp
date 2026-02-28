#include "nmUtilities.h"

#include <iostream>

using namespace std;

namespace nm
{

	void populateNodeID2locChildLocMaps(vector<const Node*> nodes, std::unordered_map<int, int>& nodeID2LocMap, std::unordered_map<int, std::vector<int>>& nodeID2childLocMap)
	{
		nodeID2LocMap.clear();
		nodeID2childLocMap.clear();
		nodeID2LocMap.reserve(nodes.size());
		nodeID2childLocMap.reserve(nodes.size());

		for (int i = 0; i < nodes.size(); ++i)
		{
			const Node* node = nodes[i];
			nodeID2LocMap.emplace(node->getID(), i);
			if (node->getParentID() != -1)
				nodeID2childLocMap[node->getParentID()].push_back(i);
		}
	}

	double getNodesLength(vector<const Node*> nodes)
	{
		unordered_map<int, int> nodeID2LocMap;
		unordered_map<int, std::vector<int>> nodeID2childLocMap;
		nm::populateNodeID2locChildLocMaps(nodes, nodeID2LocMap, nodeID2childLocMap);

		double totalLength = 0;
		for (auto& node : nodeID2childLocMap)
		{
			if (nodeID2LocMap.find(node.first) != nodeID2LocMap.end())
			{
				const Node* paNode = nodes.at(nodeID2LocMap.at(node.first));
				for (auto& childLoc : node.second)
				{
					const Node* childNode = nodes.at(childLoc);
					double dist = sqrt((paNode->getX() - childNode->getX()) * (paNode->getX() - childNode->getX()) +
						(paNode->getY() - childNode->getY()) * (paNode->getY() - childNode->getY()) +
						(paNode->getZ() - childNode->getZ()) * (paNode->getZ() - childNode->getZ()));
					totalLength += dist;
				}
			}
		}

		return totalLength;
	}
}