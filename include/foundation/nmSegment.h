#ifndef nmSEGMENT_H
#define nmSEGMENT_H

#include "nmNode.h"

#include <vector>

namespace nm
{

class Segment
{
	public:
		Segment() = default;
		Segment(const std::vector<Node>& nodes) : mNodes(nodes) {}
		~Segment() = default;

		std::vector<Node>		mNodes;


	
};

}

#endif