#include "game.h"
#include "renderer.h"
#include <SDL.h>
#include <SDL_image.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

// všechny proměnné pro hru
int *hraci_pole;
bool Key[4];
int CurrentPiece;
int CurrentRotation;
int CurrentX = TetrisGridWidth / 2;
int CurrentY = 0;
int Speed = 20;
int SpeedCount = 0;
bool ForceDown = false;
int PieceCount = 0;
int score = 0;
bool GameOver = false;

// Všech 7 tetromin
char *Tetromino[7] = {
    "..X...X...X...X.", // I
    "..X..XX...X.....", // T
    ".....XX..XX.....", // O
    "..X..XX..X......", // S
    ".X...XX...X.....", // Z
    ".X...X...XX.....", // J
    "..X...X..XX....."  // L
};

// Otáčí tetromino :o
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

// kontroluje zda se vejde tetromino na danou pozici
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
                    if (Tetromino[nTetromino][pi] == 'X' && hraci_pole[fi] != 0)
                    {
                        return false;
                    }
                }
            }
        }
    }
    return true;
}
void nextPiece(void)
{
    CurrentX = TetrisGridWidth / 2;
    CurrentY = 0;
    CurrentRotation = 0;
    CurrentPiece = rand() % 7;

    if (!doesPieceFit(CurrentPiece, CurrentRotation, CurrentX, CurrentY))
    {
        GameOver = true;
        game_quit();
    }
}

void game_init(void)
{
    srand(time(NULL));

    hraci_pole = (int *)malloc(TetrisGridWidth * TetrisGridHeight * sizeof(int));
    for (int x = 0; x < TetrisGridWidth; x++)
    {
        for (int y = 0; y < TetrisGridHeight; y++)
        {
            // na každý okraj dám 9, jinak 0 -> když 0 tak chill, v moment ale co by tetromino najelo na 9 tak dál nepujde
            hraci_pole[y * TetrisGridWidth + x] = (x == 0 || x == TetrisGridWidth - 1 || y == TetrisGridHeight - 1) ? 9 : 0;
        }
    }

    nextPiece();
}

void game_input(SDL_Event *e, bool *quit)
{

    if (e->type == SDL_KEYDOWN)
    {
        switch (e->key.keysym.sym)
        {
            // pohyb tetromina -> vždy kontrola zda vůbec lze tetronimo tím směrem posunout
        case SDLK_LEFT:
            if (doesPieceFit(CurrentPiece, CurrentRotation, CurrentX - 1, CurrentY))
            {
                CurrentX--;
            }
            break;
        case SDLK_RIGHT:
            if (doesPieceFit(CurrentPiece, CurrentRotation, CurrentX + 1, CurrentY))
            {
                CurrentX++;
            }
            break;
        case SDLK_DOWN:
            if (doesPieceFit(CurrentPiece, CurrentRotation, CurrentX, CurrentY + 1))
            {
                CurrentY++;
                SpeedCount++;
            }
            break;
        case SDLK_UP:
            if (doesPieceFit(CurrentPiece, CurrentRotation + 1, CurrentX, CurrentY))
            {
                CurrentRotation = (CurrentRotation + 1) % 4;
            }
            break;
        case SDLK_ESCAPE:
            *quit = true;
            break;
        default:
            break;
        }
    }
}
void game_update(void)
{

    if (GameOver)
        return;

    SpeedCount++;
    bool forceDown = (SpeedCount == Speed);

    //"gravitace" tetronima, jeho přirozený pohyb
    // pokud pod ním nic není, padá dál, jinak se zastaví a další tetronimo ho nahradí
    if (forceDown)
    {
        SpeedCount = 0;
        if (doesPieceFit(CurrentPiece, CurrentRotation, CurrentX, CurrentY + 1))
        {
            CurrentY++;
        }
        else
        {
            // tetromino zustane dole
            for (int px = 0; px < 4; px++)
            {
                for (int py = 0; py < 4; py++)
                {
                    int pi = Rotate(px, py, CurrentRotation);
                    if (Tetromino[CurrentPiece][pi] == 'X')
                    {
                        int fi = (CurrentY + py) * TetrisGridWidth + (CurrentX + px);
                        hraci_pole[fi] = CurrentPiece + 1;
                    }
                }
            }
            nextPiece();
        }
    }

    // kontrola zda je nějaký řádek plný
    for (int y = 0; y < TetrisGridHeight - 1; y++)
    {
        bool line = true;
        for (int x = 1; x < TetrisGridWidth - 1; x++)
        {
            if (hraci_pole[y * TetrisGridWidth + x] == 0)
            {
                line = false;
                break;
            }
        }

        if (line)
        {
            // Posuneme vše nad ním dolů.
            for (int py = y; py > 0; py--)
            {
                for (int px = 1; px < TetrisGridWidth - 1; px++)
                {

                    hraci_pole[py * TetrisGridWidth + px] = hraci_pole[(py - 1) * TetrisGridWidth + px];
                }
            }
            // Vymazání řádku
            for (int px = 1; px < TetrisGridWidth - 1; px++)
            {
                hraci_pole[px] = 0; 
            }
            score += 100;
        }
    }
}

void game_render(SDL_Renderer *renderer, SDL_Texture *background, SDL_Texture *blocky)
{
    SDL_Rect bgDst = {0, 0, ScreenWidth, ScreenHeight};
    renderTexture(background, renderer, bgDst, NULL);

    SDL_Rect gameBoardDst = {0, 0, TetrisGridWidth * TILE_SIZE, TetrisGridHeight * TILE_SIZE};
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 50);
    SDL_RenderDrawRect(renderer, &gameBoardDst);

    for (int i = 0; i < TetrisGridWidth; i++)
    {
        for (int j = 0; j < TetrisGridHeight; j++)
        {
            int block = hraci_pole[j * TetrisGridWidth + i];
            if (block > 0 && block < 9)
            {
                renderTexturePreserveWH(blocky, renderer, i * TILE_SIZE, j * TILE_SIZE, NULL);
            }
        }
    }

    // vykreslí block na každou pozici v tetrominu kde je 'X'
    if (!GameOver)
    {
        for (int x = 0; x < 4; x++)
        {
            for (int y = 0; y < 4; y++)
            {
                int r = Rotate(x, y, CurrentRotation);

                if (Tetromino[CurrentPiece][r] == 'X')
                {

                    renderTexturePreserveWH(blocky, renderer, (CurrentX + x) * TILE_SIZE, (CurrentY + y) * TILE_SIZE, NULL);
                }
            }
        }
    }
}
void game_quit(void)
{
    if (hraci_pole != NULL)
    {
        free(hraci_pole);
        hraci_pole = NULL;
    }
}
