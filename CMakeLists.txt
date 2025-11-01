cmake_minimum_required(VERSION 3.10)
project(Tetris C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

# Nastaví varování kompilátoru (můžeš nechat)
if (MSVC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W4")
else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -pedantic")
endif()

# --- ZDE ZAČÍNÁ SPRÁVA ZÁVISLOSTÍ ---

# 1. NAJDI KNIHOVNY
# Řekneš CMake, aby našel SDL2 a SDL2_image, které jsi nainstaloval
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)

# --- KONEC HLEDÁNÍ ---

# Vytvoří tvůj program "Tetris" ze souboru "main.c"
add_executable(Tetris main.c)

# --- ZDE PŘIDÁVÁŠ ZÁVISLOSTI K PROGRAMU ---

# 2. PŘILINKUJ KNIHOVNY
# Toto je ten nejdůležitější příkaz.
# Spojí tvůj "Tetris" s knihovnami SDL2 a SDL2_image.
# CMake se automaticky postará i o cesty k .h souborům (include).
target_link_libraries(Tetris PRIVATE 
    SDL2::SDL2
    SDL2_image::SDL2_image
)

# --- KONEC LINKOVÁNÍ ---

# Zkopíruje tvé obrázky do složky build/ (pro spuštění)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/assets)