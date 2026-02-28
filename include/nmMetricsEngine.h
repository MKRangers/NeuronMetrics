#ifndef nmMETRICSENGINE_H
#define nmMETRICSENGINE_H

#include "nmAxonProjectionAnalyzer.h"

namespace nm
{

	class MetricsEngine
	{
		public:
			MetricsEngine() = default;
			~MetricsEngine() = default;

			void setSWCFilesDirectory(const std::string& directoryPath);
			void setTargetRegionList();
			void outputAxonTargetReport(const std::string& outputFolder, const std::string& fileBaseName);


		private:
			AxonProjectionAnalyzer mAxonProjAnalyzer;
			boost::container::flat_set<std::string> mTargetList;
	};

}


#endif