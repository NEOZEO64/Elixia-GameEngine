#include "entities.hpp"

Entity::Entity(const Vector2D &pos, const Vector2D &size,
               const Vector2D &normViewAngle, SDL_Color color,
               SDL_Renderer *renderer)
    : pos(pos), size(size), color(color), renderer(renderer) {
  hitBox.push_back(Vector2D(0, 0));
  hitBox.push_back(Vector2D(size.x, 0));
  hitBox.push_back(Vector2D(0, size.y));
  hitBox.push_back(Vector2D(size.x, size.y));

  vel.set(0, 0);
  normViewAng = normViewAngle.norm();
  LOG("New entity appeared");
}

void Entity::move(Map *map, TileSet *tileSet, Vector2D &path) {
  Vector2D tempVel(path.x, 0);
  if (!isColliding(map, tileSet, tempVel)) {
    pos.x += path.x;
  } else {
    path.x *= -0.4 * (float)BOUNCE;
    if (!BOUNCE) {
      pos.x = (map->tileWidth) * ((int)pos.x) / (map->tileWidth);
    }
  }

  tempVel.set(0, vel.y);
  if (!isColliding(map, tileSet, tempVel)) {
    pos.y += path.y;
  } else {
    path.y *= -0.4 * (float)BOUNCE;
    if (!BOUNCE) {
      pos.y = (map->tileHeight) * ((int)pos.y) / (map->tileHeight);
    }
  }
}

bool Entity::isColliding(Map *map, TileSet *tileSet, Vector2D &offset) {
  for (Vector2D &point : hitBox) {
    if (map->isColliding(tileSet, pos + point + offset)) {
      return true;
    }
  }
  return false;
}

void Entity::update(Map *map, TileSet *tileSet) {
  move(map, tileSet, vel);
  vel *= 0.99f;
}

void Entity::render(const Vector2D &offset) {
  drawRect(renderer, offset + pos, size, color, false);
}

void Entity::renderDebug(const Vector2D &offset) {
  Vector2D dotSize(3, 3);
  SDL_Color debugColor = {255, 0, 120, 255};
  for (Vector2D &point : hitBox) {
    drawRect(renderer, offset + pos + point - dotSize / 2, dotSize, debugColor,
             false);
  }
  drawLine(renderer, offset + pos + size / 2, offset + pos + size / 2 + vel * 5,
           debugColor);
  // 5 is for length of the velocity line
}