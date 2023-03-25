#include "cam.hpp"
#include "entity.hpp"
#include "map.hpp"
#include "rapidxml.hpp"
#include "settings.hpp"
#include "vector2D.hpp"
#include <SDL2/SDL.h> // general SDL graphics, sound, events library
#include <SDL2/SDL_image.h>
#include <fmt/core.h> //to format strings
#include <fstream>
#include <iostream>
#include <math.h>
#include <vector>

class Scene {
private:
  string properties; // contains compact text about the current scene
  string errors;     // contains errors

public:
  vector<bool> keysOn; // Saves the information if the keys W,A,S,D are pressed

  Map *map;                   // The map object (contains tileset to render)
  vector<Entity *> *entities; // List of all entities
  Cam *cam;                   // The one cam object
  vector<uint8_t> backgroundColor; // Backgroundcolor, where nothing is rendered

  Scene(string sourceFile);
  // Create a scene: Reads the json-sourceFile (relative to main game directory)
  // This source file contains the information about the scene:
  //    Camera information
  //    Map information (The map includes the tileset)
  //    Entities information (including player)

  ~Scene(); // Destroy the scene with all its information on the heap

  string getProperties(); // get info about the scene (no entity info)
  string getErrors();     // get errors about the scene (empty, if no error)
  string getEntities();   // get info about the entities
  void addEntity(Entity *entity);
  void updateEntities();
  void handleEvents();
};