#include <stdio.h>
#include <stdbool.h>
#include <SDL.h>
#include <SDL_image.h>

//výška, šířka okna
const int ScreenWidth = 1120;
const int ScreenHeight = 840;

const int TILE_SIZE = 40;

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

//funkce co vykresluje texturu
void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, SDL_Rect dst, SDL_Rect *clip){
	SDL_RenderCopy(ren, tex, clip, &dst);
}

//funkce co vykresluje texturu na danou pozici
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

int main(int argc, char **argv)
{

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        log_sdl_error("SDL_Init");
        return 1;
    }
    if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) != IMG_INIT_PNG)
    {
        log_sdl_error("IMG_Init");
        SDL_Quit();
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Tetris.exe", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, ScreenWidth, ScreenHeight, SDL_WINDOW_SHOWN);
    if (window == NULL)
    {
        log_sdl_error("SDL_CreateWindow");
        SDL_Quit();
        return 1;
    }
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL)
    {
        log_sdl_error("SDL_CreateRenderer");
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    SDL_Texture *background = loadTexture("assets/background.png", renderer);
    SDL_Texture *image = loadTexture("assets/block1.png", renderer);
    
    if (background == NULL || image == NULL)
    {
        SDL_DestroyTexture(background);
        SDL_DestroyTexture(image);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }
    
    int iW, iH;
    SDL_QueryTexture(image, NULL, NULL, &iW, &iH);
    int x = ScreenWidth / 2 - iW / 2;
    int y = ScreenHeight / 2 - iH / 2;

    SDL_Event e;
    bool quit = false;
    while (!quit)
    {
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
            {
                quit = true;
            }
            if (e.type == SDL_KEYDOWN)
            {
                quit = true; 
            }
            if (e.type == SDL_MOUSEBUTTONDOWN)
            {
                quit = true;
            }
        }

        //K vyčištění obrazovky
        SDL_RenderClear(renderer);

        //K vykreslení pozadí
        SDL_Rect bgDst = {0, 0, ScreenWidth, ScreenHeight};
        renderTexture(background, renderer, bgDst, NULL);

        //K vykreslení obrázku na střed obrazovky
        renderTexturePreserveWH(image, renderer, x, y, NULL);

        //K zobrazení vykresleného obsahu
        SDL_RenderPresent(renderer);
    }
    SDL_DestroyTexture(background);
    SDL_DestroyTexture(image);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}