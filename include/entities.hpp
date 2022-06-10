#pragma once

#include "macros.hpp"
#include "mySDL.hpp"
#include "tileLib.hpp"
#include "vector2D.hpp"
#include <SDL2/SDL.h> // general SDL graphics, sound, events library
#include <SDL2/SDL_image.h>
#include <fmt/core.h> //to format strings
#include <iostream>

class Entity {
public:
  Vector2D pos, vel, size, normViewAng, offsetToCenter;
  vector<Vector2D> hitBox;
  SDL_Color color;
  SDL_Renderer *renderer;

  Entity(const Vector2D &pos, const Vector2D &size,
         const Vector2D &normViewAngle, SDL_Color color,
         SDL_Renderer *renderer);

  bool isColliding(Map *map, TileSet *tileSet, Vector2D &offset);

  // check if move is possible (return true or false) and if possible move and
  // update hitbox
  void move(Map *map, TileSet *tileSet, Vector2D &path);

  void update(Map *map, TileSet *tileSet);

  void render(const Vector2D &offset);

  void renderDebug(const Vector2D &offset);
};