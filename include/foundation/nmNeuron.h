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
			std::unordered_map<int, int>			  mNodeID2LocMap;      // Node ID -> its location in mNodes
			std::unordered_map<int, std::vector<int>> mNodeID2childLocMap; // Node ID -> a vector of locations of its child nodes in mNodes

			boost::container::flat_map<std::string, std::vector<int>>    mAxonSubregionNodeLocMap;     // region name -> vector of node locations in mNodes that are axon nodes in that region
			boost::container::flat_map<std::string, std::vector<int>>    mAxonTargetRegionNodeLocMap;  // region name -> vector of node locations in mNodes that are axon nodes in that TARGET region
			boost::container::flat_map<std::string, double>              mAxonTargetRegionLengthMap;   // region name -> axon length in that region

			boost::container::flat_map<std::string, std::vector<int>>    mR_AxonSubregionNodeLocMap;       
			boost::container::flat_map<std::string, std::vector<int>>    mR_AxonTargetRegionNodeLocMap;
			boost::container::flat_map<std::string, double>              mR_AxonTargetRegionLengthMap;

			boost::container::flat_map<std::string, std::vector<int>>    mL_AxonSubregionNodeLocMap;        
			boost::container::flat_map<std::string, std::vector<int>>    mL_AxonTargetRegionNodeLocMap;
			boost::container::flat_map<std::string, double>              mL_AxonTargetRegionLengthMap;


		private:
			std::string	mFilePath;
			std::string mName;

			std::vector<Node>	 mNodes;
			std::vector<Segment> mSegments;
	};

}

#endif