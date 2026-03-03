#ifndef nmMETRICSENGINE_H
#define nmMETRICSENGINE_H

#include "nmAxonProjectionAnalyzer.h"

namespace nm
{

	class MetricsEngine
	{
		public:
			MetricsEngine(const std::string& CCFPath, const std::string& MouseCSVPath) : mAxonProjAnalyzer(CCFPath, MouseCSVPath) {}
			~MetricsEngine() = default;

			void setSWCFilesDirectory(const std::string& directoryPath);
			void setTargetRegionList(const std::string& targetRegionListFile = "");
			void outputAxonTargetReport(const std::string& outputFolder, bool multiThread = true);


		private:
			AxonProjectionAnalyzer mAxonProjAnalyzer;
			boost::container::flat_set<std::string> mTargetList;
	};

}


#endif