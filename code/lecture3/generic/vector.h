#ifndef VECTOR_H
#define VECTOR_H

#include <iostream>

namespace VectorOperations {

template <typename T>
class Vector2D {
public:
    T x, y;

    // Constructor
    Vector2D(T x, T y) : x(x), y(y) {}

    // Default constructor
    Vector2D() : x(0), y(0) {}

    // Function to add two vectors
    Vector2D addVectors(Vector2D v) {
        return Vector2D(x + v.x, y + v.y);
    }

    // Function to subtract two vectors
    Vector2D subtractVectors(Vector2D v) {
        return Vector2D(x - v.x, y - v.y);
    }

    // Function to display a vector
    void displayVector() {
        std::cout << "(" << x << ", " << y << ")" << std::endl;
    }
};

} // namespace VectorOperations

#endif // VECTOR_H