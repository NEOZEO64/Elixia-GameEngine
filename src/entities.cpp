#include "entities.hpp"

Entity::Entity(const Vector2D &pos, const Vector2D &size,
               const Vector2D &normViewAngle, SDL_Color color,
               SDL_Renderer *renderer)
    : pos(pos), size(size), color(color), renderer(renderer) {
  offsetToCenter = size * 0.5;
  vel.set(0, 0);
  normViewAng = normViewAngle.norm();
  LOG("New entity appeared");
}

void Entity::update(Map *map, TileSet *tileSet) {
  if ( // check if 4 corners at the new position are inside any part of the map
      !map->isColliding(tileSet, pos.getAdded(vel.x, 0).getAdded(
                                     offsetToCenter.x, offsetToCenter.y)) &&
      !map->isColliding(tileSet, pos.getAdded(vel.x, 0).getAdded(
                                     -offsetToCenter.x, offsetToCenter.y)) &&
      !map->isColliding(tileSet, pos.getAdded(vel.x, 0).getAdded(
                                     offsetToCenter.x, -offsetToCenter.y)) &&
      !map->isColliding(tileSet, pos.getAdded(vel.x, 0).getAdded(
                                     -offsetToCenter.x, -offsetToCenter.y))) {
    pos.x += vel.x;
  } else {
    if (BOUNCE) {
      vel.x *= -0.4;
    } else {
      vel.x = 0;
      pos.x = (map->tileWidth) * ((int)pos.x) / (map->tileWidth);
    }
  }

  if ( // check if 4 corners at the new position are inside any part of the map
      !map->isColliding(tileSet, pos.getAdded(0, vel.y).getAdded(
                                     offsetToCenter.x, offsetToCenter.y)) &&
      !map->isColliding(tileSet, pos.getAdded(0, vel.y).getAdded(
                                     -offsetToCenter.x, offsetToCenter.y)) &&
      !map->isColliding(tileSet, pos.getAdded(0, vel.y).getAdded(
                                     offsetToCenter.x, -offsetToCenter.y)) &&
      !map->isColliding(tileSet, pos.getAdded(0, vel.y).getAdded(
                                     -offsetToCenter.x, -offsetToCenter.y))) {
    pos.y += vel.y;
  } else {
    if (BOUNCE) {
      vel.y *= -0.4;
    } else {
      vel.y = 0;
      pos.y = (map->tileHeight) * ((int)pos.y) / (map->tileHeight);
    }
  }

  vel *= 0.99f;
}

void Entity::render(const Vector2D &offset) {
  Vector2D tempPos = pos - offsetToCenter + offset;
  drawRect(renderer, tempPos, size, color, false);
}