#include "vector2D.hpp"

#define PI 3.1415926535

Vector2D::Vector2D() {
    x = 0;
    y = 0;
}

Vector2D::Vector2D(double x, double y) : x(x), y(y) {}

void Vector2D::set(double ix, double iy) {
    x = ix;
    y = iy;
}

/*
Vector2D (initializer_list<double>) {
    if (initializer_list.size()>2) {
        cout << "Warning: Initializer list too long" << endl;
    }
    this.x = initializer_list[0];
    this.y = initializer_list[1];
}
*/

void Vector2D::operator=(Vector2D const& vector) {
    x = vector.x;
    y = vector.y;
}

void Vector2D::operator+=(Vector2D const& vector) {
    x += vector.x;
    y += vector.y;
}

void Vector2D::operator-=(Vector2D const& vector) {
    x -= vector.x;
    y -= vector.y;
}

void Vector2D::operator*=(double scalar) {
    x *= scalar;
    y *= scalar;
}

void Vector2D::operator/=(double scalar) {
    x /= scalar;
    y /= scalar;
}

Vector2D Vector2D::getAdded(double ix, double iy) const {
    Vector2D temp(x + ix, y + iy);
    return temp;
}

Vector2D Vector2D::norm() const {
    Vector2D tempNormVector(x, y);
    tempNormVector*=(1.0/len());
    return tempNormVector;
}

double Vector2D::len() const {
    return sqrt(pow(x, 2) + pow(y, 2));
}

double Vector2D::ang() const { // to an vector (1,0) otherwise a vector facing east
    return acos((x / len()) / 180 * PI);
}

double Vector2D::dist(Vector2D const& vector) const {
    return sqrt(pow(x - vector.x, 2) + pow(y - vector.y, 2));
}

double Vector2D::sc(Vector2D const& vector) const {
    return x * vector.x + y * vector.y;
}

string Vector2D::to_string() const {
    string tempString = string("(") + ::to_string(x) + string(" / ") + ::to_string(y) + ")";
    return tempString;
}


Vector2D operator+(Vector2D const& thisVector, Vector2D const& otherVector) {
    Vector2D tempVector(thisVector.x, thisVector.y);
    tempVector += otherVector;
    return tempVector;
}
Vector2D operator+(Vector2D const& thisVector) {
    return thisVector;
}


Vector2D operator-(Vector2D const& thisVector, Vector2D const& otherVector) {
    Vector2D tempVector(thisVector.x, thisVector.y);
    tempVector -= otherVector;
    return tempVector;
}
Vector2D operator-(Vector2D const& thisVector) {
    Vector2D tempVector(0.0,0.0);
    tempVector -= thisVector;
    return tempVector;
}


Vector2D operator*(Vector2D const& thisVector, double x) {
    Vector2D tempVector(thisVector.x, thisVector.y);
    tempVector *= x;
    return tempVector;
}
Vector2D operator*(double x, Vector2D const& thisVector) {
    Vector2D tempVector(thisVector.x, thisVector.y);
    tempVector *= x;
    return tempVector;
}
Vector2D operator/(Vector2D const& thisVector, double x) {
    Vector2D tempVector(thisVector.x, thisVector.y);
    tempVector /= x;
    return tempVector;
}

Vector2D operator/(double x, Vector2D const& thisVector) {
    Vector2D tempVector(thisVector.x, thisVector.y);
    tempVector *= x;
    return tempVector;
}

ostream& operator<<(ostream& out, Vector2D const& thisVector) {
    out << thisVector.to_string();
    return out;
}