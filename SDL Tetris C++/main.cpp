/*/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SIMPLE TETRIS GAME IN C (with SDL)

This is the full Tetris code, written in January 2022 as a university project by 
  * Chanhq (https://github.com/Chanhq)
  * NaCl-sys (https://github.com/NaCl-sys)
  * NEOZEO64 (https://github.com/NEOZEO64)

To compile it, install the SDL-libraries and run the Makefile.

Important to understand the code:
  * Coordinates with an "s" as the first character mean window-coordinates (such as "sx" or "sy"
    Variables of coords without anything in front of the name store ingame coords!
  * Variables with a "t" as the first character belong to the info about tetrinos (the Tetris-parts which fall down)
*//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> //to get a unique seed for random integers

#include <SDL2/SDL.h> // general SDL graphics, sound, events library
#include <SDL2/SDL_timer.h> // for SDL_Delay()
#include <SDL2/SDL_mouse.h> // to work with the mouse (used to eventually disable it)
#include <SDL2/SDL_ttf.h> //to show text

#define SWIDTH 320 //screen coord
#define SHEIGHT 620
#define SOFFSETSX 10 //the offset where to show the tetris field in the window
#define SOFFSETSY 10
#define WIDTH 10 //ingame coord; has to be a dividor of SWIDTH
#define HEIGHT 20 //has to be a dividor of SHEIGHT

#define SBLOCKWIDTH 30 //has to be a dividor of WIDTH and HEIGHT
#define BORDER_THICKNESS 4

#define DISABLE_CURSOR 1 //If !0, the Cursor is disabled, else the Cursor is enabled
#define START_FASTER_FALLSPEED 150
#define FALLSPEED_FACTOR 4 //8 means, that the faster fall speed is 8 times faster than the normal fall speed

#define FPS 30

typedef int8_t i8;  //mostly used for ingame coords
typedef int32_t i32; //mostly used for screen coords


i8 instanceRunning = 0;
// if instance == 0: Menu is running
// if instance == 1: Game is running
// if instance == 2: View credits

i8 selected = 0;
const i8 buttonCount = 3;
char buttons[buttonCount][20] = {
    {"    Start"},
    {"    Credits"},
    {"    Quit"}
};

char selectedButtons[buttonCount][20] = {
    {"> Start"},
    {"> Credits"},
    {"> Quit"}
};

const i8 numberCreditLines = 6;
char credits[numberCreditLines][60] = {
    {"Created by           "},
    {"    Carl (NEOZEO)    "},
    {"    Dawid (NaCl)     "},
    {"    Chan (Hoeme)     "},
    {"Special Thanks to    "},
    {"    Maximilian Vincen"}
};

//Info about the score
i32 score = 0; //the variable which stores the current score
const i32 scoreAddLineDone = 50; //how big the score should get when a line is complete
const i32 scoreAddTetrinoFallDown = 1; 
char scoreString[20] = {"Score: "}; //later in the code, the current score gets attached behind this string

// Info about the current, falling tetrino
const i8 tSpawnPosX = WIDTH/2-1;
const i8 tSpawnPosY = -2;
i8 tForm; //type of tetrino (0-6), defined as the index in the TETRINOS-list
i8 tRot;  //rotation of tetrino (0-3), defined as the second index in the Tetrino list
i8 tColor;  //rotation of tetrino (0-3), defined as the second index in the Tetrino list
i8 tPosX, tPosY;

// SDL structs
SDL_TimerID fastFallTimer;
SDL_Window* window;
SDL_Renderer* renderer;
TTF_Font* font;
TTF_Font* bigFont;
TTF_Font* titleFont;

//Info about the speed of the game
i32 fasterFallSpeed = START_FASTER_FALLSPEED;
i8 fallFaster = 0; //the temporary variable to save whether to fall down fast or not
i8 fallTimer = 0; //the temporary variable to count down from fallSpeedFactor down to 0 and then fall down 

//Info about the tetrinos and the field
i8 field[WIDTH*HEIGHT] = {0}; //All blocks are 0 (air) at the beginning

