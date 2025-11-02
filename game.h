#define GAME_H
#include <SDL.h>
#include <stdbool.h>

// Konstanty
#define ScreenWidth 1120
#define ScreenHeight 840
#define TILE_SIZE 36
#define TetrisGridWidth 10
#define TetrisGridHeight 20

// inicializace hry
void game_init(void);
//vstupy
void game_input(SDL_Event *e, bool *quit);
//loop
void game_update(void);
//vykesleni
void game_render(SDL_Renderer *renderer, SDL_Texture *background, SDL_Texture *blocky);
//ukonceni hry
void game_quit(void);