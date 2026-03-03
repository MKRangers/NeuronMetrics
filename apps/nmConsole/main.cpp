#include "nmMetricsEngine.h"
#include "nmExceptions.h"

#include <string>
#include <iostream>
#include <filesystem>
#include <Windows.h>
#include <sstream>

using namespace std;

filesystem::path getExecutablePath()
{
	char buffer[MAX_PATH];
	GetModuleFileNameA(NULL, buffer, MAX_PATH);
	return filesystem::path(buffer).parent_path();
}

int main(int sz, char* args[])
{ 
	std::vector<string> paras;
	for (int i = 1; i < sz; ++i)
	{
		const char* paraC = args[i];
		string paraString(paraC);
		paras.push_back(paraString);
	}

	if (paras.size() < 3)
	{
		cout << "Usage: " << args[0] << " <SWC files directory> <target region list> <output folder>" << endl;
		return 1;
	}

	string swcDir = paras[0];
	string targetRegionListFile = paras[1];
	string outputFolder = paras[2];

	try
	{
		string currPath = getExecutablePath().string();
		cout << "Current executable path: " << currPath << endl;
		string ccfPath = currPath + "\\..\\..\\CCF\\annotation_25_float32.tif";
		string mouseCSVPath = currPath + "\\..\\..\\CCF\\Mouse.csv";
		if (!std::filesystem::exists(ccfPath))
			throw std::runtime_error("CCF not found.");
		if (!std::filesystem::exists(mouseCSVPath))
			throw std::runtime_error("Brain region hierarchy file not found.");

		nm::MetricsEngine engine(ccfPath, mouseCSVPath);
		engine.setSWCFilesDirectory(swcDir);
		engine.setTargetRegionList(targetRegionListFile);
		engine.outputAxonTargetReport(outputFolder);
	}
	catch (const exception& e)
	{
		cerr << "Error: " << e.what() << endl;
		return 2;
	}

	return 0;
}