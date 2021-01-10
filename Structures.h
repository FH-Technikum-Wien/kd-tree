#pragma once

#include <iostream>

// Because C++ is a procedural programming language
struct Triangle;


struct Vector
{
	Vector(float values[3]) : values(values) {}
	Vector(float x, float y, float z) : values(new float[3]{ x,y,z }) {}

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

	float* values;
};

inline std::ostream& operator<<(std::ostream& str, const Vector& vector) {
	return str << "{" << vector.values[0] << "," << vector.values[1] << "," << vector.values[2] << "}";
}

struct Point
{
	Point(Vector pos, Triangle* triangle) : pos(pos), triangle(triangle) {}

	Vector pos;
	// Triangle this point belongs to
	Triangle* triangle;
};


struct Triangle
{
	Triangle(Vector a, Vector b, Vector c) : a(new Point(a, this)), b(new Point(b, this)), c(new Point(c, this)) {}

	std::vector<Point*> getPoints() { return std::vector<Point*> {a, b, c}; }

	Point* a;
	Point* b;
	Point* c;
};

/// <summary>
/// Aka. Partition, Cell
/// Node storing kd-tree
/// </summary>
struct Node
{
	Node(Point* point, Node* left, Node* right, int axis) : point(point), left(left), right(right), axis(axis) {}

	// Point of this splitting plane
	Point* point;
	// Splitting plane to the left and right
	Node* left;
	Node* right;

	// Axis in which this splitting plane lies
	int axis = 0;
};


struct Ray
{
	Ray(Vector origin, Vector direction) : origin(origin), direction(direction) {}

	Vector origin;
	Vector direction;
};