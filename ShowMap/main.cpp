#include <iostream>
#include <fstream>
#include <vector>
#include "rapidxml.hpp"
#include <SDL2/SDL.h> // general SDL graphics, sound, events library
#include <SDL2/SDL_image.h>

using namespace rapidxml;
using namespace std;

#define SWIDTH 1280 //screen coord
#define SHEIGHT 720
#define FPS 60

const int screenTileWidth = 32; //and tileHeight

typedef int8_t i8;  //mostly used for ingame coords
typedef int32_t i32; //mostly used for screen coords

SDL_Window* window;
SDL_Renderer* renderer;
SDL_Event event;

struct Color {i32 r; i32 g; i32 b; i32 a;};
const struct Color black = {0x00, 0x00, 0x00, 0xFF};
const struct Color white = {0xFF, 0xFF, 0xFF, 0xFF};
const struct Color red   = {0xFF, 0x00, 0x00, 0xFF};

bool runProgram = true;

void drawRect(i32 sx, i32 sy, i32 sw, i32 sh, struct Color color, bool fill){
  SDL_Rect rect = {sx,sy,sw,sh};
  SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

  if (fill) { SDL_RenderFillRect(renderer, &rect); } 
  else      { SDL_RenderDrawRect(renderer, &rect); }
}

void drawBackground(struct Color color) {
  SDL_Rect rect = {0,0, SWIDTH, SHEIGHT};
  SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
  SDL_RenderFillRect(renderer, &rect);
};

class TileSet {
private:
  xml_document<> doc;
  xml_node<> * root_node;
  xml_node<> * element_node;

public:
  string path;
  string imagePath;
  int tileWidth, tileHeight;
  int imageWidth, imageHeight;
  int tileColumns;
  int tileRows;
  int tileCount;

  SDL_Surface* sourceImageSet;
  SDL_Texture* sourceTexSet;

  TileSet(string ipath) {
    path = ipath;
    cout << "Parsing " << path << " to TileSet Class" << endl;

    // Read the xml file into a vector
    ifstream xmlTilesFile(path);
    if (xmlTilesFile.fail()) {
      cout << "File " << path << "doesn't exist. This will cause errors" << endl;
    }

    vector<char> buffer((istreambuf_iterator<char>(xmlTilesFile)), istreambuf_iterator<char>());
    buffer.push_back('\0');
    doc.parse<0>(&buffer[0]); // Parse the buffer using the xml file parsing library into doc 
    
    root_node = doc.first_node("tileset"); // Find our root node
    element_node = root_node->first_node("image");

    tileWidth = atoi(root_node->first_attribute("tilewidth")->value());
    tileHeight = atoi(root_node->first_attribute("tileheight")->value());
    tileCount = atoi(root_node->first_attribute("tilecount")->value());
    tileColumns = atoi(root_node->first_attribute("columns")->value());
    tileRows = tileCount/tileColumns;
    imagePath = element_node->first_attribute("source")->value();
    imagePath.insert(0,"./");

    imageWidth = atoi(element_node->first_attribute("width")->value());
    imageHeight = atoi(element_node->first_attribute("height")->value());

    const char *c = imagePath.c_str();
    sourceImageSet = IMG_Load(c);
    sourceTexSet = SDL_CreateTextureFromSurface(renderer, sourceImageSet);
    SDL_FreeSurface(sourceImageSet);
  }

  ~TileSet() {
    SDL_DestroyTexture(sourceTexSet);
  }

  void render() {
    SDL_Rect tempSourceRect = {0,0,imageWidth,imageHeight};
    SDL_Rect tempDestRect = {0,0,SWIDTH,SHEIGHT};
    SDL_RenderCopy(renderer, sourceTexSet, &tempSourceRect, &tempDestRect);
  }

  SDL_Rect getRectOfID(short ID) {
    SDL_Rect tempRect = {0,0,tileWidth,tileHeight};

    tempRect.x = ID%tileColumns-1;
    tempRect.y = ((ID-tempRect.x)/tileColumns);
    tempRect.x *= tileWidth;
    tempRect.y *= tileHeight;
    return tempRect;
  }

  void printProperties() {
    cout << "Tileset " << path << ":" << endl;
    cout << "Imagepath:" << imagePath << endl;
    cout << "Image W:" << imageWidth << " H:" << imageHeight << endl;
    cout << "Tile W:" << tileWidth << endl;
    cout << "Tile H:" << tileHeight << endl;
    cout << "Tile Columns:" << tileColumns << endl;
    cout << "Tile Count:" << tileCount << endl;
  }
};

class Map {
private:
  xml_document<> doc;
  xml_node<> * mapNode; //root node
  xml_node<> * tileSetNode; //element node
  xml_node<> * tempLayerNode; //element node
  xml_node<> * tempDataNode; //sub-element node

public:
  string path;
  string tileSetPath;

  int width, height;
  int layerCount;
  int tileWidth, tileHeight;
  short* tiles; // = new int[m * n];