i8 menuField[WIDTH*HEIGHT] = {
    0,0,0,0,0,0,0,0,0,0,
    0,0,2,0,0,0,0,0,0,0,
    0,0,2,2,0,0,3,0,0,0,
    0,0,0,2,0,3,3,3,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,1,1,1,1,0,4,0,0,
    0,0,0,0,0,0,0,4,0,0,
    0,0,5,5,0,0,4,4,0,0,
    0,0,5,5,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
}; //All blocks are 0 (air) at the beginning

const i8 tFormWidth[7] = {4,2,3,3,3,3,3};


const i8 tForms[7][4][16] = {
    { //0. tetrino: I
        {0, 0, 0, 0,
         0, 0, 0, 0,
         1, 1, 1, 1,
         0, 0, 0, 0},

        {0, 0, 1, 0,
         0, 0, 1, 0,
         0, 0, 1, 0,
         0, 0, 1, 0},
        
        {0, 0, 0, 0,
         0, 0, 0, 0,
         1, 1, 1, 1,
         0, 0, 0, 0},

        {0, 0, 1, 0,
         0, 0, 1, 0,
         0, 0, 1, 0,
         0, 0, 1, 0}
    },
    { //1. tetrino: O
        {1, 1,
         1, 1,},

        {1, 1,
         1, 1,},

        {1, 1,
         1, 1,},
        
        {1, 1,
         1, 1,},
    },
    { //2. tetrino: T
        {0, 0, 0,
         1, 1, 1,
         0, 1, 0},
    
        {0, 1, 0,
         1, 1, 0,
         0, 1, 0},
        
        {0, 1, 0,
         1, 1, 1,
         0, 0, 0},
    
        {0, 1, 0,
         0, 1, 1,
         0, 1, 0}
    },
    { //3. tetrion: S
        {0, 0, 0,
         0, 1, 1,
         1, 1, 0},

        {0, 1, 0,
         0, 1, 1,
         0, 0, 1},

        {0, 0, 0,
         0, 1, 1,
         1, 1, 0},

        {0, 1, 0,
         0, 1, 1,
         0, 0, 1}
    },
    { //4. tetrino: Z
        {0, 0, 0,
         1, 1, 0,
         0, 1, 1},

        {0, 0, 1,
         0, 1, 1,
         0, 1, 0},

        {0, 0, 0,
         1, 1, 0,
         0, 1, 1},

        {0, 0, 1,
         0, 1, 1,
         0, 1, 0}
    },
    { //5. tetrino: J
        {0, 0, 0,
         1, 1, 1,
         0, 0, 1},

        {0, 1, 0,
         0, 1, 0,
         1, 1, 0},

        {1, 0, 0,
         1, 1, 1,
         0, 0, 0},

        {0, 1, 1,
         0, 1, 0,
         0, 1, 0},
    },
    { //6. tetrino: L
        {0, 0, 0,
         1, 1, 1,
         1, 0, 0},

        {1, 1, 0,
         0, 1, 0,
         0, 1, 0},

        {0, 0, 1,
         1, 1, 1,
         0, 0, 0},

        {0, 1, 0,
         0, 1, 0,
         0, 1, 1}
    }
};

struct Color {i32 r; i32 g; i32 b; i32 a;};
const struct Color black = {0x00, 0x00, 0x00, 0xFF};
const struct Color white = {0xFF, 0xFF, 0xFF, 0xFF};

