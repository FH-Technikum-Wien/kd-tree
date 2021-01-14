#include <vector>
#include <stdlib.h> 

#include "boost/random.hpp"
#include "boost/random/uniform_int.hpp"
#include "boost/random/variate_generator.hpp"

#include "KdTree.h"

using namespace KdStructs;

boost::mt19937 mersenneTwister;
boost::uniform_int<> randomRange(0, RAND_MAX);
boost::variate_generator< boost::mt19937, boost::uniform_int<> > mersenneTwisterRand(mersenneTwister, randomRange);

// Test triangles
float vertices[18]{
	0.0f, 0.0f, 0.0f,
	2.0f, 0.0f, 0.0f,
	1.0f, 2.0f, 0.0f,

	1.5f, 1.0f, 0.0f,
	3.5f, 1.0f, 0.0f,
	2.5f, 3.0f, 0.0f,
};

float* createRandomTriangles(int numberOfTriangles, int range);
Ray createRandomRay(int originRange);

int main() {
	
	int numberOfTriangles = 18;
	float pointRange = 1000;

	//float* randomVertices = createRandomTriangles(numberOfTriangles, pointRange);

	KdTree kdtree = KdTree(vertices, 2);
	kdtree.print();

	//Ray ray = createRandomRay(10);
	Ray ray = Ray(Vector(0, 0, -1), Vector(0, 0, 1));
	std::cout << "Ray origin: " << ray.origin << " Ray direction: " << ray.direction << std::endl;

	RayHit* rayHit = kdtree.raycast(ray, 1);

	if (rayHit != nullptr) {
		std::cout << "Found a triangle:" << std::endl;
		std::cout << rayHit->triangle->a->pos << std::endl;
		std::cout << rayHit->triangle->b->pos << std::endl;
		std::cout << rayHit->triangle->c->pos << std::endl;

		std::cout << "RayHit at:" << std::endl;
		std::cout << rayHit->position << std::endl;
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