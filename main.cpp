#include <iostream>
#include <fstream>
#include <vector>
#include <SDL2/SDL.h> // general SDL graphics, sound, events library
#include <SDL2/SDL_image.h>
#include <math.h>
#include "vector2D.hpp"
#include "tileLib.hpp"
#include "macros.hpp"

using namespace rapidxml;
using namespace std;


int tileSize;

bool keysOn[4] = {false,false,false};

SDL_Window* window;
SDL_Renderer* renderer;
SDL_Event event;

struct Color {uint8_t r; uint8_t g; uint8_t b; uint8_t a;};
const struct Color black = {0x00, 0x00, 0x00, 0xFF};
//const struct Color white = {0xFF, 0xFF, 0xFF, 0xFF};
const struct Color red   = {0xFF, 0x00, 0x00, 0xFF};

bool runProgram = true;

void drawRect(const Vector2D& pos, const Vector2D& size, struct Color color, bool fill){
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
  SDL_Rect rect = {0,0, WINDOW_WIDTH, WINDOW_HEIGHT};
  SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
  SDL_RenderFillRect(renderer, &rect);
};


class Entity { 
public:
  Vector2D pos,vel, size, normViewAng, offsetToCenter;
  struct Color color;

  Entity(const Vector2D& pos, const Vector2D& size, const Vector2D& normViewAngle, struct Color color) : 
            pos(pos), size(size), color(color) {
    offsetToCenter = size * 0.5;
    vel.set(0,0);
    normViewAng = normViewAngle.norm();
    LOG("New entity appeared");
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
      if (BOUNCE){vel.x*=-0.4;}
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
      if (BOUNCE) {vel.y*=-0.4;}
      else {vel.y=0;pos.y=(map->tileHeight)*((int)pos.y)/(map->tileHeight);}
    }

    vel*=0.99f;
  }

  void render(const Vector2D& offset) {
    Vector2D tempPos = pos-offsetToCenter+offset;
    drawRect(tempPos,size,color,false);
  }
};

int mySDLInit() {
    srand(time(NULL)); // use the current time as the seed to generate random integers

    if (SDL_Init(SDL_INIT_EVENTS | SDL_INIT_TIMER | SDL_INIT_VIDEO) != 0) {
      ERROR(fmt::format("error initializing SDL: {}",SDL_GetError() ));
      return 0;
    }

    if (IMG_Init(IMG_INIT_PNG) == 0) {
      ERROR("error initializing SDL2_image");
	    return 0;
    }

    window = SDL_CreateWindow("Hello World!", SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,WINDOW_WIDTH, WINDOW_HEIGHT,0 );
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (window == NULL || renderer == 0) {
        ERROR("Error loading window, renderer or image");
        return 0;
    }
    LOG("SDL initialized successfully");
    return 1;
}

void toggleFullscreen() {
    bool isFullscreen = SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN;
    SDL_SetWindowFullscreen(window, isFullscreen ? 0 : SDL_WINDOW_FULLSCREEN);
    SDL_ShowCursor(isFullscreen);
    LOG("Toggled fullscreen");
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

void renderGame(TileSet* tileSet, Map* map, vector<Entity>& entities, const Vector2D& offset) {
  SDL_RenderClear(renderer); //make screen black
  drawBackground(black);
  //drawRect(10,10,50,50,red,true);

  //Render Ghostlayer
  map->render(tileSet,0,offset);

  //Render Collisionlayer
  map->render(tileSet,1,offset);

  for (Entity& entity: entities) {
    entity.render(offset);
  }

  //tileSet->render();
  SDL_RenderPresent(renderer); // triggers the double buffers for multiple rendering
} 

void clean() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    LOG("SDL successfully cleaned");
}

int main() {
  if (!mySDLInit()) {return -1;}

  TileSet tileSet(string(RESOURCE_PATH) + "KenneyLandscapeTiles.tsx",renderer);
  tileSet.logProperties();

  tileSize = WINDOW_WIDTH/tileSet.tileColumns;
  int tileSize2 = WINDOW_HEIGHT/tileSet.tileRows;

  if (tileSize > tileSize2) {tileSize = tileSize2;}

  Map map(string(LEVELS_PATH) + "PlaygroundMap.tmx", tileSize);
  map.logProperties();

  vector<Entity> entities;

  Vector2D tempEntityPos(map.mapWidth/2,map.mapHeight/2);
  Vector2D tempEntitySize(tileSet.tileWidth/2,tileSet.tileHeight/2);
  Vector2D normViewAngle(0,0);

  Entity tempEntity(tempEntityPos, tempEntitySize, normViewAngle, red);
  entities.push_back(tempEntity);

  Vector2D gameOffset(
    (WINDOW_WIDTH-map.mapWidth)/2,
    (WINDOW_HEIGHT-map.mapHeight)/2
  );
  LOG("Game loop is going to run");
  while (runProgram) {
    handleEvents();
    Vector2D tempAcceleration(
      SPEED*((int)keysOn[3] - (int)keysOn[2]), // D & A
      SPEED*((int)keysOn[1] - (int)keysOn[0])  // S & W
    );

    entities[0].vel += tempAcceleration;
    
    for (Entity& entity: entities) {
      entity.update(&map,&tileSet);
    }

    renderGame(&tileSet, &map, entities, gameOffset);
    SDL_Delay(1000 / FPS);
  }
  clean();
  LOG("Quit");
  return 0;
}