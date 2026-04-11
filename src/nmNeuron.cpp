#include "nmNeuron.h"
#include "nmExceptions.h"
#include "nmUtilities.h"

#include <boost\algorithm\string.hpp>

#include <iostream>
#include <fstream>

using namespace std;

namespace nm
{

    Neuron::Neuron(const string& filePath, bool populateMap)
    {
        readSWCFile(filePath);
		if (populateMap)
			populateNodeMaps();
    }

    Neuron::Neuron(const vector<Node>& nodes, bool populateMap) : mNodes(nodes)
    {
		if (populateMap)
            populateNodeMaps();
	}

    void Neuron::readSWCFile(const std::string& filePath)
    {
        ifstream file(filePath);
        if (!file)
            throw FileNotFoundException(filePath);
        mFilePath = filePath;
        vector<string> pathItems;
        boost::split(pathItems, mFilePath, boost::is_any_of("\\"));
        mName = pathItems.back();
        //cout << "Reading SWC file: " << mName << " - ";

        string testLine;
        bool isSpaceSeparated = false;
        while (getline(file, testLine))
        {
            if (testLine.empty() || testLine[0] == '#')
                continue;

            vector<string> lineItems;
            boost::split(lineItems, testLine, boost::is_any_of(" "));
            if (lineItems.size() == 7)
            {
                isSpaceSeparated = true;
                break;
            }
        }

        mNodes.clear();
        file.clear();
        file.seekg(0, ios::beg); // Reset file stream to the beginning
        string line;
        if (isSpaceSeparated)
        {
            while (getline(file, line))
            {
                if (line.empty() || line[0] == '#')
                    continue;

                vector<string> lineItems;
                boost::split(lineItems, line, boost::is_any_of(" "));
                mNodes.emplace_back(stod(lineItems[2]), stod(lineItems[3]), stod(lineItems[4]), stoi(lineItems[0]), stoi(lineItems[6]), stoi(lineItems[1]), stod(lineItems[5]));
            }
        }
        else
        {
            while (getline(file, line))
            {
                if (line.empty() || line[0] == '#')
                    continue;

                vector<string> lineItems;
                boost::split(lineItems, line, boost::is_any_of("\t"));
                mNodes.emplace_back(stod(lineItems[2]), stod(lineItems[3]), stod(lineItems[4]), stoi(lineItems[0]), stoi(lineItems[6]), stoi(lineItems[1]), stod(lineItems[5]));
            }
        }

        //cout << mNodes.size() << " nodes read." << endl;
    }

    void Neuron::writeSWCFile(const string& filePath) const
    {
        ofstream file(filePath);
        if (!file)
            throw FileNotFoundException(filePath);
        for (const Node& node : mNodes)
            file << node.getID() << " " << node.getType() << " " << node.getX() << " " << node.getY() << " " << node.getZ() << " " << node.getRadius() << " " << node.getParentID() << endl;
        file.close();
	}

    void Neuron::populateNodeMaps()
    {
		// Tip nodes won't be in mNodeID2childLocMap, but that's fine since we only use that map to find children of a node, and tip nodes don't have children
		// If mNodeID2childLocMap cannot find a node ID, it means that node is a tip node and we can handle that case accordingly in the code that uses the map

        if (mNodes.empty())
        {
            stringstream s("No nodes in neuron.");
            throw NeuronHasNoNodesException(s);
		}

        mNodeIDMap.clear();
        mNodeID2ChildMap.clear();
        mNodeIDMap.reserve(mNodes.size());
        mNodeID2ChildMap.reserve(mNodes.size());

        for (vector<Node>::const_iterator it = mNodes.begin(); it != mNodes.end(); ++it)
        {
            mNodeIDMap.emplace(it->getID(), &*it);
            if (it->getParentID() != -1)
                mNodeID2ChildMap[it->getParentID()].push_back(&*it);
        }
    }

