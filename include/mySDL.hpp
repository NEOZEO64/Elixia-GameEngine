#pragma once

#include "macros.hpp"
#include "tileLib.hpp"
#include "vector2D.hpp"
#include <SDL2/SDL.h> // general SDL graphics, sound, events library
#include <SDL2/SDL_image.h>
#include <math.h>

int mySDLinit(SDL_Window **window, SDL_Renderer **renderer);

void mySDLclean(SDL_Window **window, SDL_Renderer **renderer);

void toggleFullscreen(SDL_Window *window);

void drawRect(SDL_Renderer *renderer, const Vector2D &pos, const Vector2D &size,
              SDL_Color color, bool fill);

void drawLine(SDL_Renderer *renderer, const Vector2D &pos1,
              const Vector2D &pos2, SDL_Color color);

void drawRect(SDL_Renderer *renderer, int sx, int sy, int sw, int sh,
              SDL_Color color, bool fill);

void drawBackground(SDL_Renderer *renderer, SDL_Color color);