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
			}
		}
		//cout << neuron.getNeuronName() << ": " << neuron.mAxonSubregionNodeLocMap.size() << endl;

		for (auto& subRegion : neuron.mAxonSubregionNodeLocMap)
		{
			for (auto& loc : subRegion.second)
			{
				if (nodes[loc].getZ() < CCF_MIDDLE_25TH)
					neuron.mL_AxonSubregionNodeLocMap[subRegion.first].push_back(loc);
				else if (nodes[loc].getZ() > CCF_MIDDLE_25TH)
					neuron.mR_AxonSubregionNodeLocMap[subRegion.first].push_back(loc);
			}
		}
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
					break;
				}
				paName = mCCF.getParentRegionName(paName);
			}
		}
		/*cout << neuron.getNeuronName() << ": ";
		for (auto& paName : neuron.mAxonTargetRegionNodeLocMap)
			cout << paName.first << " ";
		cout << endl;*/

		const vector<Node>& nodes = neuron.getNodes();
		for (auto& target : neuron.mAxonTargetRegionNodeLocMap)
		{
			for (auto& loc : target.second)
			{
				if (nodes[loc].getZ() < CCF_MIDDLE_25TH)
					neuron.mL_AxonTargetRegionNodeLocMap[target.first].push_back(loc);
				else if (nodes[loc].getZ() > CCF_MIDDLE_25TH)
					neuron.mR_AxonTargetRegionNodeLocMap[target.first].push_back(loc);
			}
		}
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

		cout << neuron.getNeuronName() << endl;
		for (auto& region : neuron.mL_AxonTargetRegionNodeLocMap)
		{
			vector<const Node*> nodes;
			for (auto& loc : region.second)
				nodes.push_back(&neuron.getNodes().at(loc));
			double length = nm::getNodesLength(nodes) * 25;
			neuron.mL_AxonTargetRegionLengthMap[region.first] = length;
			cout << region.first << "_L:" << neuron.mL_AxonTargetRegionLengthMap[region.first] << " ";
		}
		cout << endl;
		for (auto& region : neuron.mR_AxonTargetRegionNodeLocMap)
		{
			vector<const Node*> nodes;
			for (auto& loc : region.second)
				nodes.push_back(&neuron.getNodes().at(loc));
			double length = nm::getNodesLength(nodes) * 25;
			neuron.mR_AxonTargetRegionLengthMap[region.first] = length;
			cout << region.first << "_R:" << neuron.mR_AxonTargetRegionLengthMap[region.first] << " ";
		}
		cout << endl;
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
		}
	}

}