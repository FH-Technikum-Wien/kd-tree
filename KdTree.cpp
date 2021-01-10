#include "KdTree.h"

#include <algorithm>
#include <functional>

KdTree::KdTree(float* vertices, int numberOfTriangles) : KdTree(getPointList(vertices, numberOfTriangles)){}

KdTree::KdTree(std::vector<Point*> points) {
	root = createKdTree(points, 0);
}

void KdTree::Print()
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

std::vector<Point*> KdTree::getPointList(float* vertices, int numberOfTriangles)
{
	std::vector<Point*> points;
	for (int i = 0; i < numberOfTriangles; i++)
	{
		int index = i * 9;
		Vector a = Vector(vertices[index], vertices[index + 1], vertices[index + 2]);
		Vector b = Vector(vertices[index + 3], vertices[index + 4], vertices[index + 5]);
		Vector c = Vector(vertices[index + 6], vertices[index + 7], vertices[index + 8]);

		Triangle* triangle = new Triangle(a, b, c);
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

	if (points.size() == 1)
		return new Node(points[0], nullptr, nullptr, depth % DIMENSIONS);

	// Get widest axis
	float maxAxisWidth = 0;
	int axisWithMaxWidth = 0;

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

	// Get median point (and sort my median).
	int medianIndex = points.size() / 2;	
	std::nth_element(points.begin(), points.begin() + medianIndex, points.end(), getComparatorForAxis(axisWithMaxWidth));
	Point* medianPoint = points[medianIndex];

	std::vector<Point*> leftPoints(points.begin(), points.begin() + medianIndex);
	// Remove median point from list by skipping it.
	std::vector<Point*> rightPoints(points.begin() + medianIndex + 1, points.end());

	// Recursively go down each branch.
	return new Node(medianPoint, createKdTree(leftPoints, depth + 1), createKdTree(rightPoints, depth + 1), axisWithMaxWidth);
}
