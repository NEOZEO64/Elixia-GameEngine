#include <SDL2/SDL.h> // general SDL graphics, sound, events library
#include <SDL2/SDL_image.h>

#define SWIDTH 320 //screen coord
#define SHEIGHT 620
#define FPS 60

typedef int8_t i8;  //mostly used for ingame coords
typedef int32_t i32; //mostly used for screen coords

SDL_Window* window;
SDL_Renderer* renderer;
SDL_Event event;

struct Color {i32 r; i32 g; i32 b; i32 a;};
const struct Color black = {0x00, 0x00, 0x00, 0xFF};
const struct Color white = {0xFF, 0xFF, 0xFF, 0xFF};
const struct Color red   = {0xFF, 0x00, 0x00, 0xFF};

bool runProgram = true;

void drawRect(i32 sx, i32 sy, i32 sw, i32 sh, struct Color color, bool fill){
    SDL_Rect rect = {sx,sy,sw,sh};
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    if (fill) { SDL_RenderFillRect(renderer, &rect); } 
    else      { SDL_RenderDrawRect(renderer, &rect); }
}

void drawBackground(struct Color color) {
    SDL_Rect rect = {0,0, SWIDTH, SHEIGHT};
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &rect);
};

int init() {
    srand(time(NULL)); // use the current time as the seed to generate random integers

    if (SDL_Init(SDL_INIT_EVENTS | SDL_INIT_TIMER | SDL_INIT_VIDEO) != 0) {
        printf("error initializing SDL: %s\n", SDL_GetError());
        return 0;
    }

    window = SDL_CreateWindow("Hello World!", SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,SWIDTH, SHEIGHT,0 );
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (window == NULL || renderer == 0) {return 0;}

    return 1;
}

void handleEvents() {
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) { runProgram = 0; }
        if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.scancode) {
            case SDL_SCANCODE_W:
            case SDL_SCANCODE_UP:
                //do something
                break;
            case SDL_SCANCODE_S:
            case SDL_SCANCODE_DOWN:
                //do something
                break;
            case SDL_SCANCODE_A:
            case SDL_SCANCODE_LEFT:
                //do something
                break;
            case SDL_SCANCODE_D:
            case SDL_SCANCODE_RIGHT:
                //do something
                break;
            case SDL_SCANCODE_ESCAPE:
                runProgram = 0;
                break;
            }
        } else if (event.type == SDL_KEYUP) { 
            switch (event.key.keysym.scancode) {
            case SDL_SCANCODE_W:
            case SDL_SCANCODE_UP:
                break;
            case SDL_SCANCODE_A:
            case SDL_SCANCODE_LEFT:
                break;
            case SDL_SCANCODE_S:
            case SDL_SCANCODE_DOWN:
                break;
            case SDL_SCANCODE_D:
            case SDL_SCANCODE_RIGHT:
                break;
            }
        }
    }
}

void render() {
    SDL_RenderClear(renderer); //make screen black
    drawBackground(black);

    //drawings here
    drawRect(10,10,50,50,red,true);


    SDL_RenderPresent(renderer); // triggers the double buffers for multiple rendering
}

void clean() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

}

int main() {
    if (!init()) {return -1;}

    while (runProgram) {
        handleEvents();       
        render();
        SDL_Delay(1000 / FPS);
    }
    clean();
    return 0;
}