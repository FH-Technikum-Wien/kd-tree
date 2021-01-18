#include <vector>
#include <stdlib.h> 
#include <chrono>
#include <map>

#include "boost/random.hpp"
#include "boost/random/uniform_int.hpp"
#include "boost/random/variate_generator.hpp"

#include "KdTree.h"
#include "objLoader/OBJ_Loader.h"

using namespace KdStructs;

enum class ArgumentType { LOAD, TRIANGLES, POINT_RANGE, INTERACTIVE, VERBOSE, FORCE_SLOW, HELP };

std::map<std::string, ArgumentType> argumentMap{
	{"--load", ArgumentType::LOAD},
	{"-l", ArgumentType::LOAD},
	{"--triangles", ArgumentType::TRIANGLES},
	{"-p", ArgumentType::TRIANGLES},
	{"--range", ArgumentType::POINT_RANGE},
	{"-r", ArgumentType::POINT_RANGE},
	{"--interactive", ArgumentType::INTERACTIVE},
	{"-i", ArgumentType::INTERACTIVE},
	{"--verbose", ArgumentType::VERBOSE},
	{"-v", ArgumentType::VERBOSE},
	{"--slow", ArgumentType::FORCE_SLOW},
	{"-s", ArgumentType::FORCE_SLOW},
	{"--help", ArgumentType::HELP},
};

//Argument handling functions
void handleArguments(int argc, char* argv[]);
void showWrongArguments();
void showHelp();

void handleRayHit(RayHit* rayHit);
float* createRandomTriangles(int numberOfTriangles, int range);
unsigned int* getIndexList(unsigned int numberOfVertices);
Ray createRandomRay(int originRange);

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


std::string filePath = "";
int triangleAmount = 1000;
int pointRange = 1000;
bool interactive = false;
bool verbose = false;
bool forceSlow = false;

int main(int argc, char* argv[])
{
	handleArguments(argc, argv);

	KdTree* kdtree = nullptr;

	// Load file.
	if (filePath != "") {
		std::cout << "\n[*] Loading file" << std::endl;
		objl::Loader loader;
		if (loader.LoadFile(filePath)) {
			std::cout << "[->] Done!" << std::endl;
			if (verbose) {
				std::cout << "Vertices: " << loader.LoadedVertices.size() << std::endl;
				std::cout << "Indices: " << loader.LoadedIndices.size() << std::endl;
			}
		}
		else {
			std::cout << "[X] Could not load file!" << std::endl;
			std::exit(1);
		}

		std::vector<float> vertices;
		for (const objl::Vertex& vertex : loader.LoadedVertices) {
			vertices.push_back(vertex.Position.X);
			vertices.push_back(vertex.Position.Y);
			vertices.push_back(vertex.Position.Z);
		}

		std::vector<unsigned int> indices;
		for (unsigned int index : loader.LoadedIndices) {
			indices.push_back(index);
		}

		// Create kd-tree.
		std::chrono::steady_clock::time_point start, end;
		if (forceSlow) {
			std::cout << "\n[*] Building kd-tree (slow)" << std::endl;
			start = std::chrono::high_resolution_clock::now();
			kdtree = new KdTree(vertices.data(), vertices.size() / 3);
			end = std::chrono::high_resolution_clock::now();
		}
		else {
			std::cout << "\n[*] Building kd-tree" << std::endl;
			start = std::chrono::high_resolution_clock::now();
			kdtree = new KdTree(vertices.data(), vertices.size() / 3, indices.data(), indices.size());
			start = std::chrono::high_resolution_clock::now();
		}

		std::cout << "[->] Done!" << std::endl;
		std::cout << "Building time: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " microseconds." << std::endl;

	}
	else {
		int numberOfVertices = triangleAmount * 3;
		float* randomVertices = createRandomTriangles(triangleAmount, pointRange);

		// Create kd-tree.
		std::cout << "\n[*] Building kd-tree (slow)" << std::endl;
		auto start = std::chrono::high_resolution_clock::now();
		kdtree = new KdTree(randomVertices, numberOfVertices);
		auto end = std::chrono::high_resolution_clock::now();
		std::cout << "[->] Done!" << std::endl;
		std::cout << "Building time: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " microseconds." << std::endl;
	}

	if (verbose)
		kdtree->printStatistics();

	if (interactive) {
		std::cout << "\n[->] Interaction enabled!" << std::endl;
		std::cout << "You can shoot rays now. Example: 0,0,0;1,0,0 (<origin>,<direction>). You can also shhot a random ray by simply typing 'r'." << std::endl;
		while (true)
		{
			std::cout << "Ray: ";

			KdStructs::Ray ray = KdStructs::Ray(Vector(0,0,0), Vector(0,0,0), 1000);
			if (std::cin.peek() == 'r') {
				std::string temp;
				std::getline(std::cin, temp);
				ray = createRandomRay(pointRange);
			}
			else {
				std::string input;
				// Get origin.
				std::getline(std::cin, input, ',');
				float x = std::stof(input);
				std::getline(std::cin, input, ',');
				float y = std::stof(input);
				std::getline(std::cin, input, ';');
				float z = std::stof(input);
				// Get direction.
				std::getline(std::cin, input, ',');
				float dx = std::stof(input);
				std::getline(std::cin, input, ',');
				float dy = std::stof(input);
				std::getline(std::cin, input);
				float dz = std::stof(input);

				ray = Ray(Vector(x, y, z), Vector(dx, dy, dz), 1000);
			}

			RayHit* rayHit = nullptr;
			// Casting ray
			std::cout << "\n[*] Casting Ray." << std::endl;
			auto start = std::chrono::high_resolution_clock::now();
			kdtree->raycast(ray, rayHit);
			auto end = std::chrono::high_resolution_clock::now();
			std::cout << "Raycast time: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " microseconds." << std::endl;
			handleRayHit(rayHit);
		}
	}
	else {
		Ray ray = createRandomRay(pointRange);
		RayHit* rayHit = nullptr;

		// Casting ray
		std::cout << "\n[*] Casting Ray." << std::endl;
		if (verbose)
			std::cout << "Ray origin: " << ray.origin << " Ray direction: " << ray.direction << std::endl;
		auto start = std::chrono::high_resolution_clock::now();
		kdtree->raycast(ray, rayHit);
		auto end = std::chrono::high_resolution_clock::now();
		handleRayHit(rayHit);
		std::cout << "Raycast time: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " microseconds." << std::endl;
	}
}

