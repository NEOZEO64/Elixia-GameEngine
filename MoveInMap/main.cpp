#include <iostream>
#include <fstream>
#include <vector>
#include "rapidxml.hpp"
#include <SDL2/SDL.h> // general SDL graphics, sound, events library
#include <SDL2/SDL_image.h>
#include <math.h>

#define FPS 60

#define PI 3.14159265

using namespace rapidxml;
using namespace std;

bool bounciness = false;

const int SWIDTH = 1280; //screen coord
const int SHEIGHT = 800;

const int tileSize = 32;

const int maxEntityCount = 100;

struct Vector2D {
  float x, y;

  Vector2D() {}

  Vector2D (float ix, float iy) {
    x = ix;
    y = iy;
  }

  void set(float ix, float iy) {
    x = ix;
    y = iy;
  }

  void set(Vector2D newVector) {
    x = newVector.x;
    y = newVector.y;
  }

  void add(Vector2D vector) {
    x += vector.x;
    y += vector.y;
  }

  void add(float ix, float iy) {
    x += ix;
    y += iy;
  }

  void subt(Vector2D vector) {
    x -= vector.x;
    y -= vector.y;
  }

  Vector2D getAdded(Vector2D vector) {
    Vector2D tempSum(x+vector.x,y+vector.y);
    return tempSum;
  }

  Vector2D getAdded(float ix, float iy) {
    Vector2D tempSum(x+ix,y+iy);
    return tempSum;
  }

  Vector2D getSubt(Vector2D vector) {
    Vector2D tempSum(x-vector.x,y-vector.y);
    return tempSum;
  }

  void multWithScalar(float scalar) {
    x *= scalar;
    y *= scalar;
  }

  void multWithVector(Vector2D vector) {
    x *= vector.x;
    y *= vector.y;
  }

  void multWithVector(float ix, float iy) {
    x *= ix;
    y *= iy;
  }

  Vector2D getNorm() {
    Vector2D tempNormVector(x,y);
    tempNormVector.normalize();
    return tempNormVector;
  }

  void normalize() {
    multWithScalar(1.0f/getLen());
  }

  float getLen() {
    return sqrt(pow(x,2)+pow(y,2));
  }

  float getAngle() { //to an vector (1,0) otherwise a vector facing east
    return acos((x/getLen())/180*PI);
  }

  float getDist(Vector2D vector) {
    return sqrt(pow(x-vector.x,2)+pow(y-vector.y,2));
  }

  Vector2D getDiffVector(Vector2D vector) { //from this vector to vector in parameter
    Vector2D tempDiffVector(vector.x-x, vector.y-y);
    return tempDiffVector;
  }

  Vector2D getOppositeVector() {
    Vector2D tempOppositeVector(-x,-y);
    return tempOppositeVector;
  }


};

Vector2D gameOffset;

bool keysOn[4] = {false,false,false};

SDL_Window* window;
SDL_Renderer* renderer;
SDL_Event event;

struct Color {uint8_t r; uint8_t g; uint8_t b; uint8_t a;};
const struct Color black = {0x00, 0x00, 0x00, 0xFF};
const struct Color white = {0xFF, 0xFF, 0xFF, 0xFF};
const struct Color red   = {0xFF, 0x00, 0x00, 0xFF};

bool runProgram = true;

void drawRect(Vector2D pos, Vector2D size, struct Color color, bool fill){
  SDL_Rect rect = {(int)pos.x,(int)pos.y,(int)size.x,(int)size.y};
  SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

  if (fill) { SDL_RenderFillRect(renderer, &rect); } 
  else      { SDL_RenderDrawRect(renderer, &rect); }
}

