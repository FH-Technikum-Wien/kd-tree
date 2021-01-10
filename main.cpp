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

	Ray ray = Ray(Vector(2, 1, -10), Vector(0, 0, 1));

	Triangle* triangle = kdtree.raycast(ray, 1);

	if (triangle != nullptr) {
		std::cout << "Found triangle!" << std::endl;
		std::cout << triangle->a->pos << std::endl;
		std::cout << triangle->b->pos << std::endl;
		std::cout << triangle->c->pos << std::endl;
	}
	else {
		std::cout << "Nothing here!" << std::endl;
	}
}