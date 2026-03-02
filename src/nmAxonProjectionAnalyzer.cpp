#include "nmAxonProjectionAnalyzer.h"
#include "nmUtilities.h"

#include <filesystem>
#include <iostream>
#include <unordered_set>
#include <algorithm>

using namespace std;

namespace nm
{

	void AxonProjectionAnalyzer::getSWCFilePathsFromDirectory(const std::string& directoryPath)
	{
		for (auto& entry : std::filesystem::directory_iterator(directoryPath))
		{
			if (entry.path().extension() == ".swc")
				mSWCFilePaths.push_back(entry.path().string());
		}
	}

	void AxonProjectionAnalyzer::batchComputeTargetRegionLengths()
	{
		atomic<size_t> nextFileIndex(0);
		size_t numThreads = thread::hardware_concurrency();
		vector<thread> workers;

		for (size_t t = 0; t < numThreads; ++t)
			workers.emplace_back(&AxonProjectionAnalyzer::targetRegionLengthWorker, this, ref(nextFileIndex));
		
		for (auto& worker : workers)
			worker.join();
	}

	void AxonProjectionAnalyzer::targetRegionLengthWorker(atomic<size_t>& nextFileIndex)
	{
		while (true)
		{
			size_t fileIndex = nextFileIndex.fetch_add(1);
			if (fileIndex >= mSWCFilePaths.size())
				break;

			Neuron neuron(mSWCFilePaths[fileIndex]);
			neuron.scale(0.04); // Scale the neuron to match CCF resolution
			populateAxonSubregionMaps(neuron);
			populateAxonTargetRegionMaps(neuron);
			populateAxonTargetRegionLengths(neuron);

			lock_guard<mutex> lock(mReportMutex);
			for (auto& taRegion : neuron.mAxonTargetRegionLengthMap)
				mTargetReport[neuron.getNeuronName()][taRegion.first] = taRegion.second;
			for (auto& taRegion : neuron.mL_AxonTargetRegionLengthMap)
				mTargetReport_L[neuron.getNeuronName()][taRegion.first] = taRegion.second;
			for (auto& taRegion : neuron.mR_AxonTargetRegionLengthMap)
				mTargetReport_R[neuron.getNeuronName()][taRegion.first] = taRegion.second;
		}
	}

	void AxonProjectionAnalyzer::populateAxonSubregionMaps(Neuron& neuron)
	{
		if (!mRemoveSpikes)
		{
			const vector<Node>& nodes = neuron.getNodes();
			for (size_t i = 0; i < nodes.size(); ++i)
			{
				const Node& node = nodes[i];
				if (nodes[i].getType() == 2)
				{
					string regionName = mCCF.getRegionNameByNode(node);
					if (!regionName.compare("Out of bounds") || !regionName.compare("Unknown region"))
						continue;
					neuron.mAxonSubregionNodeMap[regionName].push_back(&node);
				}
			}
		}
		else
		{
			vector<vector<const Node*>> spikeNodes = nm::getSpikeNodes(neuron, mSpikeLengthThreshold);
			/*cout << neuron.getNeuronName() << ": ";
			for (auto& spike : spikeNodes)
				cout << spike.size() << " ";
			cout << endl;*/
			
			vector<const Node*> nodes;
			for (auto& node : neuron.getNodes())
				nodes.push_back(&node);

			for (auto& spike : spikeNodes)
			{
				unordered_set<const Node*> toRemove(spike.begin(), spike.end());
				nodes.erase(remove_if(nodes.begin(), nodes.end(), [&](const Node* node) { return toRemove.count(node) > 0; }), nodes.end());
			}

			for (size_t i = 0; i < nodes.size(); ++i)
			{
				if (nodes[i]->getType() == 2)
				{
					string regionName = mCCF.getRegionNameByNode(*nodes[i]);
					if (!regionName.compare("Out of bounds") || !regionName.compare("Unknown region"))
						continue;
					neuron.mAxonSubregionNodeMap[regionName].push_back(nodes[i]);
				}
			}
		}
		//cout << neuron.getNeuronName() << ": " << neuron.mAxonSubregionNodeMap.size() << endl;

		for (auto& subRegion : neuron.mAxonSubregionNodeMap)
		{
			for (auto& node : subRegion.second)
			{
				if (node->getZ() < CCF_MIDDLE_25TH)
					neuron.mL_AxonSubregionNodeMap[subRegion.first].push_back(node);
				else if (node->getZ() > CCF_MIDDLE_25TH)
					neuron.mR_AxonSubregionNodeMap[subRegion.first].push_back(node);
			}
		}
	}

