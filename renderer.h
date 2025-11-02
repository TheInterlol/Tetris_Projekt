
#define RENDERER_H
#include <SDL.h>

// funkce co vykresluje texturu
void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, SDL_Rect dst, SDL_Rect *clip);

// funkce co vykresluje texturu na danou pozici
void renderTexturePreserveWH(SDL_Texture *tex, SDL_Renderer *ren, int x, int y, SDL_Rect *clip);
