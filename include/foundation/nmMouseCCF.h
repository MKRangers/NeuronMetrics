#ifndef nmMOUSECCF_H
#define nmMOUSECCF_H

#include "nmNode.h"

#include <boost\container\flat_map.hpp>
#include <boost\container\flat_set.hpp>

#include <vector>
#include <string>
#include <set>
#include <memory>

namespace nm
{

	class MouseCCF
	{
		public:
			MouseCCF();
			~MouseCCF() = default;
			MouseCCF(const MouseCCF&) = delete;
			MouseCCF& operator=(const MouseCCF&) = delete;

			void readTIFFFile(const std::string& filePath);
			void setCCFPath(const std::string& CCFPath) { readTIFFFile(CCFPath); }
			int  getWidth() { return mWidth; }
			int  getHeight() { return mHeight; }
			int  getDepth() { return mDepth; }
			std::string getRegionNameByNode(const Node& node);
			std::string getRegionNameByCoord(int x, int y, int z);
			std::string getRegionNameByIntensity(int intensity) { return mIntensity2RegionMap.at(intensity); }
			std::set<std::string> getAllRegionNames();

			std::string getParentRegionName(std::string regionName) { return mRegionMap.at(regionName)->parent->name; }
			int	getRegionIntensity(std::string regionName) { return mRegionMap.at(regionName)->intensity; }
			boost::container::flat_set<std::string> getChildrenRegions(std::string parentRegionName);
			void getAllDescendentRegions(std::string ancestorRegionName, boost::container::flat_set<std::string>& allDescendents);


		private:
			const std::string CCFPath = "D:\\NeuronMetrics\\CCF\\annotation_25_float32.tif";
			const std::string intensityRegionMappingFilePath = "D:\\NeuronMetrics\\CCF\\intensityValue2regionName.txt";
			const std::string mouseRegionHierarchyFilePath = "D:\\NeuronMetrics\\CCF\\Mouse.csv";

			std::vector<float> mData; // 1D vector to store the 3D image data
			int mWidth;
			int mHeight;
			int mDepth;

			struct Region
			{
				std::string name;
				int level = -1;
				int intensity = -1;
				Region* parent = nullptr;
				std::vector<Region*> children;
			};

			void constructRegionMaps();
			boost::container::flat_map<int, std::string> mIntensity2RegionMap; // Intensity value -> brain region name
			boost::container::flat_map<std::string, std::unique_ptr<Region>> mRegionMap; // Maps region name to its node in the hierarchy
	};

}

#endif