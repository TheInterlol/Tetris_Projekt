#include <SDL.h>
#include "game.h"

void menu_init(SDL_Renderer *renderer);

void menu_renderer(SDL_Renderer *renderer);

GameState menu_input(SDL_Event *e, bool *quit);

void menu_quit(void);