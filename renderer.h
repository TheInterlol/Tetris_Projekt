
#define RENDERER_H
#include <SDL.h>
#include <SDL_ttf.h>

// funkce co vykresluje texturu
void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, SDL_Rect dst, SDL_Rect *clip);

// funkce co vykresluje texturu na danou pozici
void renderTexturePreserveWH(SDL_Texture *tex, SDL_Renderer *ren, int x, int y, SDL_Rect *clip);

// funkce co renderuje text na texturu
SDL_Texture *renderText(SDL_Renderer *ren, const char *text, SDL_Color color, TTF_Font *font);

// text
void draw_text(SDL_Renderer *renderer, TTF_Font *font, SDL_Color color, SDL_Rect location, const char *text);
