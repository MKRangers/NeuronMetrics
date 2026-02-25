#include "nmNeuron.h"
#include "nmMouseCCF.h"

#include <iostream>
#include <ctime>

using namespace std;

int main()
{
	string testSWCFilePath = "D:\\Allen Institute Work\\Connectome_refinement\\_SWC_DATASET\\2873Cells_SWCfiles\\17781_2886-X14249-Y36315_reg_xy25z25_xy0z0.swc";
	clock_t startTime = clock();
	nm::Neuron neuron(testSWCFilePath);
	clock_t endTime = clock();
	cout << float(endTime - startTime) / CLOCKS_PER_SEC << " seconds" << endl;
	neuron.scaleNeuron(0.04);

	nm::MouseCCF mouseCCF;
	mouseCCF.readTIFFFile("D:\\Arboreta_analysis\\Arboreta_anaysis\\data\\BrainTemplates\\annotation_25_float32.tif");
	string intensityRegionMappingFilePath = "D:\\Arboreta_analysis\\Arboreta_anaysis\\data\\BrainTemplates\\idValue2regionName.txt";
	mouseCCF.setIntensityRegionMapping(intensityRegionMappingFilePath);
	cout << "CCF dimensions: " << mouseCCF.getWidth() << " x " << mouseCCF.getHeight() << " x " << mouseCCF.getDepth() << endl;
	cout << "Region name for node 0: " << mouseCCF.getRegionName(neuron.mNodes[0]) << endl;

	system("pause");

    return 0;
}