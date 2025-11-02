#include <stdio.h>
#include <stdbool.h>
#include <SDL.h>
#include <SDL_image.h>
#include "game.h"
#include "renderer.h"

SDL_Window *window;
SDL_Renderer *renderer;
SDL_Texture *background;
SDL_Texture *image;

//funkce pro error zprávy
void log_sdl_error(const char *msg)
{
    printf("%s Error: %s\n", msg, SDL_GetError());
}

//funkce co loaduje texturu ze souboru
SDL_Texture *loadTexture(const char *file, SDL_Renderer *ren)
{
    SDL_Texture *texture = IMG_LoadTexture(ren, file);
    if (texture == NULL)
    {
        log_sdl_error("LoadTexture");
    }
    return texture;
}

void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, SDL_Rect dst, SDL_Rect *clip){
    SDL_RenderCopy(ren, tex, clip, &dst);
}

void renderTexturePreserveWH(SDL_Texture *tex, SDL_Renderer *ren, int x, int y, SDL_Rect *clip)
{
    SDL_Rect dst;
    dst.x = x;
    dst.y = y;
    if(clip != NULL)
    {
        dst.w = clip->w;
        dst.h = clip->h;
    }
    else
    {
        SDL_QueryTexture(tex, NULL, NULL, &dst.w, &dst.h);
    }
    renderTexture(tex, ren, dst, clip);
}

void engine_inicializace(void)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        log_sdl_error("SDL_Init");
    }
    if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) != IMG_INIT_PNG)
    {
        log_sdl_error("IMG_Init");
        SDL_Quit();
    }

    window = SDL_CreateWindow("Tetris.exe", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, ScreenWidth, ScreenHeight, SDL_WINDOW_SHOWN);
    if (window == NULL)
    {
        log_sdl_error("SDL_CreateWindow");
        SDL_Quit();
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL)
    {
        log_sdl_error("SDL_CreateRenderer");
        SDL_DestroyWindow(window);
        SDL_Quit();
    }
    background = loadTexture("../assets/background.png", renderer);
    image = loadTexture("../assets/block_red.png", renderer);
    
    if (background == NULL || image == NULL)
    {
        SDL_DestroyTexture(background);
        SDL_DestroyTexture(image);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
    }
}
void clear_everything(void)
{
    SDL_DestroyTexture(background);
    SDL_DestroyTexture(image);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
}   

int main(int argc, char **argv)
{

    engine_inicializace();
    game_init();
    SDL_Event e;
    bool quit = false;
    while (!quit)
    {
        while(SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
            {
                quit = true;
            }

            game_input(&e, &quit);
        }

        game_update();
        //K vyčištění obrazovky
        SDL_RenderClear(renderer);
        game_render(renderer, background, image);
        //K zobrazení vykresleného obsahu
        SDL_RenderPresent(renderer);
    }
    clear_everything();

    return 0;
}