void drawRect(int sx, int sy, int sw, int sh, struct Color color, bool fill){
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

  void renderTest() {
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

  void renderTile(short ID, Vector2D pos, Vector2D size) {
    SDL_Rect tempSourceRect = getRectOfID(ID);
    SDL_Rect tempDestRect = {(int)pos.x,(int)pos.y,(int)size.x,(int)size.y};
    SDL_RenderCopy(renderer, sourceTexSet, &tempSourceRect, &tempDestRect);
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
  int mapWidth, mapHeight;
  short* tiles; // = new int[m * n];

  Map(string ipath, int iTileWidth) {
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

    //tileWidth = atoi(mapNode->first_attribute("tilewidth")->value());
    //tileHeight = atoi(mapNode->first_attribute("tileheight")->value());

    tileWidth = iTileWidth;
    tileHeight = iTileWidth;

    width = atoi(mapNode->first_attribute("width")->value());
    height = atoi(mapNode->first_attribute("height")->value());
    layerCount = atoi(mapNode->first_attribute("nextlayerid")->value()) - 1;

    mapWidth = tileWidth*width;
    mapHeight = tileHeight*height;

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

  short getID(int tx, int ty, int layer) {
    return tiles[layer* width*height + ty * width + tx]; 
  }

  bool isColliding(TileSet* tileSet,Vector2D pos) {
    float tempTx = ((int)pos.x)/tileWidth; //without the rest!
    float tempTy = ((int)pos.y)/tileHeight;
    short tempID = getID(tempTx,tempTy,1);
    return (0<tempID && tempID <(tileSet->tileCount));

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
    cout << "Map Columns:" << width << " Rows:" << height << endl;
    cout << "Map PixelW:" << mapWidth << " H:" << mapHeight << endl;
    cout << "Number of layers: " << layerCount << endl;
    cout << "Tile W:" << tileWidth << " H:" << tileHeight << endl;
  }

  void render(TileSet* tileSet, int layer, Vector2D offset) {
    for (int ty = 0; ty<height; ty++) {
      for (int tx = 0; tx<width; tx++) {
        Vector2D tempPos(tx*tileWidth+offset.x, ty*tileWidth+offset.y);
        Vector2D tempSize(tileWidth,tileWidth);
        tileSet->renderTile(getID(tx,ty,layer),tempPos,tempSize);
      }
    }
  }
};

class Entity { 
public:
  Vector2D pos;
  Vector2D vel;
  Vector2D size;
  Vector2D normViewAngle;
  Vector2D offsetToCenter;
  struct Color color;
  bool initialized;

  Entity() {
    initialized = false;
  }

  void init(Vector2D iPos, Vector2D iSize, Vector2D iNormViewAngle, struct Color icolor) {
    pos = iPos;
    size = iSize;
    vel.set(0,0);
    normViewAngle = iNormViewAngle.getNorm();
    color = icolor;
    initialized = true;
    offsetToCenter.set(size.x*0.5, size.y*0.5);
  }

  void init(float ix, float iy, struct Color icolor) {
    pos.x = ix; pos.y = iy;
    size.x = tileSize*0.8; size.y = tileSize*0.8;
    normViewAngle.set(1,0);
    vel.set(0,0);
    color = icolor;
    initialized = true;
    offsetToCenter.set(size.x*0.5, size.y*0.5);
  }

  void update(Map* map, TileSet* tileSet) {
    if ( //check if 4 corners at the new position are inside any part of the map
      !map->isColliding(tileSet,pos.getAdded(vel.x,0).getAdded( offsetToCenter.x, offsetToCenter.y)) &&
      !map->isColliding(tileSet,pos.getAdded(vel.x,0).getAdded(-offsetToCenter.x, offsetToCenter.y)) &&
      !map->isColliding(tileSet,pos.getAdded(vel.x,0).getAdded( offsetToCenter.x,-offsetToCenter.y)) &&
      !map->isColliding(tileSet,pos.getAdded(vel.x,0).getAdded(-offsetToCenter.x,-offsetToCenter.y)) 
    ) {
      pos.x += vel.x;
    } else {
      if (bounciness){vel.x*=-0.9;}
      else {vel.x=0;pos.x=(map->tileWidth)*((int)pos.x)/(map->tileWidth);}
    }

    if ( //check if 4 corners at the new position are inside any part of the map
      !map->isColliding(tileSet,pos.getAdded(0,vel.y).getAdded( offsetToCenter.x, offsetToCenter.y)) &&
      !map->isColliding(tileSet,pos.getAdded(0,vel.y).getAdded(-offsetToCenter.x, offsetToCenter.y)) &&
      !map->isColliding(tileSet,pos.getAdded(0,vel.y).getAdded( offsetToCenter.x,-offsetToCenter.y)) &&
      !map->isColliding(tileSet,pos.getAdded(0,vel.y).getAdded(-offsetToCenter.x,-offsetToCenter.y))
    ) {  
      pos.y += vel.y;
    } else {
      if (bounciness) {vel.y*=-0.9;}
      else {vel.y=0;pos.y=(map->tileHeight)*((int)pos.y)/(map->tileHeight);}
    }

    vel.multWithScalar(0.99f);
  }

  void render(Vector2D offset) {
    Vector2D tempPos = pos.getSubt(offsetToCenter).getAdded(offset);
    drawRect(tempPos,size,color,false);
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

void toggleFullscreen() {
    bool isFullscreen = SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN;
    SDL_SetWindowFullscreen(window, isFullscreen ? 0 : SDL_WINDOW_FULLSCREEN);
    SDL_ShowCursor(isFullscreen);
}

void handleEvents() {
  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_QUIT) { runProgram = 0; }
    if (event.type == SDL_KEYDOWN) {
      switch (event.key.keysym.scancode) {
      case SDL_SCANCODE_W:
      case SDL_SCANCODE_UP:
        keysOn[0] = true;
        break;
      case SDL_SCANCODE_S:
      case SDL_SCANCODE_DOWN:
        keysOn[1] = true;
        break;
      case SDL_SCANCODE_A:
      case SDL_SCANCODE_LEFT:
        keysOn[2] = true;
        break;
      case SDL_SCANCODE_D:
      case SDL_SCANCODE_RIGHT:
        keysOn[3] = true;
        break;
      case SDL_SCANCODE_F:
        toggleFullscreen();
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
        keysOn[0] = false;
        break;
      case SDL_SCANCODE_S:
      case SDL_SCANCODE_DOWN:
        keysOn[1] = false;
        break;
      case SDL_SCANCODE_A:
      case SDL_SCANCODE_LEFT:
        keysOn[2] = false;
        break;
      case SDL_SCANCODE_D:
      case SDL_SCANCODE_RIGHT:
        keysOn[3] = false;
        break;
      default:
        break;
      }
    }
  }
}

void renderGame(TileSet* tileSet, Map* map, Entity* entities, Vector2D offset) {
  SDL_RenderClear(renderer); //make screen black
  drawBackground(black);
  //drawRect(10,10,50,50,red,true);

  //Render Ghostlayer
  map->render(tileSet,0,offset);

  //Render Collisionlayer
  map->render(tileSet,1,offset);

  for (int i = 0; i<maxEntityCount; i++) {
    if (entities[i].initialized) {entities[i].render(offset);}
  }

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
  Map map("./PlaygroundMap.tmx", tileSize);

  Entity entities[maxEntityCount];
  entities[0].init(map.mapWidth/2,map.mapHeight/2,red);

  gameOffset.set(
    (SWIDTH-map.mapWidth)/2,
    (SHEIGHT-map.mapHeight)/2
  );

  while (runProgram) {
    handleEvents();
    Vector2D tempAcceleration(
      0.2*((int)keysOn[3] - (int)keysOn[2]), // D & A
      0.2*((int)keysOn[1] - (int)keysOn[0])  // S & W
    );

    entities[0].vel.add(tempAcceleration);
    
    for (int i = 0; i<maxEntityCount; i++) {
      if (entities[i].initialized) {entities[i].update(&map,&tileSet);}
    }

    renderGame(&tileSet, &map, entities, gameOffset);
    SDL_Delay(1000 / FPS);
  }
  clean();
  return 0;
}