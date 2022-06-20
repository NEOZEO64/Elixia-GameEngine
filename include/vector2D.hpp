#pragma once

#include <iostream>
#include <math.h>

using namespace std;

struct Vector2D {
  double x, y;
  Vector2D();
  Vector2D(double x, double y);
  // Vector2D (initializer_list<double>);

  void set(double x, double y);

  void operator=(Vector2D const &vector);
  void operator+=(Vector2D const &vector);
  void operator-=(Vector2D const &vector);
  void operator*=(double scalar);
  void operator*=(Vector2D const &vector); // my own implementation
  void operator/=(double scalar);
  void operator/=(Vector2D const &vector); // my own implementation

  Vector2D norm() const;
  double len() const;
  double ang() const;
  double sc(Vector2D const &vector) const;
  double dist(Vector2D const &vector) const;
  string to_string() const;
};

Vector2D operator+(Vector2D const &thisVector, Vector2D const &otherVector);
Vector2D operator+(Vector2D const &thisVector);

Vector2D operator-(Vector2D const &thisVector, Vector2D const &otherVector);
Vector2D operator-(Vector2D const &thisVector);

Vector2D operator*(Vector2D const &thisVector, double x);
Vector2D operator*(double x, Vector2D const &thisVector);
Vector2D operator*(Vector2D const &thisVector,
                   Vector2D const &otherVector); // my own implementation

Vector2D operator/(Vector2D const &thisVector, double x);
Vector2D operator/(double x, Vector2D const &thisVector);
Vector2D operator/(Vector2D const &thisVector,
                   Vector2D const &otherVector); // my own implementation

ostream &operator<<(ostream &out, Vector2D const &vector);