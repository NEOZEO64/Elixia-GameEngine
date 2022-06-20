#pragma once

#include "macros.hpp"
#include "mySDL.hpp"
#include "rapidxml.hpp"
#include "vector2D.hpp"
#include <SDL2/SDL.h> // general SDL graphics, sound, events library
#include <SDL2/SDL_image.h>
#include <fmt/core.h> //to format strings
#include <fstream>
#include <iostream>
#include <math.h>
#include <vector>

using namespace rapidxml;
using namespace std;

class TileSet;
class Map;

class Map {
private:
  string properties;
  string errors;

public:
  string path;
  string tileSetPath;

  Vector2D size;
  unsigned short layerCount;
  short *tiles;

  Map(string ipath);

  short getID(const Vector2D &pos, unsigned short layer);
  string getProperties();
  string getErrors();
  void render(TileSet *tileSet, unsigned short layer,
              const Vector2D &tileRenderSize, const Vector2D &gameRenderPos,
              const Vector2D &gameRenderSize);
};

class TileSet {
private:
  string properties;
  string errors;

public:
  string path;
  string imagePath;
  Vector2D tileSize;
  Vector2D imageSize;
  Vector2D tileRange; // tileRows & tileColumns
  unsigned int tileCount;

  SDL_Renderer *renderer;
  SDL_Surface *sourceImageSet;
  SDL_Texture *sourceTexSet;

  TileSet(string ipath, SDL_Renderer *renderer);
  ~TileSet();

  void renderTest();
  SDL_Rect getRectOfID(short ID);
  void renderTile(short ID, const Vector2D &pos, const Vector2D &size);
  string getProperties();
  string getErrors();
};