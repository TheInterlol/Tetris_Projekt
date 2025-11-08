#include <SDL.h>
#include "game.h"

void gameover_init();

void gameover_renderer(SDL_Renderer *renderer, SDL_Texture *background);

GameState gameover_input(SDL_Event *e);

void gameover_quit(void);