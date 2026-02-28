#include "nmMouseCCF.h"
#include "nmExceptions.h"

#include <tiffio.h>

#include <boost\algorithm\string.hpp>

#include <iostream>
#include <fstream>

using namespace std;

namespace nm
{

	MouseCCF::MouseCCF()
	{
		readTIFFFile(CCFPath);
		constructRegionMaps();
	}

	void MouseCCF::readTIFFFile(const std::string& filePath)
	{
		TIFF* tif = TIFFOpen(filePath.c_str(), "r");
		if (!tif)
			throw std::runtime_error("Failed to open TIFF " + filePath);

		uint32 w, h;
		uint16 bitsPerSample, sampleFormat;
		// Read first slice to determine dimensions
		TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &w);
		TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &h);
		TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &bitsPerSample);
		TIFFGetField(tif, TIFFTAG_SAMPLEFORMAT, &sampleFormat);
		if (bitsPerSample != 32 || sampleFormat != SAMPLEFORMAT_IEEEFP)
		{
			TIFFClose(tif);
			throw std::runtime_error("Expected CCF to be float32");
		}
		mWidth = static_cast<int>(w);
		mHeight = static_cast<int>(h);

		// Count slices
		mDepth = 0;
		do { ++mDepth; } while (TIFFReadDirectory(tif));
		TIFFSetDirectory(tif, 0);  // reset to first slice

		mData.clear();
		mData.resize(mWidth * mHeight * mDepth); // allocate space for entire volume
		for (int z = 0; z < mDepth; ++z)
		{
			for (uint32 row = 0; row < h; ++row)
			{
				float* rowPtr = mData.data() + z * mWidth * mHeight + row * mWidth;
				if (TIFFReadScanline(tif, rowPtr, row) < 0) // read scanline into correct position in mData
				{
					TIFFClose(tif);
					throw std::runtime_error("Failed reading scanline");
				}
			}
			TIFFReadDirectory(tif);
		}

		TIFFClose(tif);
	}

	void MouseCCF::constructRegionMaps()
	{
		ifstream file(mouseRegionHierarchyFilePath);
		if (!file)
			throw FileNotFoundException(mouseRegionHierarchyFilePath);

		auto& rootNodePtr = mRegionMap["root"] = std::make_unique<Region>();
		rootNodePtr->name = "root";
		rootNodePtr->intensity = 997;
		rootNodePtr->level = 0;
		mIntensity2RegionMap[997] = "root";

		vector<string> levelChain(11);
		levelChain[0] = "root";
		int currentLevel = 0;
		string line;
		while (getline(file, line))
		{
			vector<string> items;
			boost::split(items, line, boost::is_any_of(","));
			if (!items[0].compare("InDel") || !items[2].compare("root"))
				continue;

			for (int i = 9; i < items.size(); ++i)
			{
				if (!items[i].empty())
				{
					currentLevel = i - 9;
					break;
				}
			}

			for (int i = currentLevel; i < levelChain.size(); ++i)
				levelChain[i].clear();
			levelChain[currentLevel] = items[2];

			auto& nodePtr = mRegionMap[items[2]] = std::make_unique<Region>();
			nodePtr->name = items[2];
			Region* paPtr = mRegionMap[levelChain[currentLevel - 1]].get();
			paPtr->children.push_back(nodePtr.get());
			nodePtr->parent = paPtr;
			nodePtr->intensity = stoi(items[1]);
			nodePtr->level = currentLevel;
			mIntensity2RegionMap[nodePtr->intensity] = nodePtr->name;
			//cout << nodePtr->name << " " << nodePtr->parent->name << endl;
		}
	}

	string MouseCCF::getRegionNameByNode(const Node& node)
	{
		int x = static_cast<int>(round(node.getX()));
		int y = static_cast<int>(round(node.getY()));
		int z = static_cast<int>(round(node.getZ()));

		if (x < 0 || x >= mWidth || y < 0 || y >= mHeight || z < 0 || z >= mDepth)
			return "Out of bounds";
		int intensity = static_cast<int>(mData[z * mWidth * mHeight + y * mWidth + x]);
		//cout << intensity << endl;

		// annotation_25_float32.tif seems to have some mismatches against Mouse.csv and idValue2regionName.txt.
		// This is a temporary workaround to assign region names to those voxels based on their intensity and x coordinate until we can figure out the root cause of the issue.
		if ((intensity == 312782592 || intensity == 312782624) && node.getX() <= 335)
			return "VISrl";
		else if (intensity == 312782592 && node.getX() >= 339)
			return "VISli";
		auto it = mIntensity2RegionMap.find(intensity);
		return (it != mIntensity2RegionMap.end()) ? it->second : "Unknown region";
	}

	string MouseCCF::getRegionNameByCoord(int x, int y, int z)
	{
		if (x < 0 || x >= mWidth || y < 0 || y >= mHeight || z < 0 || z >= mDepth)
			return "Out of bounds";
		int intensity = static_cast<int>(mData[z * mWidth * mHeight + y * mWidth + x]);
		//cout << intensity << endl;

		// annotation_25_float32.tif seems to have some erroneous intensity values that don't match those in Mouse.csv.
		// This is a temporary workaround to assign region names to those voxels based on their intensity and x coordinate until we can figure out the root cause of the issue.
		if ((intensity == 312782592 || intensity == 312782624) && x <= 335)
			return "VISrl";
		else if (intensity == 312782592 && x >= 339)
			return "VISli";
		auto it = mIntensity2RegionMap.find(intensity);
		return (it != mIntensity2RegionMap.end()) ? it->second : "Unknown region";
	}

	set<string> MouseCCF::getAllRegionNames()
	{
		set<string> outputNames;
		for (auto& it : mRegionMap)
			outputNames.insert(it.first);
		return outputNames;
	}

	boost::container::flat_set<std::string> MouseCCF::getChildrenRegions(std::string parentRegionName)
	{
		boost::container::flat_set<string> childrenNames;
		for (auto& ptr : mRegionMap.at(parentRegionName)->children)
			childrenNames.insert(ptr->name);
		return childrenNames;
	}

	void MouseCCF::getAllDescendentRegions(std::string ancestorRegionName, boost::container::flat_set<std::string>& allDescendents)
	{
		for (auto& ptr : mRegionMap.at(ancestorRegionName)->children)
		{
			allDescendents.insert(ptr->name);
			getAllDescendentRegions(ptr->name, allDescendents);
		}
	}

}