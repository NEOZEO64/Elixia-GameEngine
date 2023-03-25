#include "rapidxml.hpp"
#include "vector2D.hpp"
#include <SDL2/SDL.h> // general SDL graphics, sound, events library
#include <SDL2/SDL_image.h>
#include <fmt/core.h> //to format strings
#include <fstream>
#include <iostream>
#include <vector>

using namespace rapidxml;
using namespace std;

class TileSet {
private:
  SDL_Renderer *renderer;
  SDL_Surface *sourceImageSet;
  SDL_Texture *sourceTexSet;
  string path;
  string imagePath;
  Vector2D tileSize;
  Vector2D imageSize;
  Vector2D tileRange; // tileRows & tileColumns
  unsigned int tileCount;

  string properties;
  string errors;

public:
  TileSet(string ipath, SDL_Renderer *renderer);
  ~TileSet();

  void renderTest();
  SDL_Rect getRectOfID(short ID);
  void renderTile(short ID, const Vector2D &pos, const Vector2D &size);
  string getProperties();
  string getErrors();
};