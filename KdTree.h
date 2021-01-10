#pragma once

#include <vector>

#include "Structures.h"

constexpr int DIMENSIONS = 2;

class KdTree
{
public:
	KdTree(float* vertices, int numberOfTriangles);
	KdTree(std::vector<Point*> points);

	void Print();

private:

	std::vector<Point*> getPointList(float* vertices, int numberOfTriangles);
	Node* createKdTree(std::vector<Point*> points, int depth);

	auto getComparatorForAxis(int axis) const
	{ 
		return [axis](Point* p1, Point* p2)
		{
			return p1->pos[axis] < p2->pos[axis]; 
		}; 
	}

	Node* root;
};

