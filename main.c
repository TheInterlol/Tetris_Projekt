#include <stdio.h>
#include <stdbool.h>
#include <SDL.h>
#include <SDL_image.h>

void log_sdl_error(const char *msg)
{
    printf("%s Error: %s\n", msg, SDL_GetError());
}

SDL_Texture *loadTexture(const char *file, SDL_Renderer *ren)
{
    SDL_Texture *texture = IMG_LoadTexture(ren, file);
    SDL_Surface *loadedImage = IMG_Load(file);
    if (loadedImage != NULL)
    {
        texture = SDL_CreateTextureFromSurface(ren, loadedImage);
        SDL_FreeSurface(loadedImage);
        if (texture == NULL)
        {
            log_sdl_error("SDL_CreateTextureFromSurface");
        }
        else
        {
            log_sdl_error("Texture loaded successfully");
        }
    }
    if(texture == NULL)
    {
        log_sdl_error("LoadTexture");
    }
    return texture;
}

void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y)
{
    SDL_Rect dst;
    dst.x = x;
    dst.y = y;
    SDL_QueryTexture(tex, NULL, NULL, &dst.w, &dst.h);
    SDL_RenderCopy(ren, tex, NULL, &dst);
}

void renderTextureSpecialWH(SDL_Texture *tex, SDL_Renderer *ren, int x, int y, int w, int h){
    SDL_Rect dst;
    dst.x = x;
    dst.y = y;
    dst.w = w;
    dst.h = h;
    SDL_RenderCopy(ren, tex, NULL, &dst);
}

void renderTexturePreserveWH(SDL_Texture *tex, SDL_Renderer *ren, int x, int y){
    int w, h;
    SDL_QueryTexture(tex, NULL, NULL, &w, &h);
    renderTextureSpecialWH(tex, ren, x, y, w, h);
}

int main(int argc, char *argv[])
{

    const int ScreenWidth = 1120;
    const int ScreenHeight = 840;

    const int TILE_SIZE = 40;

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        log_sdl_error("SDL_Init");
        return 1;
    }
    if((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) != IMG_INIT_PNG)
    {
        log_sdl_error("IMG_Init");
        SDL_Quit();
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Hello world!", 100, 100, ScreenWidth, ScreenHeight, SDL_WINDOW_SHOWN);
    if (window == NULL)
    {
        log_sdl_error("SDL_CreateWindow");
        SDL_Quit();
    }
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL)
    {
        log_sdl_error("SDL_CreateRenderer");
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    const char *image_path = "assets/background.png";
    SDL_Surface *png = IMG_Load(image_path);
    if (png == NULL)
    {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        printf("IMG_Load Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, png);
    SDL_FreeSurface(png);
    if (tex == NULL)
    {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        printf("SDL_CreateTextureFromSurface Error: %s\n", SDL_GetError());
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

    SDL_RenderClear(renderer);

    int xTiles = ScreenWidth / TILE_SIZE;
    int yTiles = ScreenHeight / TILE_SIZE;

    for(int i = 0; i < xTiles * yTiles; i++){
        int x = i % xTiles;
        int y = i / xTiles;
        renderTextureSpecialWH(tex, renderer, x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE);
    }

    int iW, iH;
    SDL_QueryTexture(image, NULL, NULL, &iW, &iH);
    int x = ScreenWidth / 2 - iW / 2;
    int y = ScreenHeight / 2 - iH / 2;
    renderTexturePreserveWH(image, renderer, x, y);

    SDL_RenderPresent(renderer);
    SDL_Delay(10000);

    SDL_DestroyTexture(background);
    SDL_DestroyTexture(image);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}