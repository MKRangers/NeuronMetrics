#ifndef nmAXONPROJECTIONANALYZER_H
#define nmAXONPROJECTIONANALYZER_H

#include "nmNeuron.h"
#include "nmMouseCCF.h"

#include <map>
#include <thread>
#include <atomic>
#include <mutex>

namespace nm
{ 

	class AxonProjectionAnalyzer
	{
		public:
			AxonProjectionAnalyzer(const std::string& CCFPath, const std::string& MouseCSVPath) : mCCF(CCFPath, MouseCSVPath) {}
			~AxonProjectionAnalyzer() = default;

			void getSWCFilePathsFromDirectory(const std::string& directoryPath);
			void setTargetList(boost::container::flat_set<std::string> targetList) { mTargetList = targetList; }
			void setRemoveSpikes(bool removeSpikes) { mRemoveSpikes = removeSpikes; }
			void setSpikeLengthThreshold(double length) { mSpikeLengthThreshold = length; }

			void populateAxonSubregionMaps(Neuron& neuron);
			void populateAxonTargetRegionMaps(Neuron& neuron);
			void populateAxonTargetRegionLengths(Neuron& neuron);
			
			std::map<std::string, std::map<std::string, double>> mTargetReport;
			std::map<std::string, std::map<std::string, double>> mTargetReport_R;
			std::map<std::string, std::map<std::string, double>> mTargetReport_L;
			void batchComputeTargetRegionLengths();
			void batchComputeTargetRegionLengths_singleThread();

			std::string getParentRegionName(std::string regionName) { return mCCF.getParentRegionName(regionName); }
			boost::container::flat_set<std::string> getChildrenRegions(std::string parentRegionName) { return mCCF.getChildrenRegions(parentRegionName); }
			void getAllDescendentRegions(std::string ancestorRegionName, boost::container::flat_set<std::string>& allDescendents) { return mCCF.getAllDescendentRegions(ancestorRegionName, allDescendents); }


		private:
			bool   mRemoveSpikes = true;
			double mSpikeLengthThreshold = 20;
			double mMajorTargetLengthThreshold = 1000;
			int    mMajorTargetBifurCount = 0;
			int	   mMajorTargetEndCount = 1;
			int	   mMinorTargetBifurCount = 1;
			int    mMinorTargetEndCount = 1;

			MouseCCF mCCF;

			std::vector<std::string> mSWCFilePaths;
			boost::container::flat_set<std::string> mTargetList;
			
			std::mutex mReportMutex;
			void targetRegionLengthWorker(std::atomic<size_t>& nextFileIndex);

			std::pair<bool, double> isTarget(const std::vector<const Node*>& nodes, std::unordered_map<int, std::vector<const Node*>>& nodeID2childMap);
	};

}

const boost::container::flat_set<std::string> targetRegionsWithLayerNums = {
	"FRP", "ACAd", "ACAv", "PL", "ILA", "ORBl", "ORBm", "ORBvl", "AId", "AIv", "AIp", "GU", "VISC", "TEa", "PERI",
	"ECT", "SSp-bfd", "SSp-ll", "SSp-m", "SSp-n", "SSp-tr", "SSp-ul", "SSp-un", "SSs", "MOp", "MOs",
	"VISp", "VISa", "VISal", "VISam", "VISl", "VISli", "VISpl", "VISpm", "VISpor", "VISrl",
	"RSPagl", "RSPd", "RSPv", "AUDd", "AUDp", "AUDpo", "AUDv"
};

#endif