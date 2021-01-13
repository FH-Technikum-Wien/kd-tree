#pragma once

#include <vector>

#include "Structures.h"

constexpr int DIMENSIONS = 3;

class KdTree
{
public:
	KdTree(float* vertices, int numberOfTriangles);
	KdTree(std::vector<Point*> points);
	RayHit* raycast(Ray ray, float maxDistance);
	std::vector<Node*> getNodes();

	void print();
	void printStatistics();

private:

	std::vector<Point*> getPointList(float* vertices, int numberOfTriangles);
	Node* createKdTree(std::vector<Point*> points, int depth, Vector max, Vector min);
	RayHit* findIntersection(Node* node, Ray ray, float maxDistance);
	RayHit* rayIntersectionWithTriagnle(Triangle* triangle, Ray ray);

	inline auto getComparatorForAxis(int axis) const
	{ 
		return [axis](Point* p1, Point* p2)
		{
			return p1->pos[axis] < p2->pos[axis]; 
		}; 
	}

	Node* root;
};

