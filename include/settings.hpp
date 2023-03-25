#pragma once

// WINDOW SETTINGS
#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 800
#define FPS 60

// PATH SETTINGS
#define TILESET_PATH "./tilesets/"
#define MAPS_PATH "./maps/"
#define SCENES_PATH "./scenes/"

// SIMPLE LOG SETTINGS
#define DEBUG true
#define PRINT_TILES false // prints all tiles in terminal
#define COLORS true

// GAME SETTINGS
#define BOUNCE true
#define SPEED 0.1

// ADVANCES LOG SETTINGS – DO NOT CHANGE
#if COLORS
#define BLUE "\033[0;36;94m"
#define RED "\033[0;31;91m"
#define WHITE "\033[0;37m"
#else
#define BLUE ""
#define RED ""
#define WHITE ""
#endif

#include <iostream>
using std::cout, std::endl;

#define ERROR(x) std::cerr << RED << "ERROR: " << WHITE << x << std::endl

#if DEBUG == true
#define LOG(x) std::cerr << BLUE << "LOG: " << WHITE << x << std::endl
#define LOGn(x) std::cerr << x // no new line
#else
#define LOG(x)
#define LOGn(x)
#endif

// END