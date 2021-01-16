#include "KdTree.h"

#include <algorithm>
#include <functional>

#include <fstream>
#include <limits>
#include <cmath>


KdTree::KdTree(float* vertices, unsigned int vertexCount, unsigned int* indices, unsigned int indexCount) :
	KdTree(getPointList(vertices, vertexCount, indices, indexCount)) {}

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

KdTree::~KdTree()
{
	for (KdStructs::Triangle* triangle : triangles)
		delete triangle;
	triangles.clear();

	delete root;
}

void KdTree::raycast(KdStructs::Ray ray, KdStructs::RayHit*& hit)
{
	findIntersection(root, ray, hit);
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
		if (node == nullptr)
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

std::vector<KdStructs::Point*> KdTree::getPointList(float* vertices, unsigned int vertexCount, unsigned int* indices, unsigned int indexCount)
{
	std::vector<KdStructs::Point*> points;
	// Create points for each triangle and connect them (with a reference to the triangle).
	for (int i = 0; i < indexCount; i += 3)
	{
		// Get vertex indices, defining the current triangle
		int vertexIndex1 = indices[i] * 3;
		int vertexIndex2 = indices[i + 1] * 3;
		int vertexIndex3 = indices[i + 2] * 3;

		int triangleIndex = i / 3;

		// Convert vertexData to Vector.
		KdStructs::Vector a = KdStructs::Vector(vertices[vertexIndex1], vertices[vertexIndex1 + 1], vertices[vertexIndex1 + 2]);
		KdStructs::Vector b = KdStructs::Vector(vertices[vertexIndex2], vertices[vertexIndex2 + 1], vertices[vertexIndex2 + 2]);
		KdStructs::Vector c = KdStructs::Vector(vertices[vertexIndex3], vertices[vertexIndex3 + 1], vertices[vertexIndex3 + 2]);

		KdStructs::Triangle* triangle = new KdStructs::Triangle(a, b, c);
		triangles.push_back(triangle);

		// Check if point is already in list (duplicate vertex).
		// For a.
		KdStructs::Point* point = findPoint(KdStructs::Point(a), points);
		if (point != nullptr) {
			// if in list, add triangle to point.
			point->triangles.push_back(triangle);
		}
		else {
			// If not in list, add it as a new point.
			points.push_back(new KdStructs::Point(a, triangle));
		}
		// For b.
		point = findPoint(KdStructs::Point(b), points);
		if (point != nullptr)
			point->triangles.push_back(triangle);
		else
			points.push_back(new KdStructs::Point(b, triangle));
		// For c.
		point = findPoint(KdStructs::Point(c), points);
		if (point != nullptr)
			point->triangles.push_back(triangle);
		else
			points.push_back(new KdStructs::Point(c, triangle));
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
	int axis = 0;

	// Go through each axis and determine biggest extend.
	for (int currentAxis = 0; currentAxis < DIMENSIONS; currentAxis++)
	{
		auto comparator = getComparatorForAxis(currentAxis);

		KdStructs::Point* min = *std::min_element(points.begin(), points.end(), comparator);
		KdStructs::Point* max = *std::max_element(points.begin(), points.end(), comparator);

		float axisWidth = max->pos[currentAxis] - min->pos[currentAxis];

		if (axisWidth > maxAxisWidth) {
			maxAxisWidth = axisWidth;
			axis = currentAxis;
		}
	}

	// Get median point (and sort by median).
	int medianIndex = points.size() / 2;
	std::nth_element(points.begin(), points.begin() + medianIndex, points.end(), getComparatorForAxis(axis));
	KdStructs::Point* medianPoint = points[medianIndex];

	std::vector<KdStructs::Point*> leftPoints(points.begin(), points.begin() + medianIndex);
	// Remove median point from list by skipping it.
	std::vector<KdStructs::Point*> rightPoints(points.begin() + medianIndex + 1, points.end());

	// Set max and min values for the splitting plane
	KdStructs::Vector newMax = KdStructs::Vector(max);
	KdStructs::Vector newMin = KdStructs::Vector(min);
	newMax[axis] = medianPoint->pos[axis];
	newMin[axis] = medianPoint->pos[axis];

	// Recursively go down each branch.
	KdStructs::Node* left = createKdTree(leftPoints, depth + 1, newMax, min);
	KdStructs::Node* right = createKdTree(rightPoints, depth + 1, max, newMin);

	return new KdStructs::Node(medianPoint, left, right, axis, max, min);
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
void KdTree::findIntersection(KdStructs::Node* node, KdStructs::Ray ray, KdStructs::RayHit*& hit)
{
	// No node, no triangle to intersect.
	if (node == nullptr)
		return;

	// Check current node.
	for (KdStructs::Triangle* triangle : node->point->triangles) {
		float distance = rayIntersectionWithTriagnle(triangle, ray);
		if (distance < 0)
			continue;

		KdStructs::Vector position = ray.origin + ray.direction * distance;
		if (hit == nullptr)
			hit = new KdStructs::RayHit(triangle, position, distance);
		else if (distance <= hit->distance) {
			hit->triangle = triangle;
			hit->position = position;
			hit->distance = distance;
		}
	}
	

	int axis = node->axis;

	// Get near and far nodes depending on ray's origin (Before or after splitting plane?).
	KdStructs::Node* near = ray.origin[axis] > node->point->pos[axis] ? node->right : node->left;
	KdStructs::Node* far = near == node->right ? node->left : node->right;


	// If our direction is parallel to the axis, only visit near
	if (ray.direction[axis] == 0.0f) {
		findIntersection(near, ray, hit);
	}
	else {
		// Distance from ray to splitting plane.
		float t = (node->point->pos[axis] - ray.origin[axis]) / ray.direction[axis];

		KdStructs::Ray newRay = KdStructs::Ray(ray.origin, ray.direction, hit != nullptr ? hit->distance : ray.distance);
		// Only check far node if intersection is possible (ray can reach it).
		// Also skip if current hit is smaller than splitting plane distance.
		if (0 <= t && t < ray.distance && (hit == nullptr || hit->distance > t)) {
			findIntersection(near, newRay, hit);
			findIntersection(far, newRay, hit);
		}
		else {
			findIntersection(near, newRay, hit);
		}
	}
}

/// <summary>
/// M�ller�Trumbore intersection algorithm
/// https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
/// </summary>
float KdTree::rayIntersectionWithTriagnle(KdStructs::Triangle* triangle, KdStructs::Ray ray)
{
	const float EPSILON = 0.0000001;

	KdStructs::Vector v1 = triangle->a;
	KdStructs::Vector v2 = triangle->b;
	KdStructs::Vector v3 = triangle->c;

	KdStructs::Vector edge1 = v2 - v1;
	KdStructs::Vector edge2 = v3 - v1;

	KdStructs::Vector h = ray.direction.cross(edge2);
	float a = edge1.dot(h);

	// This ray is parallel to this triangle.
	if (a > -EPSILON && a < EPSILON)
		return -1;

	float f = 1.0f / a;
	KdStructs::Vector s = ray.origin - v1;
	float u = f * s.dot(h);

	if (u < 0 || u > 1)
		return -1;

	KdStructs::Vector q = s.cross(edge1);
	float v = f * ray.direction.dot(q);

	if (v < 0 || u + v > 1)
		return -1;

	float t = f * edge2.dot(q);

	if (t > EPSILON) {
		return t;
	}

	return -1;
}
