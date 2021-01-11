#include "KdTree.h"

#include <algorithm>
#include <functional>

#include <fstream>
#include <limits>

KdTree::KdTree(float* vertices, int numberOfTriangles) : KdTree(getPointList(vertices, numberOfTriangles)) {}

KdTree::KdTree(std::vector<Point*> points) {
	root = createKdTree(points, 0);
}

Triangle* KdTree::raycast(Ray ray, float maxDistance)
{
	return findIntersection(root, ray, maxDistance);
}

void KdTree::print()
{
	std::function<void(Node*)> printTree;
	printTree = [&printTree](Node* node) {
		std::cout << node->point->pos << " | " << node->axis << std::endl;
		if (node->left != nullptr) {
			std::cout << "Left:" << std::endl;
			printTree(node->left);
		}
		if (node->right != nullptr) {
			std::cout << "Right:" << std::endl;
			printTree(node->right);
		}
	};

	printTree(root);
}

void KdTree::printStatistics()
{
	int maxDepth = 0;
	int minDepth = std::numeric_limits<int>::max();
	std::function<void(Node*, int)> depthCount;
	depthCount = [&maxDepth, &minDepth, &depthCount](Node* node, int depth) {
		if(node == nullptr)
			return;

		// Current depth higher than maxDepth -> new highest depth.
		if (depth > maxDepth)
			maxDepth = depth;

		// If leaf node and smaller depth than minDepth -> new lowest depth.
		if (node->left == nullptr && node->right == nullptr && depth < minDepth)
			minDepth = depth;
		
		// Continue left and right recursively.
		depthCount(node->left, depth + 1);
		depthCount(node->right, depth + 1);
	};

	depthCount(root, 0);
	std::cout << "Max Depth: " << maxDepth << std::endl;
	std::cout << "Min Depth: " << minDepth << std::endl;
}

std::vector<Point*> KdTree::getPointList(float* vertices, int numberOfTriangles)
{
	std::vector<Point*> points;
	// Create points for each triangle and connect them (with a reference to the triangle).
	for (int i = 0; i < numberOfTriangles; i++)
	{
		int index = i * 9;
		Vector a = Vector(vertices[index], vertices[index + 1], vertices[index + 2]);
		Vector b = Vector(vertices[index + 3], vertices[index + 4], vertices[index + 5]);
		Vector c = Vector(vertices[index + 6], vertices[index + 7], vertices[index + 8]);

		// Connects triangle with points and vice-versa.
		Triangle* triangle = new Triangle(a, b, c);
		// Get as std::vector.
		std::vector<Point*> trianglePoints = triangle->getPoints();
		points.insert(points.end(), trianglePoints.begin(), trianglePoints.end());
	}
	return points;
}

Node* KdTree::createKdTree(std::vector<Point*> points, int depth)
{
	// If reached leaf, stop
	if (points.empty())
		return nullptr;

	// If only one point left, just add it.
	if (points.size() == 1)
		return new Node(points[0], nullptr, nullptr, 0);

	// Get widest axis
	float maxAxisWidth = 0;
	int axisWithMaxWidth = 0;

	// Go through each axis and determine biggest extend.
	for (int axis = 0; axis < DIMENSIONS; axis++)
	{
		auto comparator = getComparatorForAxis(axis);

		Point* min = *std::min_element(points.begin(), points.end(), comparator);
		Point* max = *std::max_element(points.begin(), points.end(), comparator);

		float axisWidth = max->pos[axis] - min->pos[axis];

		if (axisWidth > maxAxisWidth) {
			maxAxisWidth = axisWidth;
			axisWithMaxWidth = axis;
		}
	}

	// Get median point (and sort by median).
	int medianIndex = points.size() / 2;
	std::nth_element(points.begin(), points.begin() + medianIndex, points.end(), getComparatorForAxis(axisWithMaxWidth));
	Point* medianPoint = points[medianIndex];

	std::vector<Point*> leftPoints(points.begin(), points.begin() + medianIndex);
	// Remove median point from list by skipping it.
	std::vector<Point*> rightPoints(points.begin() + medianIndex + 1, points.end());

	// Recursively go down each branch.
	return new Node(medianPoint, createKdTree(leftPoints, depth + 1), createKdTree(rightPoints, depth + 1), axisWithMaxWidth);
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
Triangle* KdTree::findIntersection(Node* node, Ray ray, float maxDistance)
{
	// No node, no triangle to intersect.
	if (node == nullptr)
		return nullptr;

	int axis = node->axis;

	// Get near and far nodes depending on ray's origin (Before or after splitting plane?).
	Node* near = ray.origin[axis] > node->point->pos[axis] ? node->right : node->left;
	Node* far = near == node->right ? node->left : node->right;

	// Distance from ray to splitting plane.
	float t = (node->point->pos[axis] - ray.origin[axis]) / ray.direction[axis];

	// If our direction is parallel to the axis, don't reduce it.
	if (ray.direction[axis] == 0.0f)
		t = maxDistance;

	// Check near node.
	Triangle* triangle = findIntersection(near, ray, t);

	// Return triangle if collided
	if (triangle != nullptr)
		return triangle;

	// Check current node.
	if (rayIntersectionWithTriagnle(node->point->triangle, ray))
		return node->point->triangle;

	

	// Only check far node if intersection is possible.
	if (ray.direction[axis] != 0.0f && t >= 0 && t < maxDistance) {
		// Check far with new position, no need to check again what's behind us.
		// Move ray by 't' factor in its direction.
		return findIntersection(far, Ray(ray.origin + (ray.direction * t), ray.direction), maxDistance - t);
	}

	// No collision until now -> No collision at all.
	return nullptr;
}

/// <summary>
/// Möller–Trumbore intersection algorithm
/// https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
/// </summary>
bool KdTree::rayIntersectionWithTriagnle(Triangle* triangle, Ray ray)
{
	const float EPSILON = 0.0000001;

	Vector v1 = triangle->a->pos;
	Vector v2 = triangle->b->pos;
	Vector v3 = triangle->c->pos;

	Vector edge1 = v2 - v1;
	Vector edge2 = v3 - v1;

	Vector h = ray.direction.cross(edge2);
	float a = edge1.dot(h);

	// This ray is parallel to this triangle.
	if (a > -EPSILON && a < EPSILON)
		return false;

	float f = 1.0f / a;
	Vector s = ray.origin - v1;
	float u = f * s.dot(h);
	
	if (u < 0 || u > 1)
		return false;

	Vector q = s.cross(edge1);
	float v = f * ray.direction.dot(q);

	if (v < 0 || u + v > 1)
		return false;

	float t = f * edge2.dot(q);

	return t > EPSILON;
}
