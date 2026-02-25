#ifndef nmNEURON_H
#define nmNEURON_H

#include "nmSegment.h"

#include <string>
#include <map>

namespace nm
{

	class Neuron
	{
		public:
			Neuron() = default;
			Neuron(const std::string& filePath);
			Neuron(const std::vector<Node>& nodes) : mNodes(nodes) { populateNodeMaps(); }
			~Neuron() = default;

			void readSWCFile(const std::string& filePath);
			void populateNodeMaps(); // Call this after mNodes is populated to fill the maps for quick access
			void scaleNeuron(double scaleFactor);

			std::vector<Node>		mNodes;
			std::vector<Segment>	mSegments;

			std::unordered_map<int, int>			  mNodeID2LocMap;      // Node ID -> its location in mNodes
			std::unordered_map<int, std::vector<int>> mNodeID2childLocMap; // Node ID -> a vector of locations of its child nodes in mNodes


		private:
			std::string	mFilePath;
			std::string mName;
	};

}

#endif