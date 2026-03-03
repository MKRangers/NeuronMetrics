#include "nmMetricsEngine.h"
#include "nmExceptions.h"

#include <iostream>
#include <fstream>
#include <filesystem>

using namespace std;

namespace nm
{

	void MetricsEngine::setSWCFilesDirectory(const string& directoryPath)
	{
		if (!(filesystem::exists(directoryPath) && filesystem::is_directory(directoryPath)))
			throw runtime_error("Invalid SWC files directory: " + directoryPath);
		mAxonProjAnalyzer.getSWCFilePathsFromDirectory(directoryPath);
	}

	void MetricsEngine::setTargetRegionList(const string& targetRegionListFile)
	{
		if (!targetRegionListFile.empty())
		{
			ifstream inFile(targetRegionListFile);
			if (!inFile)
				throw FileNotFoundException(targetRegionListFile);

			boost::container::flat_set<string> targetRegions;
			string line;
			while (getline(inFile, line))
			{
				if (!line.empty())
					targetRegions.insert(line);
			}
			inFile.close();
			mAxonProjAnalyzer.setTargetList(targetRegions);
			mTargetList = targetRegions;
			return;
		}
		else
		{
			mAxonProjAnalyzer.setTargetList(targetRegionsWithLayerNums);
			mTargetList = targetRegionsWithLayerNums;
		}
	}

	void MetricsEngine::outputAxonTargetReport(const string& outputFolder, bool multiThread)
	{
		if (!(filesystem::exists(outputFolder) && filesystem::is_directory(outputFolder)))
			filesystem::create_directories(outputFolder);

		string outputRTargetFileName = outputFolder + "\\targetReport_R.csv";
		string outputLTargetFileName = outputFolder + "\\targetReport_L.csv";
		ofstream outFileR(outputRTargetFileName);
		ofstream outFileL(outputLTargetFileName);

		outFileR << "file name,";
		outFileL << "file name,";
		for (auto& region : mTargetList)
		{
			outFileR << region << ",";
			outFileL << region << ",";
		}
		outFileR << endl;
		outFileL << endl;

		if (multiThread && thread::hardware_concurrency() > 0)
			mAxonProjAnalyzer.batchComputeTargetRegionLengths();
		else
			mAxonProjAnalyzer.batchComputeTargetRegionLengths_singleThread();

		for (auto& cell : mAxonProjAnalyzer.mTargetReport_R)
		{
			outFileR << cell.first << ",";
			for (auto& region : mTargetList)
			{
				if (cell.second.find(region) != cell.second.end())
					outFileR << cell.second[region] << ",";
				else
					outFileR << 0 << ",";
			}
			outFileR << endl;
		}

		for (auto& cell : mAxonProjAnalyzer.mTargetReport_L)
		{
			outFileL << cell.first << ",";
			for (auto& region : mTargetList)
			{
				if (cell.second.find(region) != cell.second.end())
					outFileL << cell.second[region] << ",";
				else
					outFileL << 0 << ",";
			}
			outFileL << endl;
		}

		outFileR.close();
		outFileL.close();
	}
	
}