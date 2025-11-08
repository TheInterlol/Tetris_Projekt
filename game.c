#include "game.h"
#include "renderer.h"
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

// všechny proměnné pro hru
int *hraci_pole;
TTF_Font *gGameFont = NULL;

int CurrentPiece;
int nextPiece1;
int nextPiece2;
int CurrentRotation;
int CurrentX = TetrisGridWidth / 2;
int CurrentY = 0;
int Speed = 50;
int SpeedCount = 0;

int PieceCount = 0;
int score = 0;
int boardoffsetY = 50;
int held = -1;
Uint32 TimeSinceComboStarted = 0;
int combo = 0;
int lastHoldAtPieceCount = -1;
int boardoffsetX = (ScreenWidth / 2) - 160;
int lines = 0;
Uint32 sTime;
int sekundy;
int highScore = 0;
extern SDL_Renderer *renderer;
extern GameState currnetState;

bool GameOver = false;
bool ForceDown = false;
bool jePauza = false;
bool madeALine = false;

SDL_Texture *texI = NULL;
SDL_Texture *texT = NULL;
SDL_Texture *texO = NULL;
SDL_Texture *texS = NULL;
SDL_Texture *texZ = NULL;
SDL_Texture *texL = NULL;
SDL_Texture *texJ = NULL;

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

void draw_text(SDL_Renderer *renderer, TTF_Font *font, SDL_Color color, SDL_Rect location, const char *text)
{
    SDL_Surface *surf = TTF_RenderText_Blended(font, text, color);
    SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surf);

    SDL_Rect loc = location;
    loc.w = surf->w;
    loc.h = surf->h;

    SDL_SetTextureAlphaMod(tex, color.a);
    SDL_RenderCopy(renderer, tex, NULL, &loc);
    SDL_DestroyTexture(tex);
    SDL_FreeSurface(surf);
}

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
                if (nPosY + py >= 0 && nPosY + py < TetrisGridHeight)
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
    // dodělat aby po holdu se nespawnul to stejne tetromino

    CurrentX = TetrisGridWidth / 2;
    CurrentY = 0;
    CurrentRotation = 0;
    CurrentPiece = nextPiece1;
    nextPiece1 = nextPiece2;
    nextPiece2 = rand() % 7;

    if (!doesPieceFit(CurrentPiece, CurrentRotation, CurrentX, CurrentY))
    {
        GameOver = true;
    }
}
// holduje tetromino
int hold(int tetromino)
{
    if (lastHoldAtPieceCount == PieceCount)
    {
        return tetromino;
    }
    if (held == -1)
    {
        held = tetromino;
        nextPiece();
    }
    else
    {
        int temp = held;
        held = tetromino;
        lastHoldAtPieceCount = PieceCount;
        CurrentY = 0;
        return temp;
    }
    lastHoldAtPieceCount = PieceCount;
    return held;
}

// restart hry
void restart()
{
    GameOver = false;
    jePauza = false;
    
    held = -1;
    score = 0;
    PieceCount = 0;
    combo = 0;
    lines = 0;
    sekundy = 0;
    SpeedCount = 0;
    lastHoldAtPieceCount = -1;
    
    game_quit();
    game_init();
 
    nextPiece();
}

// inicializace hry
void game_init()
{

    sTime = SDL_GetTicks();
    srand(time(NULL));

    gGameFont = TTF_OpenFont("../assets/Fonty/8-bitArcadeIn.ttf", 54);

    nextPiece1 = rand() % 7;
    nextPiece2 = rand() % 7;

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

// vstupy z klavesnice
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
                score+=10;
            }
            break;
        case SDLK_UP:
            if (doesPieceFit(CurrentPiece, CurrentRotation + 1, CurrentX, CurrentY))
            {
                CurrentRotation = (CurrentRotation + 1) % 4;
            }
            break;
        case SDLK_c:
            CurrentPiece = hold(CurrentPiece);
            break;
        case SDLK_r:
            restart();
            break;
        case SDLK_p:
            jePauza = !jePauza;
            break;
        case SDLK_ESCAPE:
            *quit = true;
            break;
        default:
            break;
        }
    }
}

