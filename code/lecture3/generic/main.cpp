#include "vector.h"

int main() {
    VectorOperations::Vector2D<int> v1(1, 2);
    VectorOperations::Vector2D<int> v2(3, 4);

    std::cout << "Vector 1: ";
    v1.displayVector();

    std::cout << "Vector 2: ";
    v2.displayVector();

    VectorOperations::Vector2D<int> v3 = v1.addVectors(v2);
    std::cout << "v1 + v2 = ";
    v3.displayVector();

    VectorOperations::Vector2D<int> v4 = v2.subtractVectors(v1);
    std::cout << "v2 - v1 = ";
    v4.displayVector();

    VectorOperations::Vector2D<double> v5(1.1, 2.2);
    VectorOperations::Vector2D<double> v6(3.3, 4.4);

    std::cout << "Vector 5: ";
    v5.displayVector();

    std::cout << "Vector 6: ";
    v6.displayVector();

    VectorOperations::Vector2D<double> v7 = v5.addVectors(v6);
    std::cout << "v5 + v6 = ";
    v7.displayVector();

    return 0;
}