const struct Color blockColorsBase[] = { //r,g,b,a in hex
    { 0x30, 0x30, 0x30, 0xFF }, //{ 0x28, 0x28, 0x28, 0xFF }, //background color
    { 0x2D, 0x99, 0x99, 0xFF },
    { 0x99, 0x99, 0x2D, 0xFF },
    { 0x99, 0x2D, 0x99, 0xFF },
    { 0x2D, 0x99, 0x51, 0xFF },
    { 0x99, 0x2D, 0x2D, 0xFF },
    { 0x2D, 0x63, 0x99, 0xFF },
    { 0x99, 0x63, 0x2D, 0xFF },
    { 0x60, 0x20, 0x20, 0xFF } //red color to show that a collision quits the game
};
const struct Color blockColorsLight[] = { //r,g,b,a in hex
    { 0x25, 0x25, 0x25, 0xFF }, //{ 0x28, 0x28, 0x28, 0xFF }, //background color
    { 0x44, 0xE5, 0xE5, 0xFF },
    { 0xE5, 0xE5, 0x44, 0xFF },
    { 0xE5, 0x44, 0xE5, 0xFF },
    { 0x44, 0xE5, 0x7A, 0xFF },
    { 0xE5, 0x44, 0x44, 0xFF },
    { 0x44, 0x95, 0xE5, 0xFF },
    { 0xE5, 0x95, 0x44, 0xFF },
    { 0x60, 0x20, 0x20, 0xFF } //red color to show that a collision quits the game
};
const struct Color blockColorsDark[] = { //r,g,b,a in hex
    { 0x35, 0x35, 0x35, 0xFF }, //{ 0x28, 0x28, 0x28, 0xFF }, //background color
    { 0x1E, 0x66, 0x66, 0xFF },
    { 0x66, 0x66, 0x1E, 0xFF },
    { 0x66, 0x1E, 0x66, 0xFF },
    { 0x1E, 0x66, 0x36, 0xFF },
    { 0x66, 0x1E, 0x1E, 0xFF },
    { 0x1E, 0x42, 0x66, 0xFF },
    { 0x66, 0x42, 0x1E, 0xFF },
    { 0x60, 0x20, 0x20, 0xFF } //red color to show that a collision quits the game
};


//A lot of functions
i8 getRandom(i8 min, i8 max) { // min and max are in the interval
    return rand()%(max-min + 1)+min;
}

void drawRect(i32 sx, i32 sy, i32 sw, i32 sh, struct Color color, i8 fill){
    SDL_Rect rect = {0};
    rect.x = sx;
    rect.y = sy;
    rect.w = sw;
    rect.h = sh;
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    if (fill) {
        SDL_RenderFillRect(renderer, &rect);
    } else {
        SDL_RenderDrawRect(renderer, &rect);
    }
}

void drawBackground(struct Color color) {
    SDL_Rect rect = {0,0, SWIDTH, SHEIGHT};
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &rect);
};