	void AxonProjectionAnalyzer::populateAxonTargetRegionMaps(Neuron& neuron)
	{
		if (neuron.mAxonSubregionNodeMap.empty())
			populateAxonSubregionMaps(neuron);

		for (auto& it : neuron.mAxonSubregionNodeMap)
		{
			string paName = it.first;
			while (paName.compare("root"))
			{
				//cout << paName << endl;
				if (mTargetList.find(paName) != mTargetList.end())
				{
					neuron.mAxonTargetRegionNodeMap[paName].insert(neuron.mAxonTargetRegionNodeMap[paName].end(), it.second.begin(), it.second.end());
					break;
				}
				paName = mCCF.getParentRegionName(paName);
			}
		}
		/*cout << neuron.getNeuronName() << ": ";
		for (auto& paName : neuron.mAxonTargetRegionNodeMap)
			cout << paName.first << " ";
		cout << endl;*/

		for (auto& target : neuron.mAxonTargetRegionNodeMap)
		{
			for (auto& node : target.second)
			{
				if (node->getZ() < CCF_MIDDLE_25TH)
					neuron.mL_AxonTargetRegionNodeMap[target.first].push_back(node);
				else if (node->getZ() > CCF_MIDDLE_25TH)
					neuron.mR_AxonTargetRegionNodeMap[target.first].push_back(node);
			}
		}
	}

	void AxonProjectionAnalyzer::populateAxonTargetRegionLengths(Neuron& neuron)
	{
		if (neuron.mAxonTargetRegionNodeMap.empty())
			populateAxonTargetRegionMaps(neuron);

		/*for (auto& region : neuron.mAxonTargetRegionNodeMap)
		{
			double length = nm::getNodesLength(region.second) * 25;
			neuron.mAxonTargetRegionLengthMap[region.first] = length;
		}*/
		/*cout << neuron.getNeuronName() << ": ";
		for (auto& paName : neuron.mAxonTargetRegionLengthMap)
			cout << paName.first << ":" << paName.second << " ";
		cout << endl;*/

		cout << neuron.getNeuronName() << endl;
		for (auto& region : neuron.mL_AxonTargetRegionNodeMap)
		{
			//double length = nm::getNodesLength(region.second) * 25;
			pair<bool, double> cluster = isTarget(region.second, neuron.mNodeID2childMap);
			if (cluster.first)
			{
				neuron.mL_AxonTargetRegionLengthMap[region.first] = cluster.second;
				cout << region.first << "_L:" << neuron.mL_AxonTargetRegionLengthMap[region.first] << " ";
			}
		}
		cout << endl;
		for (auto& region : neuron.mR_AxonTargetRegionNodeMap)
		{
			//double length = nm::getNodesLength(region.second) * 25;
			pair<bool, double> cluster = isTarget(region.second, neuron.mNodeID2childMap);
			if (cluster.first)
			{
				neuron.mR_AxonTargetRegionLengthMap[region.first] = cluster.second;
				cout << region.first << "_R:" << neuron.mR_AxonTargetRegionLengthMap[region.first] << " ";
			}
		}
		cout << endl;
	}

	pair<bool, double> AxonProjectionAnalyzer::isTarget(const vector<const Node*> nodes, unordered_map<int, vector<const Node*>>& nodeID2childMap)
	{
		int majorBifurCount = 0, majorEndCount = 0, minorBifurCount = 0, minorEndCount = 0;
		double length = nm::getNodesLength(nodes) * 25;
		if (length >= mMajorTargetLengthThreshold)
		{
			for (size_t i = 0; i < nodes.size(); ++i)
			{
				auto it = nodeID2childMap.find(nodes.at(i)->getID());
				if (it == nodeID2childMap.end())
					++majorEndCount;
				else
				{
					if (it->second.size() > 1)
						++majorBifurCount;
				}

				if (majorBifurCount >= mMajorTargetBifurCount && majorEndCount >= mMajorTargetEndCount)
					return pair<bool, double>(true, length);
			}
			return pair<bool, double>(false, length);
		}
		else
		{
			for (size_t i = 0; i < nodes.size(); ++i)
			{
				auto it = nodeID2childMap.find(nodes.at(i)->getID());
				if (it == nodeID2childMap.end())
					++minorEndCount;
				else
				{
					if (it->second.size() > 1)
						++minorBifurCount;
				}

				if (minorBifurCount >= mMinorTargetBifurCount && minorEndCount >= mMinorTargetEndCount)
					return pair<bool, double>(true, length);
			}
			return pair<bool, double>(false, length);
		}
	}

	void AxonProjectionAnalyzer::batchComputeTargetRegionLengths_singleThread()
	{
		for (auto& fileName :mSWCFilePaths)
		{
			Neuron neuron(fileName);
			neuron.scale(0.04); // Scale the neuron to match CCF resolution
			populateAxonSubregionMaps(neuron);
			populateAxonTargetRegionMaps(neuron);
			populateAxonTargetRegionLengths(neuron);

			for (auto& taRegion : neuron.mAxonTargetRegionLengthMap)
				mTargetReport[neuron.getNeuronName()][taRegion.first] = taRegion.second;
			for (auto& taRegion : neuron.mL_AxonTargetRegionLengthMap)
				mTargetReport_L[neuron.getNeuronName()][taRegion.first] = taRegion.second;
			for (auto& taRegion : neuron.mR_AxonTargetRegionLengthMap)
				mTargetReport_R[neuron.getNeuronName()][taRegion.first] = taRegion.second;
		}
	}

}