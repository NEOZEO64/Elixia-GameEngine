#include "entities.hpp"
#include "macros.hpp"
#include "mySDL.hpp"
#include "tileLib.hpp"
#include "vector2D.hpp"
#include <SDL2/SDL.h> // general SDL graphics, sound, events library
#include <SDL2/SDL_image.h>
#include <fstream>
#include <iostream>
#include <math.h>
#include <vector>

bool keysOn[4] = {false, false, false, false};

SDL_Window *window;
SDL_Renderer *renderer;
SDL_Event event;

SDL_Color black = {0x00, 0x00, 0x00, 0xFF};
SDL_Color white = {0xFF, 0xFF, 0xFF, 0xFF};
SDL_Color red = {0xFF, 0x00, 0x00, 0xFF};

size_t level = 0;

Vector2D tileRenderSize(40, 40);
Vector2D gameRenderSize(WINDOW_WIDTH - 20, WINDOW_HEIGHT - 20);
Vector2D gameRenderPos((WINDOW_WIDTH - gameRenderSize.x) / 2,
                       (WINDOW_HEIGHT - gameRenderSize.y) / 2);

bool visualDebugMode = false;
bool runProgram = true;

void handleEvents() {
  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_QUIT) {
      runProgram = 0;
    }
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
        toggleFullscreen(window);
        break;
      case SDL_SCANCODE_ESCAPE:
        runProgram = 0;
        break;
      case SDL_SCANCODE_KP_ENTER:
        level++;
        break;
      case SDL_SCANCODE_F1:
        visualDebugMode = !visualDebugMode;
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

void renderGame(TileSet *tileSet, Map *map, vector<Entity> &entities,
                Vector2D &tileRenderSize, const Vector2D &gameRenderPos,
                const Vector2D &gameRenderSize) {
  SDL_RenderClear(renderer); // make screen black
  drawBackground(renderer, black);
  // drawRect(10,10,50,50,red,true);

  // Render Map layer per layer
  for (size_t layer = 0; layer < map->layerCount; layer++) {
    map->render(tileSet, layer, tileRenderSize, gameRenderPos, gameRenderSize);
  }

  for (Entity &entity : entities) {
    entity.render(tileRenderSize, gameRenderPos);
    if (visualDebugMode) {
      entity.renderDebug(tileRenderSize, gameRenderPos);
    }
  }

  // tileSet->renderTest(); // To test
  SDL_RenderPresent(renderer);
  // triggers the double buffers for multiple rendering
}

int main() {

  // use the current time as the seed to generate random integers
  srand(time(NULL));

  if (!mySDLinit(&window, &renderer)) {
    return -1;
  }

  // TileSet tileSet(string(RESOURCE_PATH) + "KenneyLandscapeTiles.tsx",
  // renderer);
  TileSet tileSet(string(RESOURCE_PATH) + "Town Set.tsx", renderer);
  if (tileSet.getErrors() != "") {
    ERROR(tileSet.getErrors());
    return 1;
  }
  LOG(tileSet.getProperties());

  // Map map(string(LEVELS_PATH) + "0.tmx", tileSize);
  Map map(string(LEVELS_PATH) + "1.tmx");
  if (map.getErrors() != "") {
    ERROR(map.getErrors());
    return 1;
  }
  LOG(map.getProperties());

  Vector2D tempEntityPos = map.size / 2;
  Vector2D tempEntitySize = tileSet.tileSize / 2;
  Vector2D normViewAngle(0, 1);

  Entity tempEntity(&map, &tileSet, tempEntityPos, tempEntitySize,
                    normViewAngle, red, renderer);

  vector<Entity> entities = {tempEntity};

  LOG("Game loop is going to run");
  while (runProgram) {
    handleEvents();
    Vector2D tempAcceleration(
        SPEED * ((int)keysOn[3] - (int)keysOn[2]), // D & A
        SPEED * ((int)keysOn[1] - (int)keysOn[0])  // S & W
    );

    entities[0].vel += tempAcceleration;

    for (Entity &entity : entities) {
      entity.update();
    }

    renderGame(&tileSet, &map, entities, tileRenderSize, gameRenderPos,
               gameRenderSize);
    SDL_Delay(1000 / FPS);
  }
  mySDLclean(&window, &renderer);
  LOG("Quit");
  return 0;
}