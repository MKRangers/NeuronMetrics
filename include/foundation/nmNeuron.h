#ifndef nmNEURON_H
#define nmNEURON_H

#include "nmSegment.h"

#include <boost\container\flat_map.hpp>

#include <string>
#include <unordered_map>
#include <memory>

namespace nm
{

	class Neuron
	{
		public:
			Neuron() = default;
			Neuron(const std::string& filePath);
			Neuron(const std::vector<Node>& nodes) : mNodes(nodes) { populateNodeMaps(); }
			~Neuron() = default;

			void					 readSWCFile(const std::string& filePath);
			void					 scale(double scaleFactor);
			std::string				 getNeuronName() const { return mName; }	
			const std::vector<Node>& getNodes() const { return mNodes; }
			int						 getNodesNum() const { return mNodes.size(); }

			void populateNodeMaps();  // Call this after mNodes is populated to fill the maps for quick access
			std::unordered_map<int, const Node*> mNodeIDMap;                    // Node ID -> the ptr to the node in mNodes
			std::unordered_map<int, std::vector<const Node*>> mNodeID2childMap; // Node ID -> a vector of ptrs to its child nodes in mNodes

			boost::container::flat_map<std::string, std::vector<const Node*>> mAxonSubregionNodeMap;      // region name -> vector of ptrs to the nodes in mNodes that are axon nodes in that region
			boost::container::flat_map<std::string, std::vector<const Node*>> mAxonTargetRegionNodeMap;   // region name -> vector of ptrs to the nodes in mNodes that are axon nodes in that TARGET region
			boost::container::flat_map<std::string, double>                   mAxonTargetRegionLengthMap; // region name -> axon length in that region

			boost::container::flat_map<std::string, std::vector<const Node*>> mR_AxonSubregionNodeMap;
			boost::container::flat_map<std::string, std::vector<const Node*>> mR_AxonTargetRegionNodeMap;
			boost::container::flat_map<std::string, double>                   mR_AxonTargetRegionLengthMap;

			boost::container::flat_map<std::string, std::vector<const Node*>> mL_AxonSubregionNodeMap;       
			boost::container::flat_map<std::string, std::vector<const Node*>> mL_AxonTargetRegionNodeMap;
			boost::container::flat_map<std::string, double>                   mL_AxonTargetRegionLengthMap;


		private:
			std::string	mFilePath;
			std::string mName;

			std::vector<Node>	 mNodes;
			std::vector<Segment> mSegments;
	};

}

#endif