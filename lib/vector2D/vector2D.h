class Vector2D {
public:
  double x, y;

  Vector2D() {}
  Vector2D (double ix, double iy) {}

  void set(double ix, double iy) {}
  void set(Vector2D newVector) {}

  void add(Vector2D vector) {}
  void add(double ix, double iy) {}

  void subt(Vector2D vector) {}

  Vector2D getAdded(Vector2D vector) {}
  Vector2D getAdded(double ix, double iy) {}

  Vector2D getSubt(Vector2D vector) {}

  void multWithScalar(double scalar) {}

  void multWithVector(Vector2D vector) {}
  void multWithVector(double ix, double iy) {}

  Vector2D getNorm() {}

  void normalize() {}

  double getLen() {}
  
  double getAngle() {}

  double getDist(Vector2D vector) {}

  Vector2D getDiffVector(Vector2D vector) {}

  Vector2D getOppositeVector() {}
};