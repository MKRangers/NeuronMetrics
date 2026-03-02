#include "nmMetricsEngine.h"

#include <iostream>
#include <ctime>

using namespace std;

int main()
{	
	nm::MetricsEngine engine("D:\\NeuronMetrics\\CCF\\annotation_25_float32.tif", "D:\\NeuronMetrics\\CCF\\Mouse.csv");
	engine.setSWCFilesDirectory("D:\\Allen Institute Work\\Connectome_refinement\\_SWC_DATASET\\2873Cells_SWCfiles\\");
	engine.setTargetRegionList();
	
	// Multi-thread
	clock_t timeStart = clock();
	engine.outputAxonTargetReport("C:\\Users\\hkuo9\\Desktop\\", "targetReport");
	clock_t timeEnd = clock();
	cout << float(timeEnd - timeStart) / CLOCKS_PER_SEC << " seconds" << endl;

	// Single thread
	clock_t timeStart2 = clock();
	engine.outputAxonTargetReport("C:\\Users\\hkuo9\\Desktop\\", "targetReport", false);
	clock_t timeEnd2 = clock();
	cout << float(timeEnd2 - timeStart2) / CLOCKS_PER_SEC << " seconds" << endl;

	system("pause");

    return 0;
}