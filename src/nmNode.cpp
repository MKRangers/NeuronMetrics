#include "nmNode.h"

using namespace std;

namespace nm
{
	
	const bool Node::operator==(const Node& rhsNode) const
	{
		return (x == rhsNode.x) && (y == rhsNode.y) && (z == rhsNode.z);
	}

}
