#include "nmAxonProjectionAnalyzer.h"
#include "nmUtilities.h"

#include <filesystem>
#include <iostream>

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
			for (auto& paRegion : neuron.mAxonTargetRegionLengthMap)
				mTargetReport[neuron.getNeuronName()][paRegion.first] = paRegion.second;
		}
	}

	void AxonProjectionAnalyzer::populateAxonSubregionMaps(Neuron& neuron)
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
				neuron.mAxonSubregionNodeLocMap[regionName].push_back(i);
				//if (neuron.mNodeID2childLocMap.find(node.getID()) == neuron.mNodeID2childLocMap.end())
				//	neuron.mAxonSubregionEndBifurMap[regionName].first++; // Increment end node count
				//else if (neuron.mNodeID2childLocMap[node.getID()].size() > 1)
				//	neuron.mAxonSubregionEndBifurMap[regionName].second++; // Increment bifurcation node count
			}
		}
		//cout << neuron.getNeuronName() << ": " << neuron.mAxonSubregionNodeLocMap.size() << endl;
	}

	void AxonProjectionAnalyzer::populateAxonTargetRegionMaps(Neuron& neuron)
	{
		if (neuron.mAxonSubregionNodeLocMap.empty())
			populateAxonSubregionMaps(neuron);

		for (auto& it : neuron.mAxonSubregionNodeLocMap)
		{
			string paName = it.first;
			while (paName.compare("root"))
			{
				//cout << paName << endl;
				if (mTargetList.find(paName) != mTargetList.end())
				{
					neuron.mAxonTargetRegionNodeLocMap[paName].insert(neuron.mAxonTargetRegionNodeLocMap[paName].end(), it.second.begin(), it.second.end());
					//neuron.mAxonTargetRegionEndBifurMap[paName].first += neuron.mAxonSubregionEndBifurMap[it.first].first;
					//neuron.mAxonTargetRegionEndBifurMap[paName].second += neuron.mAxonSubregionEndBifurMap[it.first].second;
					break;
				}
				paName = mCCF.getParentRegionName(paName);
			}
		}
		/*cout << neuron.getNeuronName() << ": ";
		for (auto& paName : neuron.mAxonTargetRegionNodeLocMap)
			cout << paName.first << " ";
		cout << endl;*/
	}

	void AxonProjectionAnalyzer::populateAxonTargetRegionLengths(Neuron& neuron)
	{
		if (neuron.mAxonTargetRegionNodeLocMap.empty())
			populateAxonTargetRegionMaps(neuron);

		for (auto& region : neuron.mAxonTargetRegionNodeLocMap)
		{
			vector<const Node*> nodes;
			for (auto& loc : region.second)
				nodes.push_back(&neuron.getNodes().at(loc));
			double length = nm::getNodesLength(nodes) * 25;
			neuron.mAxonTargetRegionLengthMap[region.first] = length;
		}
		/*cout << neuron.getNeuronName() << ": ";
		for (auto& paName : neuron.mAxonTargetRegionLengthMap)
			cout << paName.first << ":" << paName.second << " ";
		cout << endl;*/
	}

	/*void AxonProjectionAnalyzer::populateAxonTargetRegionMapsHemisphere(Neuron& neuron)
	{
		if (neuron.mAxonSubregionNodeLocMap.empty())
			populateAxonSubregionMaps(neuron);
		


	}*/

	void AxonProjectionAnalyzer::batchComputeTargetRegionLengths_singleThread()
	{
		for (auto& fileName :mSWCFilePaths)
		{
			Neuron neuron(fileName);
			neuron.scale(0.04); // Scale the neuron to match CCF resolution
			populateAxonSubregionMaps(neuron);
			populateAxonTargetRegionMaps(neuron);
			populateAxonTargetRegionLengths(neuron);
		}
	}

}