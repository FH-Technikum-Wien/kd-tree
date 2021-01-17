#include <vector>
#include <stdlib.h> 
#include <chrono>

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

	2.0f, 0.0f, 0.0f,
	3.5f, 1.0f, 0.0f,
	2.5f, 3.0f, 0.0f,
};

unsigned int indices[6]{
	0,1,2,
	3,4,5
};

float* createRandomTriangles(int numberOfTriangles, int range);
unsigned int* getIndexList(unsigned int numberOfVertices);
Ray createRandomRay(int originRange);

int main() {
	
	int numberOfTriangles = 1000;
	float pointRange = 10;
	int numberOfVertices = numberOfTriangles * 3;

	float* randomVertices = createRandomTriangles(numberOfTriangles, pointRange);
	unsigned int* indices = getIndexList(numberOfVertices);


	// Create kd-tree
	auto start = std::chrono::high_resolution_clock::now();
	KdTree kdtree = KdTree(randomVertices, numberOfVertices);
	auto end = std::chrono::high_resolution_clock::now();
	std::cout << "Building time: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " microseconds." << std::endl;
	kdtree.printStatistics();
	//Ray ray = createRandomRay(10);
	Ray ray = Ray(Vector(0, 0, -1), Vector(0, 0, 1), 1000);


	RayHit* rayHit = nullptr;
	start = std::chrono::high_resolution_clock::now();
	kdtree.raycast(ray, rayHit);
	end = std::chrono::high_resolution_clock::now();
	std::cout << "Raycast time: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " microseconds." << std::endl;


	std::cout << "Ray origin: " << ray.origin << " Ray direction: " << ray.direction << std::endl;
	if (rayHit != nullptr) {
		std::cout << "Found a triangle:" << std::endl;
		std::cout << rayHit->triangle->a << std::endl;
		std::cout << rayHit->triangle->b << std::endl;
		std::cout << rayHit->triangle->c << std::endl;

		std::cout << "RayHit at:" << std::endl;
		std::cout << rayHit->position << std::endl;
	}
	else {
		std::cout << "Nothing hit!" << std::endl;
	}
}

float* createRandomTriangles(int numberOfTriangles, int range)
{
	float* vertices = new float[numberOfTriangles * 9];
	// Create vertices for triangles
	for (int i = 0; i < numberOfTriangles * 9; i++)
	{
		float random = static_cast <float> (mersenneTwisterRand()) / (static_cast <float> (RAND_MAX / range));
		vertices[i] = random;
	}
	return vertices;
}

unsigned int* getIndexList(unsigned int numberOfVertices)
{
	unsigned int* indices = new unsigned int[numberOfVertices];
	// Create vertices for triangles
	for (unsigned int i = 0; i < numberOfVertices; i++)
	{
		indices[i] = i;
	}
	return indices;
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
	return Ray(origin, direction, 1000);
}