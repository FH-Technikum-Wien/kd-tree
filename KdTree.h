#pragma once

#include <vector>

#include "Structures.h"

constexpr int DIMENSIONS = 3;

class KdTree
{
public:
	KdTree(float* vertices, int numberOfTriangles);
	KdTree(std::vector<KdStructs::Point*> points);
	KdStructs::RayHit* raycast(KdStructs::Ray ray, float maxDistance);
	std::vector<KdStructs::Node*> getNodes();

	void print();
	void printStatistics();

private:

	std::vector<KdStructs::Point*> getPointList(float* vertices, int numberOfTriangles);
	KdStructs::Node* createKdTree(std::vector<KdStructs::Point*> points, int depth, KdStructs::Vector max, KdStructs::Vector min);
	KdStructs::RayHit* findIntersection(KdStructs::Node* node, KdStructs::Ray ray, float maxDistance);
	KdStructs::RayHit* rayIntersectionWithTriagnle(KdStructs::Triangle* triangle, KdStructs::Ray ray);

	inline auto getComparatorForAxis(int axis) const
	{ 
		return [axis](KdStructs::Point* p1, KdStructs::Point* p2)
		{
			return p1->pos[axis] < p2->pos[axis]; 
		}; 
	}

	KdStructs::Node* root;
};

