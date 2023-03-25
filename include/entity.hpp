#pragma once

#include "map.hpp"
#include "settings.hpp"
#include "vector2D.hpp"
#include <SDL2/SDL.h> // general SDL graphics, sound, events library
#include <SDL2/SDL_image.h>
#include <fmt/core.h> //to format strings
#include <iostream>
#include <vector>

class Entity {
private:
  string properties;

public:
  Vector2D pos, vel, size, normViewAng;
  vector<Vector2D> hitBox;
  vector<uint8_t> color; // color = {r, g, b}
  Map *map;
  // TileSet *tileSet;

  Entity(Map *map, const Vector2D &pos, const Vector2D &size,
         const Vector2D &normViewAngle, vector<uint8_t> color);

  string getProperties();

  bool isColliding(unsigned short layer, Vector2D &offset);

  // check if move is possible (return true or false) and if possible move and
  // update hitbox
  void move(Vector2D &path);

  void update();
};