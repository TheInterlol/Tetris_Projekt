#include <stdio.h>
#include <stdbool.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include "game.h"
#include "renderer.h"
#include "mainmenu.h"
#include "game_over.h"

TTF_Font *gGameOverFont = NULL;
extern int score;
static int highscore = 0;

void gameover_init()
{
    gGameOverFont = TTF_OpenFont("../assets/Fonty/8-bitArcadeIn.ttf", 54);
}

void gameover_renderer(SDL_Renderer *renderer, SDL_Texture *background)
{
    if (background != NULL)
    {
        SDL_Rect bgDst = {0, 0, ScreenWidth, ScreenHeight};
        renderTexture(background, renderer, bgDst, NULL);
    }
    else
    {
        SDL_SetRenderDrawColor(renderer, 20, 20, 40, 255);
        SDL_RenderClear(renderer);
    }
    draw_text(renderer, gGameOverFont, (SDL_Color){0, 0, 0, 255}, (SDL_Rect){450, 300, 0, 0}, "Game Over");

    SDL_Rect holdOuterRect = {360, 300, 400, 400};
    SDL_Rect holdInnerRect = {385, 325, 350, 350};
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(renderer, &holdOuterRect);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &holdInnerRect);
    if (score > highscore)
    {
        draw_text(renderer, gGameOverFont, (SDL_Color){0, 0, 0, 255}, (SDL_Rect){450, 400, 0, 0}, "New High Score");
        char buffer[64];
        sprintf(buffer, "%04d", score);
        draw_text(renderer, gGameOverFont, (SDL_Color){0, 0, 0, 255}, (SDL_Rect){450, 600, 0, 0}, buffer);
        draw_text(renderer, gGameOverFont, (SDL_Color){0, 0, 0, 255}, (SDL_Rect){450, 600, 0, 0}, "Congratulations!");
        highscore = score;
    }
    else
    {
        draw_text(renderer, gGameOverFont, (SDL_Color){0, 0, 0, 255}, (SDL_Rect){440, 375, 0, 0}, "Your Score");
        char buffer[64];
        sprintf(buffer, "%04d", score);
        draw_text(renderer, gGameOverFont, (SDL_Color){0, 0, 0, 255}, (SDL_Rect){500, 475, 0, 0}, buffer);
        draw_text(renderer, gGameOverFont, (SDL_Color){0, 0, 0, 255}, (SDL_Rect){450, 575, 0, 0}, "Good job!");
    }
}

GameState gameover_input(SDL_Event *e)
{
    if (e->type == SDL_KEYDOWN)
    {
        switch (e->key.keysym.sym)
        {
        case SDLK_RETURN:
            return STATE_MAIN_MENU;
            break;
        case SDLK_ESCAPE:
            return STATE_MAIN_MENU;
            break;
        }
    }
    return STATE_GAME_OVER;
}

void gameover_quit(void)
{
    if (gGameOverFont != NULL)
    {
        TTF_CloseFont(gGameOverFont);
        gGameOverFont = NULL;
    }
}