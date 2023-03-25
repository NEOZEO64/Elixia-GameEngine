#include "entity.hpp"
#include "map.hpp"
#include "scene.hpp"
#include "settings.hpp"
#include "vector2D.hpp"
#include <SDL2/SDL.h> // general SDL graphics, sound, events library
#include <SDL2/SDL_image.h>
#include <fmt/core.h> //to format strings
#include <iostream>
#include <math.h>
#include <vector>

class Game {
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Event event;

  Game();
  ~Game();
}