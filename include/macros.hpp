#pragma once

#define FPS 60
#define PI 3.14159265

#define BOUNCE true
#define SPEED 0.1

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 800

#define MAX_ENTITIES 200

#define RESOURCE_PATH "./res/"

#define LEVELS_PATH "./levels/"

#define DEBUG true
#define COLORS true

#if COLORS
#define BLUE "\033[0;36;94m"
#define RED "\033[0;31;91m"
#define WHITE "\033[0;37m"
#else
#define BLUE
#define RED
#define WHITE
#endif

#include <iostream>
using std::cout, std::endl;

#define ERROR(x) std::cout << RED << "ERROR: " << WHITE << x << std::endl

#if DEBUG == true
#define LOG(x) std::cout << BLUE << "LOG: " << WHITE << x << std::endl
#define LOGn(x) std::cout << x // no new line
#else
#define LOG(x)
#define LOGn(x)
#endif