    void Neuron::scale(double scaleFactor)
    {
        for (Node& node : mNodes)
            node = Node(node.getX() * scaleFactor, node.getY() * scaleFactor, node.getZ() * scaleFactor, node.getID(), node.getParentID(), node.getType(), node.getRadius());
    }

    void Neuron::writeSegmentsToSWC(const string& filePath) const
    {
        ofstream file(filePath);
        if (!file)
            throw FileNotFoundException(filePath);
        
        for (const Segment& seg : mSegments)
        {
            for (const Node* node : seg.nodes)
                file << node->getID() << " " << node->getType() << " " << node->getX() << " " << node->getY() << " " << node->getZ() << " " << node->getRadius() << " " << node->getParentID() << endl;
        }
        file.close();
    }

    void Neuron::populateSegments()
    {
        if (mNodes.empty())
        {
            stringstream s("No nodes in neuron.");
            throw NeuronHasNoNodesException(s);
        }

        if (mNodeID2ChildMap.empty())
			populateNodeMaps();

        mSegments.clear();
        int segmentID = 0;
        for (auto& it : mNodeID2ChildMap)
        {
            if (it.second.size() > 1 || mNodeIDMap.at(it.first)->getParentID() == -1)
            {
                vector<Segment> segments = buildSegmentFromNode(*mNodeIDMap.at(it.first));
                for (Segment& segment : segments)
                {
                    segment.id = segmentID++;
                    mSegments.push_back(segment);
                }
            }
        }

        // Set parent and child segments
        populateSegmentMaps();
        for (Segment& segment : mSegments)
        {
			const Node* headNode = segment.nodes.front();
            if (headNode->getParentID() != -1)
            {
				vector<Segment*> segmentsSharingSameNode = mNodeID2SegmentMap.at(headNode->getID());
                for (Segment* candidateParentSegment : segmentsSharingSameNode)
                {
                    if (candidateParentSegment->nodes.back()->getID() == headNode->getID())
                    {
                        segment.parentSegment = candidateParentSegment;
                        candidateParentSegment->childSegments.push_back(&segment);
                        break;
                    }
				}
            }
		}
    }

    vector<Neuron::Segment> Neuron::buildSegmentFromNode(const Node& node)
    {
		vector<Segment> segments;
        const Node* inputNode = &node;
        for (auto& childNode : mNodeID2ChildMap.at(inputNode->getID()))
        {
            Segment segment;
			segment.nodes.push_back(const_cast<Node*>(inputNode)); // const_cast is needed because the nodes in mSegments need to be non-const, but the nodes in mNodes are const when accessed through mNodeIDMap

			const Node* currentNode = childNode;
            while (currentNode)
            {
                segment.nodes.push_back(const_cast<Node*>(currentNode)); // const_cast is needed because the nodes in mSegments need to be non-const, but the nodes in mNodes are const when accessed through mNodeIDMap
                if (mNodeID2ChildMap.find(currentNode->getID()) == mNodeID2ChildMap.end() || mNodeID2ChildMap.at(currentNode->getID()).size() > 1) // if the parent node is a bifurcation point or the root node
                    break;
                currentNode = mNodeID2ChildMap.at(currentNode->getID()).front(); // move to the next node in the segment, which is the only child node of the current node since we break if there are more than 1 child nodes
            }
			segments.push_back(segment);
        }
        return segments;
	}

    void Neuron::populateSegmentMaps()
    {
        if (mSegments.empty())
        {
            stringstream s("No segments in neuron.");
            throw NeuronHasNoSegmentsException(s);
        }

        mNodeID2SegmentMap.clear();
        mNodeID2SegmentMap.reserve(mSegments.size());
        for (Segment& segment : mSegments)
        {
            for (Node* node : segment.nodes)
            {
                if (!mNodeID2SegmentMap.insert({ node->getID(), {&segment} }).second)
					mNodeID2SegmentMap[node->getID()].push_back(&segment);
            }
        }
	}

}