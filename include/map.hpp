#pragma once

#include "rapidxml.hpp"
#include "settings.hpp"
#include "tileset.hpp"
#include "vector2D.hpp"
#include <fmt/core.h> //to format strings
#include <fstream>
#include <iostream>
#include <vector>

using namespace rapidxml;
using namespace std;

class Map {
private:
  string path;
  string properties;
  string tilesText;
  string errors;
  TileSet *tileSet;

public:
  Vector2D size;
  unsigned short layerCount;
  short *tiles;

  Map(string ipath);
  short getID(const Vector2D &pos, unsigned short layer);
  string getProperties();
  string getTilesText();
  string getErrors();

  bool loadTileSet(string sourceFile);
};