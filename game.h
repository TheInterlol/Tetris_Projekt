#define GAME_H
#include <SDL.h>
#include <stdbool.h>

#define ScreenWidth 1120
#define ScreenHeight 840
#define TILE_SIZE 40
#define TetrisGridWidth 10
#define TetrisGridHeight 20


void game_init(void);

void game_input(SDL_Event *e, bool *quit);

void game_update(void);

void game_render(SDL_Renderer *renderer, SDL_Texture *background, SDL_Texture *blocky);
