#ifndef GAME_H
#define GAME_H
#include <SDL.h>
#include <stdbool.h>
#include <SDL_ttf.h>

// Konstanty
#define ScreenWidth 1120
#define ScreenHeight 840
#define TILE_SIZE 32
#define TetrisGridWidth 10
#define TetrisGridHeight 20
#define TetrisGridOffset 100

typedef enum GameState
{
    STATE_MAIN_MENU = 0,
    STATE_TOP_SCORE = 1,
    STATE_PLAYING = 2,
    STATE_PAUSED = 3,
    STATE_GAME_OVER = 4,
    STATE_COUNTDOWN = 5

} GameState;

// inicializace hry
void game_init();
// vstupy
void game_input(SDL_Event *e, bool *quit);
// loop
void game_update(void);
// vykesleni
void game_render(SDL_Renderer *renderer, SDL_Texture *background);
// restart hry
void restart();
// ukonceni hry
void game_quit(void);

#endif