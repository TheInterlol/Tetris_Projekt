#include <stdio.h>
#include <stdbool.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include "game.h"
#include "renderer.h"
#include "mainmenu.h"
#include "game_over.h"

SDL_Window *window;
SDL_Renderer *renderer;
SDL_Texture *background;
SDL_Texture *image;
Mix_Music *gMusic;
extern bool GameOver;
extern bool jePauza;
Uint32 countdownStartTime = 0;
int countdownNumber = 3;
extern TTF_Font *gGameFont;

// funkce pro error zprávy
void log_sdl_error(const char *msg)
{
    printf("%s Error: %s\n", msg, SDL_GetError());
}

// funkce co loaduje texturu ze souboru
SDL_Texture *loadTexture(const char *file, SDL_Renderer *ren)
{
    SDL_Texture *texture = IMG_LoadTexture(ren, file);
    if (texture == NULL)
    {
        log_sdl_error("LoadTexture");
    }
    return texture;
}

void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, SDL_Rect dst, SDL_Rect *clip)
{
    SDL_RenderCopy(ren, tex, clip, &dst);
}

void renderTexturePreserveWH(SDL_Texture *tex, SDL_Renderer *ren, int x, int y, SDL_Rect *clip)
{
    SDL_Rect dst;
    dst.x = x;
    dst.y = y;
    if (clip != NULL)
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

SDL_Texture *renderText(SDL_Renderer *ren, const char *text, SDL_Color color, TTF_Font *font)
{
    // renderování textu na surface
    SDL_Surface *surf = TTF_RenderText_Blended(font, text, color);
    if (surf == NULL)
    {
        log_sdl_error("TTF_RenderText");
        return NULL;
    }
    // vytvoření textury z surface
    SDL_Texture *texture = SDL_CreateTextureFromSurface(ren, surf);
    SDL_FreeSurface(surf);
    if (texture == NULL)
    {
        log_sdl_error("CreateTexture");
        return NULL;
    }
    return texture;
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
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    if (renderer == NULL)
    {
        log_sdl_error("SDL_CreateRenderer");
        SDL_DestroyWindow(window);
        SDL_Quit();
    }
    if (TTF_Init() != 0)
    {
        log_sdl_error("TTF_Init");
        SDL_Quit();
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        log_sdl_error("Mix_OpenAudio");
    }

    gMusic = Mix_LoadMUS("../assets/tetoris.mp3");
    if (gMusic == NULL)
    {
        printf("Nepodařilo se načíst hudbu! Chyba: %s\n", Mix_GetError());
    }

    background = IMG_LoadTexture(renderer, "../assets/UI/background.jpg");
    image = IMG_LoadTexture(renderer, "../assets/Textury/block_red.png");

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
    if (gMusic != NULL)
    {
        Mix_FreeMusic(gMusic);
        gMusic = NULL;
    }
    Mix_CloseAudio();
    IMG_Quit();
    SDL_Quit();
}

int main()
{
    engine_inicializace();

    menu_init(renderer);
    game_init();
    gameover_init();
    if (Mix_PlayMusic(gMusic, -1) == -1)
    {
        printf("Nepodařilo se přehrát hudbu! Chyba: %s\n", Mix_GetError());
    }
    Mix_VolumeMusic(MIX_MAX_VOLUME / 6);

    SDL_Event e;
    bool quit = false;
    int currentState = STATE_MAIN_MENU;
    int nextState = STATE_MAIN_MENU;

    while (!quit)
    {
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
            {
                quit = true;
            }

            switch (currentState)
            {
            case STATE_MAIN_MENU:
                nextState = menu_input(&e, &quit);
                break;
            case STATE_PLAYING:
                game_input(&e, &quit);
                if (GameOver)
                {
                    nextState = STATE_GAME_OVER;
                }
                break;
            case STATE_PAUSED:
                jePauza = !jePauza;
                break;
            case STATE_GAME_OVER:
                nextState = gameover_input(&e);
                break;
            default:
                break;
            }
        }

        if (currentState == STATE_PLAYING)
        {
            game_update();
        }
        else if (currentState == STATE_COUNTDOWN)
        {
            Uint32 currentTime = SDL_GetTicks();
            if (currentTime - countdownStartTime >= 1000)
            {
                countdownNumber--;
                countdownStartTime = currentTime;

                if (countdownNumber == 0)
                {
                    nextState = STATE_PLAYING;
                }
            }
        }

        // K vyčištění obrazovk
        SDL_RenderClear(renderer);
        switch (currentState)
        {
        case STATE_MAIN_MENU:
            menu_renderer(renderer);
            break;
        case STATE_PLAYING:
            game_render(renderer, background);
            break;
        case STATE_COUNTDOWN:
            game_render(renderer, background);
            char buffer[64];
            sprintf(buffer, "%d", countdownNumber);
            SDL_Rect box = {ScreenWidth / 2 - 50, ScreenHeight / 2 - 75, 100, 100};
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 150);
            SDL_RenderFillRect(renderer, &box);
            draw_text(renderer, gGameFont, (SDL_Color){255, 255, 255, 255}, (SDL_Rect){ScreenWidth / 2, ScreenHeight / 2 - 60, 0, 0}, buffer);
            break;
        case STATE_GAME_OVER:
            gameover_renderer(renderer, background);
            break;
        default:
            break;
        }
        // K zobrazení vykresleného obsahu
        SDL_RenderPresent(renderer);

        if (nextState != currentState)
        {

            if (currentState == STATE_MAIN_MENU && nextState == STATE_PLAYING)
            {
                restart();

                countdownStartTime = SDL_GetTicks();
                countdownNumber = 3;
                currentState = STATE_COUNTDOWN;
                nextState = STATE_COUNTDOWN;
            }
            else
            {
                currentState = nextState;
            }
        }
    }
    menu_quit();
    game_quit();
    clear_everything();

    return 0;
}