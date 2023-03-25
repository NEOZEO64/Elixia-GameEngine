#include "scene.hpp"

Scene::Scene() {
  if (SDL_Init(SDL_INIT_EVENTS | SDL_INIT_TIMER | SDL_INIT_VIDEO) != 0) {
    errors += fmt::format("error initializing SDL: {}\n", SDL_GetError());
  }

  if (IMG_Init(IMG_INIT_PNG) == 0) {
    errors += "error initializing SDL2_image\n";
  }

  window =
      SDL_CreateWindow("Top Down Shooter", SDL_WINDOWPOS_CENTERED,
                       SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
  renderer = SDL_CreateRenderer(
      window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

  if (window == NULL || renderer == 0) {
    errors += "Error loading window, renderer or image\n";
    return;
  }
  properties += "SDL initialized successfully\n";

  tileSet = new TileSet(string(RESOURCE_PATH) + "Town Set.tsx", renderer);
  if (tileSet->getErrors() != "") {
    errors += tileSet->getErrors();
    return;
  }
  properties += tileSet->getProperties();

  entities = new vector<Entity *>;

  cam = new Cam(renderer, tileSet, map, entities, tempTileRenderSize);

  keysOn = {false, false, false, false};

  properties += cam->getProperties();

  properties += "Pointers:";
  properties +=
      fmt::format("\tSDL Renderer: {}\n", static_cast<void *>(renderer));
  properties += fmt::format("\tTileset: {}\n", static_cast<void *>(tileSet));
  properties += fmt::format("\tMap: {}\n", static_cast<void *>(map));
  properties += fmt::format("\tCam: {}\n", static_cast<void *>(cam));
}

Scene::~Scene() {
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  properties += "SDL successfully cleaned\n";
  delete map;
  delete tileSet;
  for (Entity *entity : *entities) {
    delete entity;
  }
  delete entities;
  delete cam;
}

bool Scene::loadMap(string path) {

  // Map map(string(LEVELS_PATH) + "0.tmx", tileSize);
  map = new Map(path);
  if (map->getErrors() != "") {
    errors += map->getErrors();
    return false;
  }
  properties += map->getProperties();

#if PRINT_TILES
  properties += map->getTilesText();
#endif
  return true;
}

void Scene::addEntity(Entity *entity) {
  (*entities).push_back(entity);
  properties += (*entities)[(*entities).size() - 1]->getProperties();
}

void Scene::updateEntities() {
  for (Entity *entity : *entities) {
    entity->update();
  }
}

string Scene::getEntities() {
  string tempStr = "";
  for (Entity *entity : *entities) {
    tempStr += entity->getProperties();
  }
  return tempStr;
}

void Scene::handleEvents() {
  void handleEvents() {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        runProgram = 0;
      }
      if (event.type == SDL_MOUSEWHEEL) {
        cam->pos.x -= event.wheel.x * 6; // scroll x
        cam->pos.y += event.wheel.y * 6; // scroll y
      }
      if (event.type == SDL_KEYDOWN) {
        Vector2D tempFact(1.1, 1.1);
        switch (event.key.keysym.scancode) {
        case SDL_SCANCODE_W:
          keysOn[0] = true;
          break;
        case SDL_SCANCODE_S:
          keysOn[1] = true;
          break;
        case SDL_SCANCODE_A:
          keysOn[2] = true;
          break;
        case SDL_SCANCODE_D:
          keysOn[3] = true;
          break;
        case SDL_SCANCODE_F:
          toggleFullscreen(window);
          break;
        case SDL_SCANCODE_UP:
          cam->tileRenderSize *= tempFact;
          cam->tileRenderSize.makeInt();
          break;
        case SDL_SCANCODE_DOWN:
          cam->tileRenderSize /= tempFact;
          cam->tileRenderSize.makeInt();
          break;
        case SDL_SCANCODE_ESCAPE:
          runProgram = 0;
          break;
        case SDL_SCANCODE_KP_ENTER:
          level++;
          break;
        case SDL_SCANCODE_F1:
          cam->visualDebugMode = !cam->visualDebugMode;
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
}

string Scene::getProperties() { return properties; }
string Scene::getErrors() { return errors; }