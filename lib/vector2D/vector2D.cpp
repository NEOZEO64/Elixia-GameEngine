#include "vector2D.h"
#include <cmath>

#define PI 3.1415926535

Vector2D::Vector2D() {}

Vector2D::Vector2D(double ix, double iy) {
    x = ix;
    y = iy;
}

void Vector2D::set(double ix, double iy) {
    x = ix;
    y = iy;
}

void Vector2D::set(Vector2D newVector) {
    x = newVector.x;
    y = newVector.y;
}

void Vector2D::add(Vector2D vector) {
    x += vector.x;
    y += vector.y;
}

void Vector2D::add(double ix, double iy) {
    x += ix;
    y += iy;
}

void Vector2D::subt(Vector2D vector) {
    x -= vector.x;
    y -= vector.y;
}

Vector2D Vector2D::getAdded(Vector2D vector) {
    Vector2D tempSum(x + vector.x, y + vector.y);
    return tempSum;
}

Vector2D Vector2D::getAdded(double ix, double iy) {
    Vector2D tempSum(x + ix, y + iy);
    return tempSum;
}

Vector2D Vector2D::getSubt(Vector2D vector) {
    Vector2D tempSum(x - vector.x, y - vector.y);
    return tempSum;
}

void Vector2D::multWithScalar(double scalar) {
    x *= scalar;
    y *= scalar;
}

void Vector2D::multWithVector(Vector2D vector) {
    x *= vector.x;
    y *= vector.y;
}

void Vector2D::multWithVector(double ix, double iy) {
    x *= ix;
    y *= iy;
}

Vector2D Vector2D::getNorm() {
    Vector2D tempNormVector(x, y);
    tempNormVector.normalize();
    return tempNormVector;
}

void Vector2D::normalize() {
    multWithScalar(1.0f / getLen());
}

double Vector2D::getLen() {
    return sqrt(pow(x, 2) + pow(y, 2));
}

double Vector2D::getAngle() { // to an vector (1,0) otherwise a vector facing east
    return acos((x / getLen()) / 180 * PI);
}

double Vector2D::getDist(Vector2D vector) {
    return sqrt(pow(x - vector.x, 2) + pow(y - vector.y, 2));
}

Vector2D Vector2D::getDiffVector(Vector2D vector) { // from this vector to vector in parameter
    Vector2D tempDiffVector(vector.x - x, vector.y - y);
    return tempDiffVector;
}

Vector2D Vector2D::getOppositeVector() {
    Vector2D tempOppositeVector(-x, -y);
    return tempOppositeVector;
}