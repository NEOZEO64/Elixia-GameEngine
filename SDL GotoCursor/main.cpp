#include <SDL2/SDL.h> // general SDL graphics, sound, events library

#define SWIDTH 1280
#define SHEIGHT 800
#define FPS 60

typedef int8_t i8;
typedef int32_t i32;

SDL_Window* window;
SDL_Renderer* renderer;
SDL_Event event;

struct Color {i32 r; i32 g; i32 b; i32 a;};
const struct Color black = {0x00, 0x00, 0x00, 0xFF};
const struct Color white = {0xFF, 0xFF, 0xFF, 0xFF};
const struct Color red   = {0xFF, 0x00, 0x00, 0xFF};
const struct Color blue  = {0x00, 0x00, 0xFF, 0xFF};

bool runProgram = true;

float acceleration = 0.2;

bool keysOn[4] = {false,false,false,false}; //W, S, A, D

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

void toggleFullscreen() {
    bool isFullscreen = SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN;
    SDL_SetWindowFullscreen(window, isFullscreen ? 0 : SDL_WINDOW_FULLSCREEN);
    SDL_ShowCursor(isFullscreen);
}

class Entity {
public:
    float x, y;
    float vx, vy;
    i32 w, h;
    struct Color color;

    Entity(float ix, float iy, i32 iw, i32 ih, struct Color icolor) {
        x = ix;
        y = iy;
        w = iw;
        h = ih;
        vx = 0;
        vy = 0;
        color = icolor;
    }

    void update() {
        if (x+vx-w/2>=0 && x+vx+w/2 < SWIDTH) {x += vx;}
        else {vx *= -0.9;}
        
        if (y+vy-h/2>=0 && y+vy+h/2 < SHEIGHT) {y += vy;}
        else {vy *= -0.9;}

        vx *= 0.99f;
        vy *= 0.99f;
    }

    void render() {
        drawRect(x-w/2,y-w/2,w,h,color,true);
    }
};

Entity player(SWIDTH/2,SHEIGHT/2,20,20,red);

Entity mouse(SWIDTH/2,SHEIGHT/2,5,5,blue);

int mySDLinit() {
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
        if (event.type == SDL_MOUSEMOTION) {
            int tempX, tempY;
            SDL_GetMouseState(&tempX, &tempY);
            mouse.x = (float) tempX;
            mouse.y = (float) tempY;
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
                toggleFullscreen();
                break;
            case SDL_SCANCODE_ESCAPE:
                runProgram = 0;
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
            }
        }
    }
}

void render() {
    SDL_RenderClear(renderer); //make screen black
    drawBackground(black);

    //drawings here
    player.render();
    mouse.render();

    SDL_RenderPresent(renderer); // triggers the double buffers for multiple rendering
}

void clean() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main() {
    if (!mySDLinit()) {return -1;}

    while (runProgram) {
        handleEvents();  
        player.vx += (mouse.x-player.x)/200;
        player.vy += (mouse.y-player.y)/200;
        player.update();    
        render();
        SDL_Delay(1000 / FPS);
    }
    clean();
    return 0;
}