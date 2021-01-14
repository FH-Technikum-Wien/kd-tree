#include "KdTree.h"

#include <algorithm>
#include <functional>

#include <fstream>
#include <limits>

KdTree::KdTree(float* vertices, int numberOfTriangles) : KdTree(getPointList(vertices, numberOfTriangles)) {}

KdTree::KdTree(std::vector<KdStructs::Point*> points)
{
	KdStructs::Vector max = KdStructs::Vector(0, 0, 0);
	KdStructs::Vector min = KdStructs::Vector(0, 0, 0);
	// Get max values for axes.
	for (int axis = 0; axis < DIMENSIONS; axis++)
	{
		auto comparator = getComparatorForAxis(axis);

		KdStructs::Point* minPoint = *std::min_element(points.begin(), points.end(), comparator);
		KdStructs::Point* maxPoint = *std::max_element(points.begin(), points.end(), comparator);

		max[axis] = maxPoint->pos[axis];
		min[axis] = minPoint->pos[axis];
	}

	root = createKdTree(points, 0, max, min);
}

KdStructs::RayHit* KdTree::raycast(KdStructs::Ray ray, float maxDistance)
{
	return findIntersection(root, ray, maxDistance);
}

std::vector<KdStructs::Node*> KdTree::getNodes()
{
	std::vector<KdStructs::Node*> nodes;

	std::function<void(KdStructs::Node*)> getNodeRecursive;
	getNodeRecursive = [&nodes, &getNodeRecursive](KdStructs::Node* node) {
		if (node == nullptr)
			return;

		nodes.push_back(node);
		getNodeRecursive(node->left);
		getNodeRecursive(node->right);
	};

	getNodeRecursive(root);
	return nodes;
}

void KdTree::print()
{
	std::function<void(KdStructs::Node*)> printRecursive;
	printRecursive = [&printRecursive](KdStructs::Node* node) {
		std::cout << node->point->pos << " | " << node->axis << " | " << "Max: " << node->max << " Min: " << node->min << std::endl;
		if (node->left != nullptr) {
			std::cout << "Left:" << std::endl;
			printRecursive(node->left);
		}
		if (node->right != nullptr) {
			std::cout << "Right:" << std::endl;
			printRecursive(node->right);
		}
	};

	printRecursive(root);
}

void KdTree::printStatistics()
{
	int maxDepth = 0;
	int minDepth = std::numeric_limits<int>::max();
	std::function<void(KdStructs::Node*, int)> printStatisticsRecursive;
	printStatisticsRecursive = [&maxDepth, &minDepth, &printStatisticsRecursive](KdStructs::Node* node, int depth) {
		if(node == nullptr)
			return;

		// Current depth higher than maxDepth -> new highest depth.
		if (depth > maxDepth)
			maxDepth = depth;

		// If leaf node and smaller depth than minDepth -> new lowest depth.
		if (node->left == nullptr && node->right == nullptr && depth < minDepth)
			minDepth = depth;
		
		// Continue left and right recursively.
		printStatisticsRecursive(node->left, depth + 1);
		printStatisticsRecursive(node->right, depth + 1);
	};

	printStatisticsRecursive(root, 0);
	std::cout << "Max Depth: " << maxDepth << std::endl;
	std::cout << "Min Depth: " << minDepth << std::endl;
}

std::vector<KdStructs::Point*> KdTree::getPointList(float* vertices, int numberOfTriangles)
{
	std::vector<KdStructs::Point*> points;
	// Create points for each triangle and connect them (with a reference to the triangle).
	for (int i = 0; i < numberOfTriangles; i++)
	{
		int index = i * 9;
		KdStructs::Vector a = KdStructs::Vector(vertices[index], vertices[index + 1], vertices[index + 2]);
		KdStructs::Vector b = KdStructs::Vector(vertices[index + 3], vertices[index + 4], vertices[index + 5]);
		KdStructs::Vector c = KdStructs::Vector(vertices[index + 6], vertices[index + 7], vertices[index + 8]);

		// Connects triangle with points and vice-versa.
		KdStructs::Triangle* triangle = new KdStructs::Triangle(a, b, c);
		// Get as std::vector.
		std::vector<KdStructs::Point*> trianglePoints = triangle->getPoints();
		points.insert(points.end(), trianglePoints.begin(), trianglePoints.end());
	}
	return points;
}