void drawText(const char *text,i32 sx, i32 sy,i8 alignment,struct Color color, TTF_Font* font){
    SDL_Color sdl_color = {color.r, color.g, color.b, color.a };
    SDL_Surface *surface = TTF_RenderText_Solid(font, text, sdl_color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect rect;

    switch (alignment) {
    case -1: //Align left
        rect.x = sx;
        break;
    case 0: //Align center
        rect.x = sx - surface->w / 2;
        break;
    case 1: //Align right
        rect.x = sx - surface->w;
        break;
    }
    rect.y = sy;
    rect.w = surface->w;
    rect.h = surface->h;

    SDL_RenderCopy(renderer, texture, 0, &rect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void drawBlock(i8 x, i8 y, i8 color){
    if (x>=0 && x<WIDTH && y<HEIGHT) {
        i32 sx = x * SBLOCKWIDTH + SOFFSETSX;
        i32 sy = y * SBLOCKWIDTH + SOFFSETSY;

        drawRect(sx, sy, SBLOCKWIDTH, SBLOCKWIDTH, blockColorsDark[color],1);
        drawRect(sx + BORDER_THICKNESS, sy, SBLOCKWIDTH - BORDER_THICKNESS, SBLOCKWIDTH - BORDER_THICKNESS, blockColorsLight[color],1);
        drawRect(sx + BORDER_THICKNESS, sy + BORDER_THICKNESS, SBLOCKWIDTH - BORDER_THICKNESS * 2, SBLOCKWIDTH - BORDER_THICKNESS * 2, blockColorsBase[color],1);
    }
}

Uint32 generateUserEvent(Uint32 interval, void *param) {
    SDL_Event event;
    SDL_UserEvent userevent;

    userevent.type = SDL_USEREVENT;
    userevent.code = 0;
    userevent.data1 = NULL;
    userevent.data2 = NULL;

    event.type = SDL_USEREVENT;
    event.user = userevent;

    SDL_PushEvent(&event);
    return(interval);
}

i8 getBlock(i8 x, i8 y) {
    if (x < 0 || x >= WIDTH || y >= HEIGHT) {return -1;}
    else if (y<0) {return 0;}
    else {return field[y*WIDTH + x];}
}

void newTetrino() {
    tColor = getRandom(1,7);
    tRot = 0; //getRandom(0,3);
    tForm = getRandom(0,6);
    tPosY = tSpawnPosY;

    if (tForm == 0) {tPosX = tSpawnPosX-2;}
    else if (tForm == 1) {tPosX = tSpawnPosX+1;}
    else {tPosX = tSpawnPosX;}
    
}

void drawTetrino() {
    for (i8 y = 0; y<tFormWidth[tForm]; y++) {
        for (i8 x = 0; x<tFormWidth[tForm]; x++) {
            if (tForms[tForm][tRot][y * tFormWidth[tForm] + x] != 0) {
                drawBlock(tPosX+x,tPosY+y,tForms[tForm][tRot][y * tFormWidth[tForm] + x] * tColor);
            }
        }
    }
}

void gameOverAnimation() {
    time_t old = time(NULL);
    time_t now = time(NULL);
    i8 run = 1;
    while (difftime(now,old) < 2 && run) {
        now = time(NULL);
        SDL_Event event;
        while (SDL_PollEvent(&event)) { // Go through every event which occured
            if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {run = 0;}
        }

        SDL_RenderClear(renderer); //make screen black
        drawBackground(black); //(0 is black, 1 is white, 2 is grey)
        for (int y = 0; y<HEIGHT; y++) {
            for (int x = 0; x<WIDTH; x++) {
                drawBlock(x,y,getRandom(1,7)); //field blocks
            }
        }
        SDL_RenderPresent(renderer); // triggers the double buffers for multiple rendering
        SDL_Delay(1000 / FPS);
    }
}

i8 collide(i8 tXTemp, i8 tYTemp, i8 tFormTemp, i8 tRotTemp) { //returns 1 if tetrino collides, 0 if it doesn't collide
    for (i8 y = 0; y<tFormWidth[tFormTemp]; y++) {
        for (i8 x = 0; x<tFormWidth[tFormTemp];x++) {
            if (tForms[tFormTemp][tRotTemp][y * tFormWidth[tFormTemp] + x] == 1 && getBlock(tXTemp+x,tYTemp+y)!=0) {
                return 1;
            }
        }
    }
    return 0;
}

void newGame() {
    newTetrino();
    for (int y = 0; y<HEIGHT; y++) {
        for (int x = 0; x<WIDTH; x++) {
            field[y*WIDTH+x] = 0;
        }
    }
    score = 0;
    fasterFallSpeed = START_FASTER_FALLSPEED;
    SDL_RemoveTimer(fastFallTimer);
    fastFallTimer = SDL_AddTimer(fasterFallSpeed, generateUserEvent, NULL);
}

int main(int argc, char *argv[]) {
    srand(time(NULL)); // use the current time as the seed to generate random integers

    if (SDL_Init(SDL_INIT_EVENTS | SDL_INIT_TIMER | SDL_INIT_VIDEO) != 0) {
        printf("error initializing SDL: %s\n", SDL_GetError());
    }
    TTF_Init();

    window = SDL_CreateWindow("Tetris", SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,SWIDTH, SHEIGHT,0 );
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    fastFallTimer = SDL_AddTimer(fasterFallSpeed, generateUserEvent, NULL);
    
    font = TTF_OpenFont("./font/PixelFont.ttf", 14);
    bigFont = TTF_OpenFont("./font/PixelFont.ttf", 19);
    titleFont = TTF_OpenFont("./font/PixelFont.ttf", 40);

    if (font == NULL){
		printf("'PixelFont.ttf' in folder './font/' was not found\n");
		return 1;
    }

    //disable cursor
    if (DISABLE_CURSOR) {SDL_ShowCursor(SDL_DISABLE);}
    
    i8 runProgram = 1;
    while (runProgram) {
        while (instanceRunning == 0) { //Run Menu
            SDL_Event event;
            while (SDL_PollEvent(&event)) { // Go through every event which occured
                if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {runProgram = 0; instanceRunning = 99;}
                if (event.type == SDL_KEYDOWN) {
                    switch (event.key.keysym.scancode) {
                    case SDL_SCANCODE_W:
                    case SDL_SCANCODE_UP:
                        //Move cursor up
                        if (selected-1 >= 0) {selected -= 1;}
                        break;
                    case SDL_SCANCODE_S:
                    case SDL_SCANCODE_DOWN:
                        //Move cursor down
                        if (selected+1 < buttonCount) {selected += 1;}
                        break;
                    case SDL_SCANCODE_RETURN:
                    case SDL_SCANCODE_SPACE:
                        if (selected == 0) {instanceRunning = 2; newGame();} //run game
                        if (selected == 1) {instanceRunning = 1;} //view credits
                        if (selected == 2) {instanceRunning = 99;runProgram = 0;} //quit
                        break;
                    }
                }
            }

            SDL_RenderClear(renderer); //make screen black
            drawBackground(black); //(0 is black, 1 is white, 2 is grey)

            for (int y = 0; y<HEIGHT; y++) {
                for (int x = 0; x<WIDTH; x++) {
                    drawBlock(x,y,menuField[y*WIDTH +x]);
                }
            }

            drawText("Tetris",50, 300,-1,white,titleFont); //Selected "Start" button

            if (selected == 0) { //Selected Start button
                drawText(selectedButtons[0],50, SHEIGHT-220,-1,white,bigFont); //"Start" button
                drawText(        buttons[1],50, SHEIGHT-180,-1,white,bigFont); //"Credits" button
                drawText(        buttons[2],50, SHEIGHT-140,-1,white,bigFont); //"Quit" button
            } else if (selected == 1) { //Selected credits button
                drawText(        buttons[0],50, SHEIGHT-220,-1,white,bigFont); //"Start" button
                drawText(selectedButtons[1],50, SHEIGHT-180,-1,white,bigFont); //"Credits" button
                drawText(        buttons[2],50, SHEIGHT-140,-1,white,bigFont); //"Quit" button
            } else if (selected == 2) { //Selected quit button
                drawText(        buttons[0],50, SHEIGHT-220,-1,white,bigFont); //"Start" button
                drawText(        buttons[1],50, SHEIGHT-180,-1,white,bigFont); //"Credits" button
                drawText(selectedButtons[2],50, SHEIGHT-140,-1,white,bigFont); //"Quit" button
            }


            SDL_RenderPresent(renderer); // triggers the double buffers for multiple rendering
            SDL_Delay(1000 / FPS);
        }
        while (instanceRunning == 1) { //View credits
            SDL_Event event;
            while (SDL_PollEvent(&event)) { // Go through every event which occured
                if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {runProgram = 0; instanceRunning = 99;} //quit
                else if (event.type == SDL_KEYDOWN) {instanceRunning = 0;} //Go back into menu when pressing any key
            }

            SDL_RenderClear(renderer); //make screen black
            drawBackground(black); //0 is black, 1 is white, 2 is grey

            for (int y = 0; y<HEIGHT; y++) {
                for (int x = 0; x<WIDTH; x++) {
                    drawBlock(x,y,0);
                }
            }

            drawText("Tetris",50, 100,-1,white,titleFont); //Selected "Start" button
            drawText("Credits",100, 200,-1,white,bigFont); //Selected "Start" button

            i32 tempPosY = 300;
            for (i8 line = 0; line<numberCreditLines; line++) {
                drawText(credits[line],50, tempPosY+line*30,-1,white,font); //"Start" button
            }


            SDL_RenderPresent(renderer); // triggers the double buffers for multiple rendering
            SDL_Delay(1000 / FPS);
        }
        while (instanceRunning == 2) { //Run Game
            SDL_Event event;
            while (SDL_PollEvent(&event)) { // Go through every event which occured
                if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {instanceRunning = 0;}
                if (event.type == SDL_KEYDOWN) {

                    switch (event.key.keysym.scancode) {
                    case SDL_SCANCODE_W:
                    case SDL_SCANCODE_UP:
                        //Rotate tetrino
                        if (!collide(tPosX, tPosY, tForm, tRot+1)) {
                            tRot++;
                            if (tRot>3) {tRot = 0;}
                        }
                        break;
                    case SDL_SCANCODE_S:
                    case SDL_SCANCODE_DOWN:
                        //Move tetrino faster down
                        fallFaster = 1;
                        break;
                    case SDL_SCANCODE_A:
                    case SDL_SCANCODE_LEFT:
                        //Move tetrino left
                        if (collide(tPosX-1, tPosY, tForm, tRot)==0) {tPosX -= 1;}
                        break;
                    case SDL_SCANCODE_D:
                    case SDL_SCANCODE_RIGHT:
                        //Move tetrino right
                        if (collide(tPosX+1, tPosY, tForm, tRot)==0) {tPosX += 1;}
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
                        fallFaster = 0;
                        break;
                    case SDL_SCANCODE_D:
                    case SDL_SCANCODE_RIGHT:
                        break;
                    }
                } else if (event.type == SDL_USEREVENT) { //This is the event to decide whether to fall down or collide etc.

                    if (fallFaster || fallTimer==0) {
                        if (collide(tPosX, tPosY+1, tForm, tRot)) {
                            if (tPosY <= 0) { //Game Over, when the tetrino detects a collision above the game field
                                instanceRunning = 0; //Go into menu
                                gameOverAnimation();
                            }

                            //Put tetrino into field as solid part
                            for (i8 y = 0; y<tFormWidth[tForm]; y++) {
                                for (i8 x = 0; x<tFormWidth[tForm]; x++) {
                                    if (tForms[tForm][tRot][y * tFormWidth[tForm] + x] != 0) {
                                        field[(tPosY+y) *WIDTH + (tPosX+x)] = tColor;
                                    }
                                }
                            }
                            
                            //Delete line if line is complete (check every row)
                            for (i8 y = 0; y < HEIGHT; y++) {
                                i8 lineComplete = 1;
                                for (i8 x = 0; x<WIDTH; x++) {
                                    if (getBlock(x,y)==0) {lineComplete = 0;} //line isnt complete
                                }

                                if (lineComplete) { 
                                    for (i8 yToShift = y; yToShift>0; yToShift--) {
                                        for (i8 x = 0; x<WIDTH; x++) {
                                            field[yToShift*WIDTH + x] = field[(yToShift-1)*WIDTH + x];
                                        }
                                    }
                                    for (i8 x = 0; x<WIDTH; x++) {field[0*WIDTH + x] = 0;}

                                    score += scoreAddLineDone;
                                    fasterFallSpeed -= 2;
                                    SDL_RemoveTimer(fastFallTimer);
                                    fastFallTimer = SDL_AddTimer(fasterFallSpeed, generateUserEvent, NULL);
                                }
                            }
                            newTetrino();
                            fallTimer = 0;
                        } else {
                            tPosY += 1;
                            score += scoreAddTetrinoFallDown;
                        }
                    }

                    fallTimer += 1;
                    if (fallTimer >= FALLSPEED_FACTOR) {fallTimer = 0;}
                    if (fallFaster) {fallTimer = 1;}
                    sprintf(&scoreString[7], "%d", score);
                }
            }
            
            SDL_RenderClear(renderer); //make screen black
            drawBackground(black); 

            for (int x = 0; x<WIDTH; x++) {
                drawBlock(x,-1,0); //field blocks
            }

            for (int y = -1; y<HEIGHT; y++) {
                for (int x = 0; x<WIDTH; x++) {
                    drawBlock(x,y,getBlock(x,y)); //field blocks
                }
            }

            //drawRect(SOFFSETSX,0,SWIDTH-2*SOFFSETSX, SOFFSETSY, black, 1);

            drawTetrino();

            drawText(scoreString,14, 12,-1,white,font); //scoreString

            SDL_RenderPresent(renderer); // triggers the double buffers for multiple rendering
            SDL_Delay(1000 / FPS);
        }
    }
 
    //clean up
    TTF_CloseFont(font);
    TTF_CloseFont(bigFont);
    TTF_CloseFont(titleFont);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}