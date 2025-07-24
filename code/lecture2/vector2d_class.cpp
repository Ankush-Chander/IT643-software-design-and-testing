#include <iostream>

class Vector2D {

public:
    float x, y;
    Vector2D(float x = 0, float y = 0){
        this->x = x;
        this->y = y;
    }


    // Function to add two vectors
    Vector2D addVectors(Vector2D v) const {
        return Vector2D(x + v.x, y + v.y);
    }

    // Function to subtract two vectors
    Vector2D subtractVectors(Vector2D v) {
        return Vector2D(x - v.x, y - v.y);
    }

    // Function to display a vector
    void displayVector(){
        std::cout << "(" << x << ", " << y << ")" << std::endl;
    }
};

int main() {
    Vector2D v1(3.0, 4.0);
    Vector2D v2(1.0, 2.0);

    std::cout << "Vector 1: ";
    v1.displayVector();
    
    std::cout << "Vector 2: ";
    v2.displayVector();

    Vector2D sum = v1.addVectors(v2);
    std::cout << "Sum: ";
    sum.displayVector();

    Vector2D difference = v1.subtractVectors(v2);
    std::cout << "Difference: ";
    difference.displayVector();

    return 0;
}