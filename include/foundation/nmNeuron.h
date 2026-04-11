#ifndef nmNEURON_H
#define nmNEURON_H

#include "nmNode.h"

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
			Neuron(const std::string& filePath, bool populateMap = true);
			Neuron(const std::vector<Node>& nodes, bool populateMap = true);
			~Neuron() = default;

			void					 readSWCFile(const std::string& filePath);
			void					 writeSWCFile(const std::string& filePath) const;
			void					 scale(double scaleFactor);
			std::string				 getNeuronName() const { return mName; }	
			const std::vector<Node>& getNodes() const { return mNodes; }
			int						 getNodesNum() const { return mNodes.size(); }

			void populateNodeMaps();  // Call this after mNodes is populated to fill the maps for quick access
			std::unordered_map<int, const Node*> mNodeIDMap;                    // Node ID -> the ptr to the node in mNodes
			std::unordered_map<int, std::vector<const Node*>> mNodeID2ChildMap; // Node ID -> a vector of ptrs to its child nodes in mNodes

			boost::container::flat_map<std::string, std::vector<const Node*>> mAxonSubregionNodeMap;      // region name -> vector of ptrs to the nodes in mNodes that are axon nodes in that region
			boost::container::flat_map<std::string, std::vector<const Node*>> mAxonTargetRegionNodeMap;   // region name -> vector of ptrs to the nodes in mNodes that are axon nodes in that TARGET region
			boost::container::flat_map<std::string, double>                   mAxonTargetRegionLengthMap; // region name -> axon length in that region

			boost::container::flat_map<std::string, std::vector<const Node*>> mR_AxonSubregionNodeMap;
			boost::container::flat_map<std::string, std::vector<const Node*>> mR_AxonTargetRegionNodeMap;
			boost::container::flat_map<std::string, double>                   mR_AxonTargetRegionLengthMap;

			boost::container::flat_map<std::string, std::vector<const Node*>> mL_AxonSubregionNodeMap;       
			boost::container::flat_map<std::string, std::vector<const Node*>> mL_AxonTargetRegionNodeMap;
			boost::container::flat_map<std::string, double>                   mL_AxonTargetRegionLengthMap;

			//!
			//! A segment is defined as a sequence of nodes between two bifurcation points or between a bifurcation point and an end point
			struct Segment
			{
				int id; // segment ID, can be assigned in the order of segments being created
				int type;
				std::vector<Node*> nodes;	
				Segment* parentSegment = nullptr; // the segment that this segment branches out from, null if this segment is the root segment
				std::vector<Segment*> childSegments; // child segments that branch out from the end of this segment
				
				Node* operator[](size_t index) { return nodes[index]; }
				Node* front() { return nodes.front(); }
				Node* back() { return nodes.back(); }
				void pop_front() { nodes.pop_back(); }
				void pop_back() { nodes.erase(nodes.begin()); }
				void push_front(Node* node) { nodes.insert(nodes.begin(), node); }
				void push_back(Node* node) { nodes.push_back(node); }
				//Segment split(size_t index);
			};

			void populateSegments(); // Call this after mNodes is populated to fill mSegments based on root nodes, bifurcations, and end points in the neurons
			std::vector<Segment> buildSegmentFromNode(const Node& node); // Build a segment starting from the input node and ending at the next bifurcation point or end point
			void writeSegmentsToSWC(const std::string& filePath) const;
			std::vector<Segment> mSegments;

			void populateSegmentMaps(); // Call this after mSegments is populated to fill the maps for quick access
			std::unordered_map<int, std::vector<Segment*>> mNodeID2SegmentMap; // Node ID -> the segment that starts from that node (i.e. the segment that has that node as its last node)


		private:
			std::string	mFilePath;
			std::string mName;

			std::vector<Node> mNodes; 
	};

}

#endif