cmake_minimum_required(VERSION 3.10)

project(sejarah-rpg LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Opsi debug symbols biar gampang debug
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Cari library SDL
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2)
pkg_check_modules(SDL2_IMAGE REQUIRED SDL2_image)
pkg_check_modules(SDL2_TTF REQUIRED SDL2_ttf)
pkg_check_modules(SDL2_MIXER REQUIRED SDL2_mixer)

include_directories(
    ${SDL2_INCLUDE_DIRS}
    ${SDL2_IMAGE_INCLUDE_DIRS}
    ${SDL2_TTF_INCLUDE_DIRS}
    ${SDL2_MIXER_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

add_executable(sejarah-rpg
    src/main.cpp
    src/Game.cpp
    src/SceneManager.cpp
    src/MenuScene.cpp
    src/BadarIntroScene.cpp
    src/DuelScene.cpp
    src/EndingScene.cpp
    src/TextRenderer.cpp
    src/DialogueBox.cpp
    src/CharacterSelectScene.cpp
)

target_link_libraries(sejarah-rpg
    ${SDL2_LIBRARIES}
    ${SDL2_IMAGE_LIBRARIES}
    ${SDL2_TTF_LIBRARIES}
    ${SDL2_MIXER_LIBRARIES}
)
