#ifndef nmMETRICSENGINE_H
#define nmMETRICSENGINE_H

#include "nmNeuron.h"
#include "nmMouseCCF.h"

namespace nm
{

	class MetricsEngine
	{
		public:
			MetricsEngine() = default;
			~MetricsEngine() = default;

			void getSWCFilePathsFromDirectory(const std::string& directoryPath);


		private:
			std::vector<std::string> SWCFilePaths;
			
	};

}


#endif