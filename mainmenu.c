#include <stdio.h>
#include <stdbool.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include "game.h"
#include "renderer.h"
#include "mainmenu.h"

SDL_Texture *pozadi = NULL;
TTF_Font *mMenuFont = NULL;

void menu_init(SDL_Renderer *renderer)
{
    pozadi = IMG_LoadTexture(renderer, "../assets/UI/main_menu.jpg");
    mMenuFont = TTF_OpenFont("../assets/Fonty/8-bitArcadeIn.ttf", 54);
}
void menu_renderer(SDL_Renderer *renderer)
{
    if (pozadi != NULL)
    {
        SDL_Rect bgDst = {0, 0, ScreenWidth, ScreenHeight};
        renderTexture(pozadi, renderer, bgDst, NULL);
    }
    else
    {
        SDL_SetRenderDrawColor(renderer, 20, 20, 40, 255);
        SDL_RenderClear(renderer);
    }
    draw_text(renderer, mMenuFont, (SDL_Color){0, 0, 0, 255}, (SDL_Rect){500, 500, 0, 0}, "Start");
    draw_text(renderer, mMenuFont, (SDL_Color){0, 0, 0, 255}, (SDL_Rect){450, 600, 0, 0}, "Top Score");
}
GameState menu_input(SDL_Event *e, bool *quit)
{
    if (e->type == SDL_KEYDOWN)
    {
        switch (e->key.keysym.sym)
        {
        case SDLK_RETURN:
            return STATE_PLAYING;
            break;
        case SDLK_t:
            return STATE_TOP_SCORE;
            break;
        case SDLK_ESCAPE:
            *quit = true;
            break;
        }
    }
    return STATE_MAIN_MENU;
}

void menu_quit(void)
{
    if (pozadi != NULL)
    {
        SDL_DestroyTexture(pozadi);
        pozadi = NULL;
    }
    if (mMenuFont != NULL)
    {
        TTF_CloseFont(mMenuFont);
        mMenuFont = NULL;
    }
}