  Map(string ipath) {
    path = ipath;
    cout << "Parsing " << path << " to Map" << endl;

    // Read the xml file into a vector
    ifstream xmlTilesFile(path);
    if (xmlTilesFile.fail()) {
      cout << "File " << path << "doesn't exist. This will cause errors" << endl;
    }
    vector<char> buffer((istreambuf_iterator<char>(xmlTilesFile)), istreambuf_iterator<char>());
    buffer.push_back('\0');
    doc.parse<0>(&buffer[0]); // Parse the buffer using the xml file parsing library into doc 
    
    mapNode = doc.first_node("map"); // Find our root node
    tileSetNode = mapNode->first_node("tileset");

    tileSetPath = tileSetNode->first_attribute("source")->value();

    tileWidth = atoi(mapNode->first_attribute("tilewidth")->value());
    tileHeight = atoi(mapNode->first_attribute("tileheight")->value());

    width = atoi(mapNode->first_attribute("width")->value());
    height = atoi(mapNode->first_attribute("height")->value());
    layerCount = atoi(mapNode->first_attribute("nextlayerid")->value()) - 1;

    tiles = new short[layerCount * height * width];

    //iterate through layers
    tempLayerNode = mapNode->first_node("layer"); 
    string dataDelimiter = ",";

    for (int layerID = 1; layerID <= layerCount; layerID ++) {
      tempDataNode = tempLayerNode->first_node("data");
      string layerData = tempDataNode->value();
      int pos;

      // remove all \n & ' ' from layerData
      while ((pos = layerData.find('\n'))!=string::npos) {
        layerData.replace(pos, 1, "");
      }
      while ((pos = layerData.find(' '))!=string::npos) {
        layerData.replace(pos, 1, "");
      }
      //needed to parse the last tileID
      layerData.append(",");
      
      //Parse tiles
      int layer = layerID-1;
      int index = 0;
      while ((pos = layerData.find(dataDelimiter)) != string::npos) {
        tiles[layer* width*height + index] = stoi(layerData.substr(0, pos));
        index ++;
        layerData.erase(0, pos + dataDelimiter.length());
      }
      //go to next layer
      tempLayerNode = tempLayerNode->next_sibling();
    }


  }

  short getID(TileSet* tileSet, int x, int y, int layer) {
    return tiles[layer* width*height + y * width + x]; 
    //tiles[layer* (*tileSet).tileCount + y * (*tileSet).tileColumns + x]
  }

  void printTiles() {
    for (int layer = 0; layer<layerCount; layer++) {
      cout << "Layer " << layer+1 << ":" << endl;
      for (int y = 0; y<height; y++) {
        for (int x = 0; x<width; x++) {
          cout << tiles[layer* width*height + y * width + x] << ",";
        }
        cout << endl;
      }
    }
  }

  void printProperties() {
    cout << "Map " << path << ":" << endl;
    cout << "Tileset Path:" << tileSetPath << endl;
    cout << "Map W:" << width << " H:" << height << endl;
    cout << "Number of layers: " << layerCount << endl;
    cout << "Tile W:" << tileWidth << " H:" << tileHeight << endl;
  }

  void render(TileSet* tileSet, SDL_Texture* texture, int layer) {
    for (int y = 0; y<height; y++) {
      for (int x = 0; x<width; x++) {
        short currentBlockID = getID(tileSet,x,y,layer);
        SDL_Rect tempSourceRect = (*tileSet).getRectOfID(currentBlockID);
        SDL_Rect tempDestRect = {x*screenTileWidth, y*screenTileWidth, screenTileWidth, screenTileWidth};
        SDL_RenderCopy(renderer, texture, &tempSourceRect, &tempDestRect);
      }
    }
  }
};

int mySDLInit() {
    srand(time(NULL)); // use the current time as the seed to generate random integers

    if (SDL_Init(SDL_INIT_EVENTS | SDL_INIT_TIMER | SDL_INIT_VIDEO) != 0) {
        cout << "error initializing SDL:" << SDL_GetError() << endl;
        return 0;
    }

    if (IMG_Init(IMG_INIT_PNG) == 0) {
        cout << "error initializing SDL2_image" << endl;
	    return 0;
    }

    window = SDL_CreateWindow("Hello World!", SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,SWIDTH, SHEIGHT,0 );
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (window == NULL || renderer == 0) {
        cout << "Error loading window, renderer or image" << endl;
        return 0;
    }

    return 1;
}

void handleEvents() {
  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_QUIT) { runProgram = 0; }
    if (event.type == SDL_KEYDOWN) {
      switch (event.key.keysym.scancode) {
      case SDL_SCANCODE_W:
      case SDL_SCANCODE_UP:
        //do something
        break;
      case SDL_SCANCODE_S:
      case SDL_SCANCODE_DOWN:
        //do something
        break;
      case SDL_SCANCODE_A:
      case SDL_SCANCODE_LEFT:
        //do something
        break;
      case SDL_SCANCODE_D:
      case SDL_SCANCODE_RIGHT:
        //do something
        break;
      case SDL_SCANCODE_ESCAPE:
        runProgram = 0;
        break;
      default:
        break;
      }
  } else if (event.type == SDL_KEYUP) { 
      switch (event.key.keysym.scancode) {
      case SDL_SCANCODE_W:
      case SDL_SCANCODE_UP:
        break;
      case SDL_SCANCODE_A:
      case SDL_SCANCODE_LEFT:
        break;
      case SDL_SCANCODE_S:
      case SDL_SCANCODE_DOWN:
        break;
      case SDL_SCANCODE_D:
      case SDL_SCANCODE_RIGHT:
        break;
      default:
        break;
      }
    }
  }
}

void render(Map* map, TileSet* tileSet) {
  SDL_RenderClear(renderer); //make screen black
  drawBackground(black);
  //drawRect(10,10,50,50,red,true);
  map->render(tileSet,tileSet->sourceTexSet,0);
  //tileSet->render();
  SDL_RenderPresent(renderer); // triggers the double buffers for multiple rendering

} 

void clean() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main(void) {
  if (!mySDLInit()) {return -1;}

  TileSet tileSet("./KenneyLandscapeTiles.tsx");
  Map playgroundMap("./PlaygroundMap.tmx");

  playgroundMap.printProperties();
  tileSet.printProperties();

  while (runProgram) {
    handleEvents();
    render(&playgroundMap,&tileSet);

    SDL_Delay(1000 / FPS);
  }
  clean();
  return 0;
}