#include "entity.hpp"

Entity::Entity(Map *map, const Vector2D &pos, const Vector2D &size,
               const Vector2D &normViewAngle, vector<uint8_t> color)
    : pos(pos), size(size), color(color), map(map) {
  properties = "";
  hitBox.push_back(Vector2D(0, 0));
  hitBox.push_back(Vector2D(size.x, 0));
  hitBox.push_back(Vector2D(0, size.y));
  hitBox.push_back(Vector2D(size.x, size.y));

  vel.set(0, 0);
  normViewAng = normViewAngle.norm();
  properties += "New entity appeared";
}

string Entity::getProperties() { return properties; }

void Entity::move(Vector2D &path) {
  Vector2D tempVel(path.x, 0);
  if (!isColliding(2, tempVel)) {
    pos.x += path.x;
  } else {
    path.x *= -0.4 * (float)BOUNCE;
  }

  tempVel.set(0, vel.y);
  if (!isColliding(2, tempVel)) {
    pos.y += path.y;
  } else {
    path.y *= -0.4 * (float)BOUNCE;
  }
}

bool Entity::isColliding(unsigned short layer, Vector2D &offset) {
  for (Vector2D &point : hitBox) {
    Vector2D tempV = (pos + point + offset);

    if (tempV.x < 0 || tempV.y < 0 || tempV.x > map->size.x ||
        tempV.y > map->size.y || map->getID(tempV, layer) != 0) {
      return true;
    }
  }
  return false;
}

void Entity::update() {
  move(vel);
  vel *= 0.99f;
}