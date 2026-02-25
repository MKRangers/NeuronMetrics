#ifndef nmNODE_H
#define nmNODE_H

namespace nm
{
	enum NodeType { ntSoma, ntAxon, ntBasalDendrite, ntApicalDendrite, ntUndefined };


class Node
{
	public:
		Node() = default;
		Node(double x, double y, double z, int id = 0, int parentID = -1, int type = 5, double radius = 1) : x(x), y(y), z(z), id(id), parentID(parentID), type(type), radius(radius) {}
		~Node() = default;

		const bool	operator==(const Node& rhsNode) const;
		const bool	operator!=(const Node& rhsNode) const { return !(*this == rhsNode); }

		int	   getID() const { return id; }
		double getX() const { return x; }
		double getY() const { return y; }
		double getZ() const { return z; }
		int	   getType() const { return type; }
		int	   getParentID() const { return parentID; }
		double getRadius() const { return radius; }


	private:
		double x;
		double y;
		double z;
		int    id;
		int	   type;
		int	   parentID;
		double radius;
};

}

#endif