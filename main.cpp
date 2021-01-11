#include <vector>
#include <stdlib.h> 

#include "boost/random.hpp"
#include "boost/random/uniform_int.hpp"
#include "boost/random/variate_generator.hpp"

#include "KdTree.h"

boost::mt19937 mersenneTwister;
boost::uniform_int<> randomRange(0, RAND_MAX);
boost::variate_generator< boost::mt19937, boost::uniform_int<> > mersenneTwisterRand(mersenneTwister, randomRange);

// Test triangles
float vertices[27]{
	0.0f, 0.0f, 0.0f,
	2.0f, 0.0f, 0.0f,
	1.0f, 2.0f, 0.0f,

	1.5f, 1.0f, 0.0f,
	3.5f, 1.0f, 0.0f,
	2.5f, 3.0f, 0.0f,

	5.5f, 4.0f, 5.0f,
	8.5f, 1.0f, 4.0f,
	4.5f, 3.0f, 3.0f,
};

float* createRandomTriangles(int numberOfTriangles, int range);
Ray createRandomRay(int originRange);

int main() {
	
	int numberOfTriangles = 1000;
	float pointRange = 1000;

	float* randomVertices = createRandomTriangles(numberOfTriangles, pointRange);

	KdTree kdtree = KdTree(randomVertices, numberOfTriangles);
	kdtree.printStatistics();

	Ray ray = createRandomRay(10);
	std::cout << "Ray origin: " << ray.origin << " Ray direction: " << ray.direction << std::endl;

	Triangle* triangle = kdtree.raycast(ray, 1);

	if (triangle != nullptr) {
		std::cout << "Found a triangle!" << std::endl;
		std::cout << triangle->a->pos << std::endl;
		std::cout << triangle->b->pos << std::endl;
		std::cout << triangle->c->pos << std::endl;
	}
	else {
		std::cout << "Nothing here!" << std::endl;
	}
}

float* createRandomTriangles(int numberOfTriangles, int range)
{
	float* vertices = new float[numberOfTriangles * 9];
	srand(1);
	// Create vertices for triangles
	for (int i = 0; i < numberOfTriangles * 9; i++)
	{
		float random = static_cast <float> (mersenneTwisterRand()) / (static_cast <float> (RAND_MAX / range));
		vertices[i] = random;
	}
	return vertices;
}

Ray createRandomRay(int originRange) {
	Vector origin = Vector(
		static_cast <float> (mersenneTwisterRand()) / (static_cast <float> (RAND_MAX / originRange)),
		static_cast <float> (mersenneTwisterRand()) / (static_cast <float> (RAND_MAX / originRange)),
		static_cast <float> (mersenneTwisterRand()) / (static_cast <float> (RAND_MAX / originRange))
	);
	Vector direction = Vector(
		static_cast <float> (mersenneTwisterRand()) / (static_cast <float> (RAND_MAX)),
		static_cast <float> (mersenneTwisterRand()) / (static_cast <float> (RAND_MAX)),
		static_cast <float> (mersenneTwisterRand()) / (static_cast <float> (RAND_MAX))
	);
	return Ray(origin, direction);
}