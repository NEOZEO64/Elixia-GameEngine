#include "mySDL.hpp"

int mySDLinit(SDL_Window **window, SDL_Renderer **renderer) {
  if (SDL_Init(SDL_INIT_EVENTS | SDL_INIT_TIMER | SDL_INIT_VIDEO) != 0) {
    ERROR(fmt::format("error initializing SDL: {}", SDL_GetError()));
    return 0;
  }

  if (IMG_Init(IMG_INIT_PNG) == 0) {
    ERROR("error initializing SDL2_image");
    return 0;
  }

  *window =
      SDL_CreateWindow("Top Down Shooter", SDL_WINDOWPOS_CENTERED,
                       SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
  *renderer = SDL_CreateRenderer(
      *window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

  if (window == NULL || renderer == 0) {
    ERROR("Error loading window, renderer or image");
    return 0;
  }
  LOG("SDL initialized successfully");
  return 1;
}

void mySDLclean(SDL_Window **window, SDL_Renderer **renderer) {
  SDL_DestroyRenderer(*renderer);
  SDL_DestroyWindow(*window);
  SDL_Quit();
  LOG("SDL successfully cleaned");
}

void toggleFullscreen(SDL_Window *window) {
  if (window == NULL) {
    ERROR("Could not toggle fullscreen, because window pointer is NULL");
    return;
  }
  bool isFullscreen = SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN;
  SDL_SetWindowFullscreen(window, isFullscreen ? 0 : SDL_WINDOW_FULLSCREEN);
  SDL_ShowCursor(isFullscreen);
  LOG("Toggled fullscreen");
}

void drawRect(SDL_Renderer *renderer, const Vector2D &pos, const Vector2D &size,
              SDL_Color color, bool fill) {
  SDL_Rect rect = {(int)pos.x, (int)pos.y, (int)size.x, (int)size.y};
  SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

  if (fill) {
    SDL_RenderFillRect(renderer, &rect);
  } else {
    SDL_RenderDrawRect(renderer, &rect);
  }
}

void drawRect(SDL_Renderer *renderer, int sx, int sy, int sw, int sh,
              SDL_Color color, bool fill) {
  SDL_Rect rect = {sx, sy, sw, sh};
  SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

  if (fill) {
    SDL_RenderFillRect(renderer, &rect);
  } else {
    SDL_RenderDrawRect(renderer, &rect);
  }
}

void drawLine(SDL_Renderer *renderer, const Vector2D &pos1,
              const Vector2D &pos2, SDL_Color color) {
  SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
  SDL_RenderDrawLine(renderer, (int)pos1.x, (int)pos1.y, (int)pos2.x,
                     (int)pos2.y);
}

void drawBackground(SDL_Renderer *renderer, SDL_Color color) {
  SDL_Rect rect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
  SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
  SDL_RenderFillRect(renderer, &rect);
};