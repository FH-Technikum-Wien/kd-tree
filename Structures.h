#pragma once

struct Vector
{
	Vector(float values[3]) : values(values) {}
	Vector(float x, float y, float z) : values(new float[3]{ x,y,z }) {}
	~Vector() {
		delete[] values;
		values = nullptr;
	}

	float operator[](int i) const { return values[i]; }
	float& operator[](int i) { return values[i]; }

	float* values;
};

struct Point
{
	Point(Vector pos, Triangle* triangle) : pos(pos), triangle(triangle) {}

	// Triangle this point belongs to
	Triangle* triangle;
	Vector pos;
};


struct Triangle
{
	Triangle(Vector a, Vector b, Vector c) : a(new Point(a, this)), b(new Point(b, this)), c(new Point(c, this)) {}

	~Triangle() {
		delete a;
		delete b;
		delete c;
	}

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

	// Axis in which this splitting plane lies
	int axis = 0;
	// Point of this splitting plane
	Point* point;
	// Splitting plane to the left and right
	Node* left;
	Node* right;
};