#pragma region Argument Handling
void handleArguments(int argc, char* argv[])
{
	// Skip first argument (it's the .exe).
	for (unsigned int i = 1; i < argc; i++)
	{
		std::string argType = std::string(argv[i]);
		std::string argData = i + 1 < argc ? std::string(argv[i + 1]) : std::string();
		std::string argData2 = i + 2 < argc ? std::string(argv[i + 2]) : std::string();
		int randomModeInt = 0;
		switch (argumentMap[argType])
		{
		case ArgumentType::LOAD:
			if (argData.empty())
				showWrongArguments();
			filePath = argData;
			i++;
			break;
		case ArgumentType::TRIANGLES:
			if (argData.empty())
				showWrongArguments();
			triangleAmount = std::stoi(argData);
			i++;
			break;
		case ArgumentType::POINT_RANGE:
			if (argData.empty())
				showWrongArguments();
			pointRange = std::stoi(argData);
			i++;
			break;
		case ArgumentType::INTERACTIVE:
			interactive = true;
			break;
		case ArgumentType::VERBOSE:
			verbose = true;
			break;
		case ArgumentType::FORCE_SLOW:
			forceSlow = true;
			break;
		case ArgumentType::HELP:
			showHelp();
			std::exit(0);
			break;
		default:
			showWrongArguments();
			break;
		}
	}
}

void showWrongArguments()
{
	std::cerr << "Wrong arguments! Use --help to show possible arguments." << std::endl;
	std::exit(1);
}

void showHelp()
{
	std::cout << "Arguments:" << std::endl;
	std::cout << "--load [-l] <file>                                 -> Filename to read from." << std::endl;
	std::cout << "--triangles [-p] <numberOfTriangles>               -> Number of random triangles to be generated." << std::endl;
	std::cout << "--range [-r] <vertexRange>                         -> Range in which the random vertices will be generated." << std::endl;
	std::cout << "--interactive [-i]                                 -> Enables 'interactive-mode' allowing to define custom rays." << std::endl;
	std::cout << "--verbose [-v]                                     -> Prints out additional information." << std::endl;
	std::cout << "--slow [-s]                                        -> Uses a slow procedure to check and merge same vertices." << std::endl;
	std::cout << "--help                                             -> Prints out this message." << std::endl;
	std::cout << std::endl;
}
#pragma endregion

void handleRayHit(RayHit* rayHit) {
	if (rayHit != nullptr) {
		std::cout << "[->] Hit at: ";
		std::cout << rayHit->position << std::endl;
	}
	else {
		std::cout << "[->] Nothing hit!" << std::endl;
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