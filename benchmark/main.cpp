#include "nmMetricsEngine.h"
#include "nmNeuron.h"
#include "nmUtilities.h"

#include <iostream>
#include <ctime>
#include <filesystem>

using namespace std;

int main()
{	
	// ----------------- Test code for nmMetricsEngine -- --------------- //
	/*string currPath = filesystem::current_path().string();
	nm::MetricsEngine engine(currPath + "\\..\\..\\CCF\\annotation_25_float32.tif", currPath + "\\..\\..\\CCF\\Mouse.csv");
	engine.setSWCFilesDirectory("D:\\Allen Institute Work\\Connectome_refinement\\_SWC_DATASET\\2873Cells_SWCfiles\\");
	engine.setTargetRegionList();*/
	
	// Multi-thread
	/*clock_t timeStart = clock();
	engine.outputAxonTargetReport("C:\\Users\\hkuo9\\Desktop\\");
	clock_t timeEnd = clock();
	cout << float(timeEnd - timeStart) / CLOCKS_PER_SEC << " seconds" << endl;*/

	// Single thread
	/*clock_t timeStart2 = clock();
	engine.outputAxonTargetReport("C:\\Users\\hkuo9\\Desktop\\", false);
	clock_t timeEnd2 = clock();
	cout << float(timeEnd2 - timeStart2) / CLOCKS_PER_SEC << " seconds" << endl;*/
	// ----------------- Test code for nmMetricsEngine -- --------------- //

	// ------------------ Test code for nmUtilities -- --------------- //
	nm::Neuron n1("C:\\Users\\hkuo9\\Desktop\\220332_4623-X12025-Y17498_reg_xy25z25_xy0z0.swc");
	n1.populateSegments();
	cout << "n1 segments number: " << n1.mSegments.size() << endl;
	vector<nm::Node> interpolatedNodes = nm::interpolateNodes(n1);
	nm::Neuron n2(interpolatedNodes, false);
	n2.writeSWCFile("C:\\Users\\hkuo9\\Desktop\\220332_4623-X12025-Y17498_reg_xy25z25_xy0z0_interpolated.swc");
	n2.populateSegments();
	n2.writeSegmentsToSWC("C:\\Users\\hkuo9\\Desktop\\220332_4623-X12025-Y17498_reg_xy25z25_xy0z0_interpolated_segments.swc");
	cout << "Segments number: " << n2.mSegments.size() << endl;
	int segNodesNumSum = 0;
	for (auto& seg : n2.mSegments)
		segNodesNumSum += seg.nodes.size();
	cout << "Sum of nodes in all segments: " << segNodesNumSum << endl;
	cout << "Original nodes number: " << n2.getNodesNum() << endl;
	// ------------------ Test code for nmUtilities -- --------------- //

	system("pause");

    return 0;
}