KdStructs::Node* KdTree::createKdTree(std::vector<KdStructs::Point*> points, int depth, KdStructs::Vector max, KdStructs::Vector min)
{
	// If reached leaf, stop
	if (points.empty())
		return nullptr;

	// If only one point left, just add it.
	if (points.size() == 1)
		return new KdStructs::Node(points[0], nullptr, nullptr, 0, max, min);

	// Get widest axis
	float maxAxisWidth = 0;
	int axisWithMaxWidth = 0;

	// Go through each axis and determine biggest extend.
	for (int axis = 0; axis < DIMENSIONS; axis++)
	{
		auto comparator = getComparatorForAxis(axis);

		KdStructs::Point* min = *std::min_element(points.begin(), points.end(), comparator);
		KdStructs::Point* max = *std::max_element(points.begin(), points.end(), comparator);

		float axisWidth = max->pos[axis] - min->pos[axis];

		if (axisWidth > maxAxisWidth) {
			maxAxisWidth = axisWidth;
			axisWithMaxWidth = axis;
		}
	}

	// Get median point (and sort by median).
	int medianIndex = points.size() / 2;
	std::nth_element(points.begin(), points.begin() + medianIndex, points.end(), getComparatorForAxis(axisWithMaxWidth));
	KdStructs::Point* medianPoint = points[medianIndex];

	std::vector<KdStructs::Point*> leftPoints(points.begin(), points.begin() + medianIndex);
	// Remove median point from list by skipping it.
	std::vector<KdStructs::Point*> rightPoints(points.begin() + medianIndex + 1, points.end());

	// Set max and min values for the splitting plane
	KdStructs::Vector newMax = KdStructs::Vector(max);
	KdStructs::Vector newMin = KdStructs::Vector(min);
	newMax[axisWithMaxWidth] = medianPoint->pos[axisWithMaxWidth];
	newMin[axisWithMaxWidth] = medianPoint->pos[axisWithMaxWidth];

	// Recursively go down each branch.
	KdStructs::Node* left = createKdTree(leftPoints, depth + 1, newMax, min);
	KdStructs::Node* right = createKdTree(rightPoints, depth + 1, max, newMin);

	return new KdStructs::Node(medianPoint, left, right, axisWithMaxWidth, max, min);
}

/// <summary>
/// 1. Check near node first -> Stop if collided.
/// 2. Check current node -> Stop if collided.
/// Skip far node if:
/// - Non parallel ray (would never intersect far plane)
/// - Distance from ray to splitting plane smaller 0 -> far plane behind us
/// - Distance greater than maxDistance -> skip
/// 3. Check far node
/// If no collision here or skipped -> No collision at all
/// </summary>
KdStructs::RayHit* KdTree::findIntersection(KdStructs::Node* node, KdStructs::Ray ray, float maxDistance)
{
	// No node, no triangle to intersect.
	if (node == nullptr)
		return nullptr;

	int axis = node->axis;

	// Get near and far nodes depending on ray's origin (Before or after splitting plane?).
	KdStructs::Node* near = ray.origin[axis] > node->point->pos[axis] ? node->right : node->left;
	KdStructs::Node* far = near == node->right ? node->left : node->right;

	// Distance from ray to splitting plane.
	float t = (node->point->pos[axis] - ray.origin[axis]) / ray.direction[axis];

	// If our direction is parallel to the axis, don't reduce it.
	if (ray.direction[axis] == 0.0f)
		t = maxDistance;

	// Check near node.
	KdStructs::RayHit* hit = findIntersection(near, ray, t);

	// Return triangle if collided
	if (hit != nullptr)
		return hit;

	// Check current node.
	hit = rayIntersectionWithTriagnle(node->point->triangle, ray);
	if (hit != nullptr)
		return hit;

	

	// Only check far node if intersection is possible.
	if (ray.direction[axis] != 0.0f && t >= 0 && t < maxDistance) {
		// Check far with new position, no need to check again what's behind us.
		// Move ray by 't' factor in its direction.
		return findIntersection(far, KdStructs::Ray(ray.origin + (ray.direction * t), ray.direction), maxDistance - t);
	}

	// No collision until now -> No collision at all.
	return nullptr;
}

/// <summary>
/// Möller–Trumbore intersection algorithm
/// https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
/// </summary>
KdStructs::RayHit* KdTree::rayIntersectionWithTriagnle(KdStructs::Triangle* triangle, KdStructs::Ray ray)
{
	const float EPSILON = 0.0000001;

	KdStructs::Vector v1 = triangle->a->pos;
	KdStructs::Vector v2 = triangle->b->pos;
	KdStructs::Vector v3 = triangle->c->pos;

	KdStructs::Vector edge1 = v2 - v1;
	KdStructs::Vector edge2 = v3 - v1;

	KdStructs::Vector h = ray.direction.cross(edge2);
	float a = edge1.dot(h);

	// This ray is parallel to this triangle.
	if (a > -EPSILON && a < EPSILON)
		return nullptr;

	float f = 1.0f / a;
	KdStructs::Vector s = ray.origin - v1;
	float u = f * s.dot(h);
	
	if (u < 0 || u > 1)
		return nullptr;

	KdStructs::Vector q = s.cross(edge1);
	float v = f * ray.direction.dot(q);

	if (v < 0 || u + v > 1)
		return nullptr;

	float t = f * edge2.dot(q);

	if (t > EPSILON) {
		return new KdStructs::RayHit(triangle, ray.origin + ray.direction * t);
	}

	return nullptr;
}
