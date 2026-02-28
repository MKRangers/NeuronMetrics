#include "nmMetricsEngine.h"

#include <iostream>
#include <filesystem>
#include <thread>

using namespace std;
using namespace nm;

namespace nm
{

	void MetricsEngine::getSWCFilePathsFromDirectory(const string& directoryPath)
	{
		for (auto& entry : filesystem::directory_iterator(directoryPath))
		{
			if (entry.path().extension() == ".swc")
				SWCFilePaths.push_back(entry.path().string());
		}
	}
	
}