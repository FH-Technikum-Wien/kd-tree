#pragma once

#include <vector>

#include "Structures.h"

constexpr int DIMENSIONS = 3;

class KdTree
{
public:
	KdTree(float* vertices, int numberOfTriangles);
	KdTree(std::vector<Point*> points);


private:

	std::vector<Point*> getPointList(float* vertices, int numberOfTriangles);
	Node* createKdTree(std::vector<Point*> points, int depth);

	bool compareOnXAxis(Point const& p1, Point const& p2) const { return p1.pos[0] < p2.pos[0]; }
	bool compareOnYAxis(Point const& p1, Point const& p2) const { return p1.pos[1] < p2.pos[1]; }
	bool compareOnZAxis(Point const& p1, Point const& p2) const { return p1.pos[2] < p2.pos[2]; }

	auto getComparatorForAxis(int axis) const
	{ 
		return [axis](Point const& p1, Point const& p2)
		{
			return p1.pos[axis] < p2.pos[axis]; 
		}; 
	}

	Node* root;
};

