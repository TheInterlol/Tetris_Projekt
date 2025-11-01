#include "game.h"
#include "renderer.h"
#include <SDL.h>
#include <stdbool.h>
#include <stdlib.h>

int *hraci_pole;

char * Tetromino[7] = {
    "..X...X...X...X.", //I
    "..X..XX...X.....", //T
    ".....XX..XX.....", //O
    "..X..XX..X......", //S
    ".X...XX...X.....", //Z
    ".X...X...XX.....", //J
    "..X...X..XX....."  //L
};

int Rotate(int px, int py, int r)
{
    switch (r % 4)
    {
    case 0:
        return py * 4 + px;
    case 1:
        return 12 + py - (px * 4);
    case 2:
        return 15 - (py * 4) - px;
    case 3:
        return 3 - py + (px * 4);
    }
    return 0;
}

bool doesPieceFit(int nTetromino, int nRotation, int nPosX, int nPosY)
{

    for (int px = 0; px < 4; px++)
    {
        for (int py = 0; py < 4; py++)
        {
            int pi = Rotate(px, py, nRotation);
            int fi = (nPosY + py) * TetrisGridWidth + (nPosX + px);
            if (nPosX + px >= 0 && nPosX + px < TetrisGridWidth)
            {
                if (nPosY + py >= 0 && nPosX + px < TetrisGridHeight)
                {
                    if(Tetromino[nTetromino][pi] == 'X' && hraci_pole[fi] != 0)
                    {
                        return false;
                    }
                }
            }
        }
    }

    return true;
}
void game_init(void)
{   
    hraci_pole = (int *)malloc(TetrisGridWidth * TetrisGridHeight * sizeof(int));
    for (int x = 0; x < TetrisGridWidth; x++)
    {
        for (int y = 0; y < TetrisGridHeight; y++)
        {
            hraci_pole[y * TetrisGridWidth + x] = (x == 0 || x == TetrisGridWidth - 1 || y == TetrisGridHeight - 1) ? 9 : 0;
        }
    }
    free(hraci_pole);
    
}
void game_input(SDL_Event *e, bool *quit)
{
    while(!quit){
        while (SDL_PollEvent(e))
        {
            if (e->type == SDL_QUIT)
            {
                *quit = true;
            }
            if (e->type == SDL_KEYDOWN)
            {
                switch (e->key.keysym.sym)
                {
                case SDLK_LEFT:
                    //pohyb doleva
                    break;
                case SDLK_RIGHT:
                    //pohyb doprava
                    break;
                case SDLK_DOWN:
                    //pohyb dolu
                    break;
                case SDLK_UP:
                    //otočení
                    break;
                case SDLK_SPACE:
                    //pád dolu
                    break;
                default:
                    break;
                }
            }
        }
    }
}
void game_update(void)
{

}
void game_render(SDL_Renderer *renderer, SDL_Texture *background, SDL_Texture *blocky)
{

}

