#include "nmNeuron.h"
#include "nmExceptions.h"
#include "nmUtilities.h"

#include <boost\algorithm\string.hpp>

#include <iostream>
#include <fstream>
#include <ctime>

using namespace std;

namespace nm
{

    Neuron::Neuron(const string& filePath)
    {
        readSWCFile(filePath);
        populateNodeMaps();
    }

    void Neuron::readSWCFile(const std::string& filePath)
    {
        std::ifstream file(filePath);
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

    void Neuron::populateNodeMaps()
    {
		// Tip nodes won't be in mNodeID2childLocMap, but that's fine since we only use that map to find children of a node, and tip nodes don't have children
		// If mNodeID2childLocMap cannot find a node ID, it means that node is a tip node and we can handle that case accordingly in the code that uses the map

        //mNodeID2LocMap.clear();
        //mNodeID2childLocMap.clear();
        //mNodeID2LocMap.reserve(mNodes.size());
        //mNodeID2childLocMap.reserve(mNodes.size());
        mNodeIDMap.clear();
        mNodeID2childMap.clear();
        mNodeIDMap.reserve(mNodes.size());
        mNodeID2childMap.reserve(mNodes.size());

        /*for (int i = 0; i < mNodes.size(); ++i)
        {
		    const Node& node = mNodes[i];
            mNodeID2LocMap.emplace(node.getID(), i);
            if (node.getParentID() != -1)
                mNodeID2childLocMap[node.getParentID()].push_back(i);
        }*/
        for (vector<Node>::const_iterator it = mNodes.begin(); it != mNodes.end(); ++it)
        {
            mNodeIDMap.emplace(it->getID(), &*it);
            if (it->getParentID() != -1)
                mNodeID2childMap[it->getParentID()].push_back(&*it);
        }
    }

    void Neuron::scale(double scaleFactor)
    {
        for (Node& node : mNodes)
            node = Node(node.getX() * scaleFactor, node.getY() * scaleFactor, node.getZ() * scaleFactor, node.getID(), node.getParentID(), node.getType(), node.getRadius());
    }

}