#include <vector>

#include "KdTree.h"


int main() {
	// Test triangles
	float vertices[18]{
		0.0f, 0.0f, 0.0f,
		2.0f, 0.0f, 0.0f,
		1.0f, 2.0f, 0.0f,

		1.5f, 1.0f, 0.0f,
		3.5f, 1.0f, 0.0f,
		2.5f, 3.0f, 0.0f,
	};

	KdTree kdtree = KdTree(vertices, 2);
	kdtree.Print();
}