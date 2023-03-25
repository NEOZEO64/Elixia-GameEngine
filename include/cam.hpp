#include "entity.hpp"
#include "map.hpp"
#include "settings.hpp"
#include "vector2D.hpp"
#include <SDL2/SDL.h> // general SDL graphics, sound, events library
#include <SDL2/SDL_image.h>
#include <fmt/core.h> //to format strings
#include <iostream>
#include <math.h>
#include <vector>

class Cam {
private:
  Map *map; // placeholder: could be NULL
  vector<Entity *> *entities;
  SDL_Renderer *renderer;
  SDL_Window *window;

  string properties;
  string errors;

  void drawRect(SDL_Renderer *renderer, const Vector2D &pos,
                const Vector2D &size, vector<uint8_t> color, bool fill);
  void drawLine(SDL_Renderer *renderer, const Vector2D &pos1,
                const Vector2D &pos2, vector<uint8_t> color);
  void drawRect(SDL_Renderer *renderer, int sx, int sy, int sw, int sh,
                vector<uint8_t> color, bool fill);
  void drawBackground(SDL_Renderer *renderer, vector<uint8_t> color);

public:
  Vector2D tileRenderSize;
  Vector2D pos, vel;
  bool visualDebugMode;

  vector<uint8_t> black, white, red, debugColor; // r,g,b,a

  Cam(SDL_Window *window, SDL_Renderer *renderer, TileSet *tileSet, Map *map,
      vector<Entity *> *entities, const Vector2D &tileRenderSize);

  void toggleFullscreen(SDL_Window *window);
  string getProperties();
  string getErrors();
  void render();
};