// main loop
void game_update(void)
{
    if (jePauza)
        return;
    if (GameOver)
        return;

    Uint32 cTime = SDL_GetTicks();
    Uint32 milisekundy = cTime - sTime;
    sekundy = milisekundy / 1000;
    int LinesClearedThisTick = 0;

    if (combo > 0)
    {
        Uint32 currentTime = SDL_GetTicks();
        if (currentTime - TimeSinceComboStarted > 5000)
        {
            combo = 0;
        }
    }

    SpeedCount++;
    bool forceDown = (SpeedCount >= Speed);

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
            PieceCount++;
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
            lines++;
            LinesClearedThisTick++;
            score += 100;
        }
    }

    if (LinesClearedThisTick > 0)
    {
        combo += LinesClearedThisTick;
        score += (100 * LinesClearedThisTick) * combo;
        TimeSinceComboStarted = SDL_GetTicks();
    }
}

void blocky_barvy_render(SDL_Renderer *renderer, int type, int x, int y)
{

    // přiřazení textur k jednotlivým tetrominům
    if (texI == NULL)
    {
        texI = IMG_LoadTexture(renderer, "../assets/Textury/block_purple.png"); // Fialova
        texT = IMG_LoadTexture(renderer, "../assets/Textury/block_yellow.png"); // Zluta
        texO = IMG_LoadTexture(renderer, "../assets/Textury/block_orange.png"); // Oranzova
        texS = IMG_LoadTexture(renderer, "../assets/Textury/block_green.png");  // Zelena
        texZ = IMG_LoadTexture(renderer, "../assets/Textury/block_red.png");    // Cervena
        texL = IMG_LoadTexture(renderer, "../assets/Textury/block_blue.png");   // Modra
        texJ = IMG_LoadTexture(renderer, "../assets/Textury/block_brown.png");  // Hneda
    }
    SDL_Rect dst = {x, y, TILE_SIZE, TILE_SIZE};

    switch (type)
    {
    case 0: // I
        SDL_RenderCopy(renderer, texI, NULL, &dst);
        break;
    case 1: // T
        SDL_RenderCopy(renderer, texT, NULL, &dst);
        break;
    case 2: // O
        SDL_RenderCopy(renderer, texO, NULL, &dst);
        break;
    case 3: // S
        SDL_RenderCopy(renderer, texS, NULL, &dst);
        break;
    case 4: // Z
        SDL_RenderCopy(renderer, texZ, NULL, &dst);
        break;
    case 5: // L
        SDL_RenderCopy(renderer, texL, NULL, &dst);
        break;
    case 6: // J
        SDL_RenderCopy(renderer, texJ, NULL, &dst);
        break;
    default:
        break;
    }
}

