#include "cam.hpp"

Cam::Cam(SDL_Window *window, SDL_Renderer *renderer, TileSet *tileSet, Map *map,
         vector<Entity *> *entities, const Vector2D &tileRenderSize)
    : tileRenderSize(tileRenderSize), map(map), tileSet(tileSet),
      entities(entities), renderer(renderer) {
  pos = Vector2D(0, 0);
  // pos = (*entities)[0].pos;
  properties = "";
  errors = "";

  black = {0x00, 0x00, 0x00, 0xFF};
  white = {0xFF, 0xFF, 0xFF, 0xFF};
  red = {0xFF, 0x00, 0x00, 0xFF};
  debugColor = red;
  visualDebugMode = false;

  properties = fmt::format("Cam: Pointer: {}\n", static_cast<void *>(this));

  if (!window || !renderer || !tileSet || !map || !entities) {
    errors += "Any of the given pointer is NULL\n";
  }
}

string Cam::getProperties() { return properties; }
string Cam::getErrors() { return errors; }

void Cam::render() {
  SDL_RenderClear(renderer); // make screen black
  drawBackground(renderer, black);
  // drawRect(10,10,50,50,red,true);

  // Render Map layer per layer
  for (size_t layer = 0; layer < map->layerCount; layer++) {
    Vector2D tempTilePos(0, 0);
    for (tempTilePos.y = 0; tempTilePos.y < map->size.y; tempTilePos.y++) {
      for (tempTilePos.x = 0; tempTilePos.x < map->size.x; tempTilePos.x++) {
        tileSet->renderTile(map->getID(tempTilePos, layer),
                            (tempTilePos + pos) * tileRenderSize,
                            tileRenderSize);
      }
    }
  }
  for (Entity *entity : (*entities)) {
    drawRect(renderer, (pos * tileRenderSize + pos) / tileSet->tileSize,
             entity->size * tileRenderSize / tileSet->tileSize, entity->color,
             false);
    if (visualDebugMode) {
      Vector2D dotSize(3, 3);

      for (Vector2D &point : entity->hitBox) {
        drawRect(
            renderer,
            (entity->pos + point + pos) * tileRenderSize / tileSet->tileSize -
                dotSize / 2,
            dotSize * tileRenderSize / tileSet->tileSize, debugColor, false);
      }
      drawLine(renderer,
               (pos + entity->pos + entity->size / 2) * tileRenderSize /
                   tileSet->tileSize,
               (pos + entity->pos + entity->size / 2) * tileRenderSize /
                       tileSet->tileSize +
                   vel * 5,
               debugColor);
      // 5 is for length of the velocity line
    }
  }

  // tileSet->renderTest(); // To test
  SDL_RenderPresent(renderer);
  // triggers the double buffers for multiple rendering
}

void Cam::toggleFullscreen() {
  bool isFullscreen = SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN;
  SDL_SetWindowFullscreen(window, isFullscreen ? 0 : SDL_WINDOW_FULLSCREEN);
  SDL_ShowCursor(isFullscreen);
  LOG("Toggled fullscreen");
}

void Cam::drawRect(SDL_Renderer *renderer, const Vector2D &pos,
                   const Vector2D &size, vector<uint8_t> color, bool fill) {
  SDL_Rect rect = {(int)pos.x, (int)pos.y, (int)size.x, (int)size.y};
  SDL_SetRenderDrawColor(renderer, color[0], color[1], color[2], color[3]);

  if (fill) {
    SDL_RenderFillRect(renderer, &rect);
  } else {
    SDL_RenderDrawRect(renderer, &rect);
  }
}

void Cam::drawRect(SDL_Renderer *renderer, int sx, int sy, int sw, int sh,
                   vector<uint8_t> color, bool fill) {
  SDL_Rect rect = {sx, sy, sw, sh};
  SDL_SetRenderDrawColor(renderer, color[0], color[1], color[2], color[3]);

  if (fill) {
    SDL_RenderFillRect(renderer, &rect);
  } else {
    SDL_RenderDrawRect(renderer, &rect);
  }
}

void Cam::drawLine(SDL_Renderer *renderer, const Vector2D &pos1,
                   const Vector2D &pos2, vector<uint8_t> color) {
  SDL_SetRenderDrawColor(renderer, color[0], color[1], color[2], color[3]);
  SDL_RenderDrawLine(renderer, (int)pos1.x, (int)pos1.y, (int)pos2.x,
                     (int)pos2.y);
}

void Cam::drawBackground(SDL_Renderer *renderer, vector<uint8_t> color) {
  SDL_Rect rect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
  SDL_SetRenderDrawColor(renderer, color[0], color[1], color[2], color[3]);
  SDL_RenderFillRect(renderer, &rect);
};