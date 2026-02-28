#include "nmNeuron.h"
#include "nmMouseCCF.h"
#include "nmMetricsEngine.h"
#include "nmAxonProjectionAnalyzer.h"

#include <boost/container/flat_set.hpp>
#include <boost\algorithm\string.hpp>

#include <iostream>
#include <ctime>
#include <thread>
#include <set>

using namespace std;

int main()
{
	/*string testSWCFilePath = "D:\\Allen Institute Work\\Connectome_refinement\\_SWC_DATASET\\2873Cells_SWCfiles\\220332_4623-X12025-Y17498_reg_xy25z25_xy0z0.swc";
	clock_t startTime = clock();
	nm::Neuron neuron(testSWCFilePath);
	clock_t endTime = clock();
	cout << float(endTime - startTime) / CLOCKS_PER_SEC << " seconds" << endl;
	neuron.scale(0.04);

	nm::MouseCCF mouseCCF;
	mouseCCF.readTIFFFile("D:\\Arboreta_analysis\\Arboreta_anaysis\\data\\BrainTemplates\\annotation_25_float32.tif");
	string intensityRegionMappingFilePath = "D:\\Arboreta_analysis\\Arboreta_anaysis\\data\\BrainTemplates\\idValue2regionName.txt";
	mouseCCF.setIntensityRegionMapping(intensityRegionMappingFilePath);
	cout << "Region name for node 0: " << mouseCCF.getRegionName(neuron.getNodes()[10000]) << endl;*/
	//nm::MetricsEngine engine;
	//cout << "CCF dimensions: " << engine.mCCF.getWidth() << " x " << engine.mCCF.getHeight() << " x " << engine.mCCF.getDepth() << endl;
	//cout << "Region name for node 0: " << engine.mCCF.getRegionName(neuron.getNodes()[0]) << endl;

	//cout << thread::hardware_concurrency() << " concurrent threads supported." << endl;
	/*clock_t startTime2 = clock();
	engine.test_readAllSWCFiles_multiThreads("D:\\Allen Institute Work\\Connectome_refinement\\_SWC_DATASET\\2873Cells_SWCfiles\\");
	clock_t endTime2 = clock();
	cout << float(endTime2 - startTime2) / CLOCKS_PER_SEC << " seconds" << endl;*/

	/*nm::AxonProjectionAnalyzer analyzer;
	clock_t startTime2 = clock();
	analyzer.populateNeuronAxonRegionMaps("D:\\Allen Institute Work\\Connectome_refinement\\_SWC_DATASET\\2873Cells_SWCfiles\\");
	clock_t endTime2 = clock();
	cout << float(endTime2 - startTime2) / CLOCKS_PER_SEC << " seconds" << endl;*/

	//nm::AxonProjectionAnalyzer analyzer;
	//analyzer.getSWCFilePathsFromDirectory("D:\\Allen Institute Work\\Connectome_refinement\\_SWC_DATASET\\2873Cells_SWCfiles\\");
	//analyzer.setTargetList(targetRegionsWithLayerNums);
	//clock_t time1 = clock();
	//analyzer.batchComputeTargetRegionLengths();
	////analyzer.computeParentRegionLengths_singleThread();
	//clock_t time2 = clock();
	//cout << float(time2 - time1) / CLOCKS_PER_SEC << " seconds" << endl;
	/*for (auto& neuronName : analyzer.mTargetReport)
	{
		cout << neuronName.first << endl << "  ";
		for (auto& regionName : neuronName.second)
		{
			cout << regionName.first << ":" << regionName.second << " ";
		}
		cout << endl;
	}*/
	//cout << analyzer.getParentRegionName("MOs5") << endl;

	clock_t timeStart = clock();
	nm::MetricsEngine engine;
	engine.setSWCFilesDirectory("D:\\Allen Institute Work\\Connectome_refinement\\_SWC_DATASET\\2873Cells_SWCfiles\\");
	engine.setTargetRegionList();
	engine.outputAxonTargetReport("C:\\Users\\hkuo9\\Desktop\\", "targetReport");
	clock_t timeEnd = clock();
	cout << float(timeEnd - timeStart) / CLOCKS_PER_SEC << " seconds" << endl;

	system("pause");

    return 0;
}