// renderuje tetromina a hrací pole
void game_render(SDL_Renderer *renderer, SDL_Texture *background)
{

    // pozadí
    SDL_Rect bgDst = {0, 0, ScreenWidth, ScreenHeight};
    renderTexture(background, renderer, bgDst, NULL);

    // hlavní hraci pole
    SDL_Rect outerRect = {boardoffsetX, boardoffsetY, TetrisGridWidth * TILE_SIZE, TetrisGridHeight * TILE_SIZE};
    SDL_Rect innerRect = {((ScreenWidth / 2) - 2) - 150, boardoffsetY + 7, (TetrisGridWidth - 0.5) * TILE_SIZE, (TetrisGridHeight - 0.5) * TILE_SIZE};
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &outerRect);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(renderer, &innerRect);

    // holding area
    SDL_Rect holdOuterRect = {boardoffsetX - 190, boardoffsetY, 6 * TILE_SIZE, 6 * TILE_SIZE};
    SDL_Rect holdInnerRect = {boardoffsetX - 180, boardoffsetY + 7, (6 - 0.5) * TILE_SIZE, (6 - 0.5) * TILE_SIZE};
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &holdOuterRect);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(renderer, &holdInnerRect);

    // upcoming tetromina area
    SDL_Rect nextOuterRect = {boardoffsetX + 315, boardoffsetY, 6 * TILE_SIZE, 12 * TILE_SIZE};
    SDL_Rect nextInnerRect = {boardoffsetX + 325, boardoffsetY + 7, (6 - 0.5) * TILE_SIZE, (12 - 0.5) * TILE_SIZE};
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &nextOuterRect);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(renderer, &nextInnerRect);

    //-------------------------------------------------------------------------------
    // boxy

    // Level
    SDL_Rect levelOuterRect = {boardoffsetX - 190, boardoffsetY + 250, 5.5 * TILE_SIZE, 1.5 * TILE_SIZE};
    SDL_Rect levelInnerRect = {boardoffsetX - 185, boardoffsetY + 255, 5.2 * TILE_SIZE, 1.2 * TILE_SIZE};
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(renderer, &levelOuterRect);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &levelInnerRect);

    // Lines
    SDL_Rect LinesOuterRect = {boardoffsetX - 190, boardoffsetY + 350, 5.5 * TILE_SIZE, 1.5 * TILE_SIZE};
    SDL_Rect LinesInnerRect = {boardoffsetX - 185, boardoffsetY + 355, 5.2 * TILE_SIZE, 1.2 * TILE_SIZE};
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(renderer, &LinesOuterRect);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &LinesInnerRect);

    // Time
    SDL_Rect TimeOuterRect = {boardoffsetX - 190, boardoffsetY + 450, 5.5 * TILE_SIZE, 1.5 * TILE_SIZE};
    SDL_Rect TimeInnerRect = {boardoffsetX - 185, boardoffsetY + 455, 5.2 * TILE_SIZE, 1.2 * TILE_SIZE};
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(renderer, &TimeOuterRect);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &TimeInnerRect);

    // Combo
    SDL_Rect ComboOuterRect = {boardoffsetX - 190, boardoffsetY + 550, 5.5 * TILE_SIZE, 1.5 * TILE_SIZE};
    SDL_Rect ComboInnerRect = {boardoffsetX - 185, boardoffsetY + 555, 5.2 * TILE_SIZE, 1.2 * TILE_SIZE};
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(renderer, &ComboOuterRect);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &ComboInnerRect);

    // Score
    SDL_Rect ScoreOuterRect = {boardoffsetX + 340, boardoffsetY + 450, 5.5 * TILE_SIZE, 1.5 * TILE_SIZE};
    SDL_Rect ScoreInnerRect = {boardoffsetX + 345, boardoffsetY + 455, 5.2 * TILE_SIZE, 1.2 * TILE_SIZE};
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(renderer, &ScoreOuterRect);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &ScoreInnerRect);

    // High Score
    SDL_Rect HScoreOuterRect = {boardoffsetX + 340, boardoffsetY + 550, 5.5 * TILE_SIZE, 1.5 * TILE_SIZE};
    SDL_Rect HScoreInnerRect = {boardoffsetX + 345, boardoffsetY + 555, 5.2 * TILE_SIZE, 1.2 * TILE_SIZE};
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(renderer, &HScoreOuterRect);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &HScoreInnerRect);

    //-----------------------------------------------------------------------------------------------

    // text na hrací ploše
    draw_text(renderer, gGameFont, (SDL_Color){0, 0, 0, 255}, (SDL_Rect){boardoffsetX - 180, boardoffsetY + 200, 0, 0}, "Level");
    draw_text(renderer, gGameFont, (SDL_Color){0, 0, 0, 255}, (SDL_Rect){boardoffsetX - 180, boardoffsetY + 300, 0, 0}, "Lines");
    draw_text(renderer, gGameFont, (SDL_Color){0, 0, 0, 255}, (SDL_Rect){boardoffsetX - 180, boardoffsetY + 400, 0, 0}, "Time");
    draw_text(renderer, gGameFont, (SDL_Color){0, 0, 0, 255}, (SDL_Rect){boardoffsetX + 340, boardoffsetY + 400, 0, 0}, "Score");
    draw_text(renderer, gGameFont, (SDL_Color){0, 0, 0, 255}, (SDL_Rect){boardoffsetX - 180, boardoffsetY + 500, 0, 0}, "Combo");
    draw_text(renderer, gGameFont, (SDL_Color){0, 0, 0, 255}, (SDL_Rect){boardoffsetX + 340, boardoffsetY + 500, 0, 0}, "High Score");

    // vykreslí všechny blocky v hracím poli
    for (int i = 0; i < TetrisGridWidth; i++)
    {
        for (int j = 0; j < TetrisGridHeight; j++)
        {
            int block = hraci_pole[j * TetrisGridWidth + i];
            if (block > 0 && block < 9)
            {
                blocky_barvy_render(renderer, block - 1, boardoffsetX + i * TILE_SIZE, boardoffsetY + j * TILE_SIZE);
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
                    blocky_barvy_render(renderer, CurrentPiece, boardoffsetX + (CurrentX + x) * TILE_SIZE, boardoffsetY + (CurrentY + y) * TILE_SIZE);
                }
            }
        }

        // vykresluje held tetromino
        if (held != -1)
        {
            for (int x = 0; x < 4; x++)
            {
                for (int y = 0; y < 4; y++)
                {
                    int r = Rotate(x, y, 0);

                    if (Tetromino[held][r] == 'X')
                    {

                        blocky_barvy_render(renderer, held, boardoffsetX - 160 + x * TILE_SIZE, boardoffsetY + 50 + y * TILE_SIZE);
                    }
                }
            }
        }
        // vykresluje další 2 tetromino
        for (int x = 0; x < 4; x++)
        {
            for (int y = 0; y < 4; y++)
            {
                int r1 = Rotate(x, y, 0);
                int r2 = Rotate(x, y, 0);
                if (Tetromino[nextPiece1][r1] == 'X')
                {
                    blocky_barvy_render(renderer, nextPiece1, boardoffsetX + 350 + x * TILE_SIZE, boardoffsetY + 50 + y * TILE_SIZE);
                }
                if (Tetromino[nextPiece2][r2] == 'X')
                {
                    blocky_barvy_render(renderer, nextPiece2, boardoffsetX + 350 + x * TILE_SIZE, boardoffsetY + 225 + y * TILE_SIZE);
                }
            }
        }

        // vykresli stin tetromina
        int shadowY = CurrentY;
        while (doesPieceFit(CurrentPiece, CurrentRotation, CurrentX, shadowY + 1))
        {
            shadowY++;
        }
        for (int x = 0; x < 4; x++)
        {
            for (int y = 0; y < 4; y++)
            {
                int r = Rotate(x, y, CurrentRotation);

                if (Tetromino[CurrentPiece][r] == 'X')
                {
                    SDL_SetRenderDrawColor(renderer, 114, 114, 114, 100);
                    SDL_Rect dst = {boardoffsetX + (CurrentX + x) * TILE_SIZE, boardoffsetY + (shadowY + y) * TILE_SIZE, TILE_SIZE, TILE_SIZE};
                    SDL_RenderFillRect(renderer, &dst);
                }
            }
        }

        char textBuffer[64];

        int level = 1;
        level += PieceCount / 10;
        if (level > 20)
        {
            level = 20;
            Speed = 50 - (level * 2);
        }
        // level text
        snprintf(textBuffer, 64, "%d", level);
        draw_text(renderer, gGameFont, (SDL_Color){0, 0, 0, 255}, (SDL_Rect){boardoffsetX - 50, boardoffsetY + 250, 0, 0}, textBuffer);
        // lines text
        snprintf(textBuffer, 64, "%d", lines);
        draw_text(renderer, gGameFont, (SDL_Color){0, 0, 0, 255}, (SDL_Rect){boardoffsetX - 50, boardoffsetY + 350, 0, 0}, textBuffer);
        // time text
        int minutes = sekundy / 60;
        int seconds = sekundy % 60;
        snprintf(textBuffer, 64, "%d:%02d", minutes, seconds);
        draw_text(renderer, gGameFont, (SDL_Color){0, 0, 0, 255}, (SDL_Rect){boardoffsetX - 155, boardoffsetY + 450, 0, 0}, textBuffer);
        // combo text
        snprintf(textBuffer, 64, "%02d", combo);
        draw_text(renderer, gGameFont, (SDL_Color){0, 0, 0, 255}, (SDL_Rect){boardoffsetX - 75, boardoffsetY + 550, 0, 0}, textBuffer);
        // scoretext
        snprintf(textBuffer, 64, "%04d", score);
        draw_text(renderer, gGameFont, (SDL_Color){0, 0, 0, 255}, (SDL_Rect){boardoffsetX + 400, boardoffsetY + 450, 0, 0}, textBuffer);
        // high score text
        snprintf(textBuffer, 64, "%04d", highScore);
        draw_text(renderer, gGameFont, (SDL_Color){0, 0, 0, 255}, (SDL_Rect){boardoffsetX + 400, boardoffsetY + 550, 0, 0}, textBuffer);
    }
}
// exit
void game_quit(void)
{
    if (hraci_pole != NULL)
    {
        free(hraci_pole);
        hraci_pole = NULL;
    }
    if (gGameFont != NULL)
    {
        TTF_CloseFont(gGameFont);
        gGameFont = NULL;
    }
    // uvolnění textur -> aby se zabránilo memory leakům
    if (texI)
    {
        SDL_DestroyTexture(texI);
        texI = NULL;
    }
    if (texT)
    {
        SDL_DestroyTexture(texT);
        texT = NULL;
    }
    if (texO)
    {
        SDL_DestroyTexture(texO);
        texO = NULL;
    }
    if (texS)
    {
        SDL_DestroyTexture(texS);
        texS = NULL;
    }
    if (texZ)
    {
        SDL_DestroyTexture(texZ);
        texZ = NULL;
    }
    if (texL)
    {
        SDL_DestroyTexture(texL);
        texL = NULL;
    }
    if (texJ)
    {
        SDL_DestroyTexture(texJ);
        texJ = NULL;
    }
}