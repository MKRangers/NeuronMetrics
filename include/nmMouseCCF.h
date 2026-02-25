#ifndef nmMOUSECCF_H
#define nmMOUSECCF_H

#include "nmNode.h"

#include <boost\container\flat_map.hpp>

#include <vector>
#include <string>

namespace nm
{

class MouseCCF
{
public:
	MouseCCF() = default;
	MouseCCF(const std::string& CCFPath, const std::string& intensityRegionMappingFilePath);
	~MouseCCF() = default;

	void readTIFFFile(const std::string& filePath);
	int getWidth() const { return mWidth; }
	int getHeight() const { return mHeight; }
	int getDepth() const { return mDepth; }

	void setIntensityRegionMapping(const std::string& mappingFilePath);

	std::string getRegionName(const Node& node);
	std::string getRegionName(int x, int y, int z);


private:
	std::vector<float> mData; // 1D vector to store the 3D image data
	int mWidth;
	int mHeight;
	int mDepth;

	boost::container::flat_map<int, std::string> mIntensity2RegionMap; // Intensity value -> brain region name
};

}

#endif