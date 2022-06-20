#include "entities.hpp"

Entity::Entity(Map *map, TileSet *tileSet, const Vector2D &pos,
               const Vector2D &size, const Vector2D &normViewAngle,
               SDL_Color color, SDL_Renderer *renderer)
    : pos(pos), size(size), color(color), renderer(renderer), map(map),
      tileSet(tileSet) {
  hitBox.push_back(Vector2D(0, 0));
  hitBox.push_back(Vector2D(size.x, 0));
  hitBox.push_back(Vector2D(0, size.y));
  hitBox.push_back(Vector2D(size.x, size.y));

  vel.set(0, 0);
  normViewAng = normViewAngle.norm();
  LOG("New entity appeared");
}

void Entity::move(Vector2D &path) {
  Vector2D tempVel(path.x, 0);
  if (!isColliding(2, tempVel)) {
    pos.x += path.x;
  } else {
    path.x *= -0.4 * (float)BOUNCE;
    if (!BOUNCE) {
      pos.x = (tileSet->tileSize.x) * ((int)pos.x) / (tileSet->tileSize.x);
    }
  }

  tempVel.set(0, vel.y);
  if (!isColliding(2, tempVel)) {
    pos.y += path.y;
  } else {
    path.y *= -0.4 * (float)BOUNCE;
    if (!BOUNCE) {
      pos.y = (tileSet->tileSize.y) * ((int)pos.y) / (tileSet->tileSize.y);
    }
  }
}

bool Entity::isColliding(unsigned short layer, Vector2D &offset) {
  for (Vector2D &point : hitBox) {
    Vector2D tempV = (pos + point + offset) / tileSet->tileSize;

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

void Entity::render(const Vector2D &tileRenderSize, const Vector2D &offset) {
  drawRect(renderer, pos * tileRenderSize / tileSet->tileSize + offset,
           size * tileRenderSize / tileSet->tileSize, color, false);
}

void Entity::renderDebug(const Vector2D &tileRenderSize,
                         const Vector2D &offset) {
  Vector2D dotSize(3, 3);
  SDL_Color debugColor = {255, 0, 120, 255};
  for (Vector2D &point : hitBox) {
    drawRect(renderer,
             offset + (pos + point) * tileRenderSize / tileSet->tileSize -
                 dotSize / 2,
             dotSize * tileRenderSize / tileSet->tileSize, debugColor, false);
  }
  drawLine(
      renderer, offset + (pos + size / 2) * tileRenderSize / tileSet->tileSize,
      offset + (pos + size / 2) * tileRenderSize / tileSet->tileSize + vel * 5,
      debugColor);
  // 5 is for length of the velocity line
}