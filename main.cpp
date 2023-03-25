#include "cam.hpp"
#include "entity.hpp"
#include "macros.hpp"
#include "map.hpp"
#include "scene.hpp"
#include "vector2D.hpp"
#include <SDL2/SDL.h> // general SDL graphics, sound, events library
#include <SDL2/SDL_image.h>
#include <fstream>
#include <iostream>
#include <math.h>
#include <vector>

size_t level = 0;

/*
Three types of coordinates:
1. Ingame coordinates – increment 1 per tile
2. Source coordinates – increment 1 per SourceTileWidth
3. Render coordinates – increment 1 per RenderTileWidth
*/

Vector2D tempTileRenderSize(40, 40);

int main() {
  // use the current time as the seed to generate random integers
  srand(time(NULL));

  Game game;

  if (game.getErrors() != "") {
    ERROR(game.getErrors());
    return 1;
  }

  string mapState = game.loadMap(string(LEVELS_PATH) + "1.tmx");
  if (mapState != "") {
    LOG(mapState);
  }

  Vector2D tempEntitySize = tileSet->tileSize / 2;
  Vector2D normViewAngle(0, 1);

  Vector2D tempEntityPos = map->size * tileSet->tileSize / 2;

  game.addEntity(
      new Entity(map, tempEntityPos, tempEntitySize, normViewAngle, cam->red));

  game.cam->pos = game.(*entities)[0]->pos;

  LOG(game.getEntities());

  LOG("Game loop is going to run");

  bool runProgram = true;
  while (runProgram) {
    game.handleEvents();

    Vector2D tempAcceleration(
        SPEED * ((int)keysOn[3] - (int)keysOn[2]), // D & A
        SPEED * ((int)keysOn[1] - (int)keysOn[0])  // S & W
    );

    game.(*entities)[0]->vel += tempAcceleration;

    game.updateEntities();

    game.cam->pos = (*entities)[0]->pos;

    game.cam->render();
    SDL_Delay(1000 / FPS);
  }

  LOG("Quit");

  return 0;
}