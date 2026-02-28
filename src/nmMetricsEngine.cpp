#include "nmMetricsEngine.h"

#include <iostream>
#include <fstream>

using namespace std;

namespace nm
{

	void MetricsEngine::setSWCFilesDirectory(const string& directoryPath)
	{
		mAxonProjAnalyzer.getSWCFilePathsFromDirectory(directoryPath);
	}

	void MetricsEngine::setTargetRegionList()
	{
		mAxonProjAnalyzer.setTargetList(targetRegionsWithLayerNums);
		mTargetList = targetRegionsWithLayerNums;
	}

	void MetricsEngine::outputAxonTargetReport(const string& outputFolder, const string& fileBaseName)
	{
		string outputRTargetFileName = outputFolder + "\\" + fileBaseName + "_R.csv";
		string outputLTargetFileName = outputFolder + "\\" + fileBaseName + "_L.csv";
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

		mAxonProjAnalyzer.batchComputeTargetRegionLengths();

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