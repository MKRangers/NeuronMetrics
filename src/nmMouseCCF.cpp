#include "nmMouseCCF.h"
#include "nmExceptions.h"

#include <tiffio.h>

#include <boost\algorithm\string.hpp>

#include <iostream>
#include <fstream>

using namespace std;

namespace nm
{

	MouseCCF::MouseCCF(const string& CCFPath, const string& intensityRegionMappingFilePath)
	{
		readTIFFFile(CCFPath);
		setIntensityRegionMapping(intensityRegionMappingFilePath);
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

	void MouseCCF::setIntensityRegionMapping(const string& mappingFilePath)
	{
		std::ifstream file(mappingFilePath);
		if (!file)
			throw FileNotFoundException(mappingFilePath);

		string line; 
		while (getline(file, line))
		{
			vector<string> lineItems;
			boost::split(lineItems, line, boost::is_any_of("\t"));
			mIntensity2RegionMap.insert({ stoi(lineItems[0]), lineItems[1] });
		}
	}

	string MouseCCF::getRegionName(const Node& node)
	{
		int x = static_cast<int>(round(node.getX()));
		int y = static_cast<int>(round(node.getY()));
		int z = static_cast<int>(round(node.getZ()));

		if (x < 0 || x >= mWidth || y < 0 || y >= mHeight || z < 0 || z >= mDepth)
			return "Out of bounds";
		int intensity = static_cast<int>(mData[z * mWidth * mHeight + y * mWidth + x]);
		//cout << intensity << endl;

		// annotation_25_float32.tif seems to have some erroneous intensity values that don't match those in Mouse.csv.
		// This is a temporary workaround to assign region names to those voxels based on their intensity and x coordinate until we can figure out the root cause of the issue.
		if (intensity == 182305696)
			return "SSp-un";
		else if (intensity == 312782560)
			return "VISa";
		else if ((intensity == 312782592 || intensity == 312782624) && node.getX() <= 335)
			return "VISrl";
		else if (intensity == 312782592 && node.getX() >= 339)
			return "VISli";
		else if (intensity == 312782656)
			return "VISpor";
		else if (intensity == 526157184)
			return "FRP5";
		auto it = mIntensity2RegionMap.find(intensity);
		return (it != mIntensity2RegionMap.end()) ? it->second : "Unknown region";
	}

	string MouseCCF::getRegionName(int x, int y, int z)
	{
		if (x < 0 || x >= mWidth || y < 0 || y >= mHeight || z < 0 || z >= mDepth)
			return "Out of bounds";
		int intensity = static_cast<int>(mData[z * mWidth * mHeight + y * mWidth + x]);
		//cout << intensity << endl;

		// annotation_25_float32.tif seems to have some erroneous intensity values that don't match those in Mouse.csv.
		// This is a temporary workaround to assign region names to those voxels based on their intensity and x coordinate until we can figure out the root cause of the issue.
		if (intensity == 182305696)
			return "SSp-un";
		else if (intensity == 312782560)
			return "VISa";
		else if ((intensity == 312782592 || intensity == 312782624) && x <= 335)
			return "VISrl";
		else if (intensity == 312782592 && x >= 339)
			return "VISli";
		else if (intensity == 312782656)
			return "VISpor";
		else if (intensity == 526157184)
			return "FRP5";
		auto it = mIntensity2RegionMap.find(intensity);
		return (it != mIntensity2RegionMap.end()) ? it->second : "Unknown region";
	}

}