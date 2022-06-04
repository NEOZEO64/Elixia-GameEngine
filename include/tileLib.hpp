#include <iostream>
#include <fstream>
#include <vector>
#include "vector2D.hpp"
#include "rapidxml.hpp"
#include <SDL2/SDL.h> // general SDL graphics, sound, events library
#include <SDL2/SDL_image.h>
#include <math.h>
#include "macros.hpp"
#include <fmt/core.h> //to format strings

using namespace rapidxml;
using namespace std;

class TileSet;
class Map;

class Map {
private:
  xml_document<> doc;
  xml_node<>* mapNode; //root node
  xml_node<>* tileSetNode; //element node
  xml_node<>* tempLayerNode; //element node
  xml_node<>* tempDataNode; //sub-element node

  ifstream xmlTilesFile;
  vector<char> buffer;

public:
  string path;
  string tileSetPath;

  int width, height;
  int layerCount;
  int tileWidth, tileHeight;
  int mapWidth, mapHeight;
  short* tiles; // = new int[m * n];

  Map(string ipath, int iTileWidth);

  short getID(int tx, int ty, int layer);

  bool isColliding(TileSet* tileSet,const Vector2D& pos);

  void logTiles();

  void logProperties();

  void render(TileSet* tileSet, int layer, const Vector2D& offset);
};

class TileSet {
public:
  string path;
  string imagePath;
  int tileWidth, tileHeight;
  int imageWidth, imageHeight;
  int tileColumns;
  int tileRows;
  int tileCount;

  xml_document<> doc;
  xml_node<> * root_node;
  xml_node<> * element_node;

  SDL_Renderer* renderer;
  SDL_Surface* sourceImageSet;
  SDL_Texture* sourceTexSet;

  ifstream xmlTilesFile;
  vector<char> buffer;

  TileSet(string ipath, SDL_Renderer* renderer);
  ~TileSet();

  void renderTest();
  SDL_Rect getRectOfID(short ID);
  void renderTile(short ID, const Vector2D& pos, const Vector2D& size);
  void logProperties();
};