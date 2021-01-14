#pragma once

#include <iostream>

namespace KdStructs {

	// Because C++ is a procedural programming language
	struct Triangle;


	struct Vector
	{
		Vector(float values[3]) : values(values) {}
		Vector(float x, float y, float z) : values(new float[3]{ x,y,z }) {}
		Vector(const Vector& vector) : values(new float[3]{ vector[0], vector[1], vector[2] }) {}
		~Vector() {
			delete[] values;
			values = nullptr;
		}

		float operator[](int i) const { return values[i]; }
		float& operator[](int i) { return values[i]; }
		Vector operator+(const Vector& other) const { return Vector(values[0] + other[0], values[1] + other[1], values[2] + other[2]); }
		Vector operator-(const Vector& other) const { return Vector(values[0] - other[0], values[1] - other[1], values[2] - other[2]); }
		Vector operator*(float scalar) const { return Vector(values[0] * scalar, values[1] * scalar, values[2] * scalar); }

		Vector cross(const Vector& other) const {
			return Vector(values[1] * other[2] - values[2] * other[1], -(values[0] * other[2] - values[2] * other[0]), values[0] * other[1] - values[1] * other[0]);
		}

		float dot(const Vector& other) const { return values[0] * other[0] + values[1] * other[1] + values[2] * other[2]; }

		void print() { std::cout << "{" << values[0] << "," << values[1] << "," << values[2] << "}" << std::endl; }

		float* values = nullptr;
	};

	inline std::ostream& operator<<(std::ostream& str, const Vector& vector) {
		return str << "{" << vector.values[0] << "," << vector.values[1] << "," << vector.values[2] << "}";
	}

	struct Point
	{
		Point(Vector pos, Triangle* triangle) : pos(pos), triangle(triangle) {}

		Vector pos = nullptr;

		// Triangle this point belongs to
		Triangle* triangle = nullptr;
	};


	struct Triangle
	{
		Triangle(Vector a, Vector b, Vector c) : a(new Point(a, this)), b(new Point(b, this)), c(new Point(c, this)) {}

		std::vector<Point*> getPoints() { return std::vector<Point*> {a, b, c}; }

		Point* a = nullptr;
		Point* b = nullptr;
		Point* c = nullptr;
	};

	/// <summary>
	/// Aka. Partition, Cell
	/// Node storing kd-tree
	/// </summary>
	struct Node
	{
		Node(Point* point, Node* left, Node* right, int axis, Vector max, Vector min) : point(point), left(left), right(right), axis(axis), max(max), min(min) {}

		// Point of this splitting plane
		Point* point = nullptr;
		// Splitting plane to the left and right
		Node* left = nullptr;
		Node* right = nullptr;

		// Axis in which this splitting plane lies
		int axis = 0;

		// Splitting plane limits for each axis
		// Defines where splitting plane for this node starts and ends
		// 0 -> undefined/endless
		Vector max = nullptr;
		Vector min = nullptr;
	};


	struct Ray
	{
		Ray(Vector origin, Vector direction) : origin(origin), direction(direction) {}

		Vector origin = nullptr;
		Vector direction = nullptr;
	};

	struct RayHit
	{
		RayHit(Triangle* triangle, Vector position) : triangle(triangle), position(position) {}

		Triangle* triangle = nullptr;
		Vector position = nullptr;
	};
}