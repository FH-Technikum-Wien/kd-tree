#include "KdTree.h"

#include <algorithm>

KdTree::KdTree(float* vertices, int numberOfTriangles) : KdTree(getPointList(vertices, numberOfTriangles)){}

KdTree::KdTree(std::vector<Point*> points) {
	root = createKdTree(points, 0);
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

		Triangle triangle = Triangle(a, b, c);
		std::vector<Point*> trianglePoints = triangle.getPoints();
		points.insert(points.end(), trianglePoints.begin(), trianglePoints.end());
	}
	return points;
}

Node* KdTree::createKdTree(std::vector<Point*> points, int depth)
{
	int axis = depth % DIMENSIONS;

	// Get widest axis
	float maxAxisWidth = 0;
	int axisWithMaxWith = 0;

	for (int i = 0; i < DIMENSIONS; i++)
	{
		auto comparator = getComparatorForAxis(i);

		Point* min = *std::min_element(points.begin(), points.end(), comparator);
		Point* max = *std::max_element(points.begin(), points.end(), comparator);

		float axisWidth = max->pos[i] - min->pos[i];

		if (maxAxisWidth < axisWidth) {
			maxAxisWidth = axisWidth;
			axisWithMaxWith = i;
		}
	}
	return nullptr;
}
