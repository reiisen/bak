#include "SDL3/SDL_events.h"
#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_video.h"
#include <SDL3/SDL.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#define TILE_ROW    8
#define TILE_COL    6
#define TILE_WIDTH  80
#define TILE_HEIGHT 80

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static bool stop = false;
static SDL_Event event;
static SDL_Color color = { 0xFF, 0xFF, 0xFF, 0xFF };
static SDL_FRect tiles[TILE_ROW][TILE_COL];
static SDL_FRect current;

void initWindowAndRenderer() {
    SDL_CreateWindowAndRenderer(
        "title",
        640,
        480,
        SDL_WINDOW_BORDERLESS,
        &window,
        &renderer);
}

void initTileCurrent() {
    for (int i = 0; i < TILE_ROW; i++) {
        for (int j = 0; j < TILE_COL; j++) {
            tiles[i][j].w = TILE_WIDTH;
            tiles[i][j].h = TILE_HEIGHT;
            tiles[i][j].x = tiles[i][j].w * i;
            tiles[i][j].y = tiles[i][j].h * j;
        }
    }

    current.x = 0;
    current.y = 0;
    current.w = TILE_WIDTH;
    current.h = TILE_HEIGHT;
}

void drawTile() {
    SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
    for (int i = 0; i < TILE_ROW; i++) {
        for (int j = 0; j < TILE_COL; j++) {
            SDL_RenderRect(renderer, &tiles[i][j]);
        }
    }
}

void drawCurrent() {
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0x00, 0xFF);
    SDL_RenderFillRect(renderer, &current);
}

void destroyWindowAndRenderer() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
}

void processKeyEvent() {
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_KEY_DOWN) {
            if (event.key.key == 0x00000071u) {
                stop = true;
            } else if (event.key.key == 0x00000077u) {
                current.y -= TILE_HEIGHT;
            } else if (event.key.key == 0x00000061u) {
                current.x -= TILE_WIDTH;
            } else if (event.key.key == 0x00000073u) {
                current.y += TILE_HEIGHT;
            } else if (event.key.key == 0x00000064u) {
                current.x += TILE_WIDTH;
            }
        } else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
            color.r ^= 0xFF;
            color.g ^= 0xFF;
            color.b ^= 0xFF;
        }
    }
}

int main(int argc, char **argv) {
    initWindowAndRenderer();
    initTileCurrent();
    while (!stop) {
        processKeyEvent();
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_RenderClear(renderer);
        drawTile();
        drawCurrent();
        SDL_RenderPresent(renderer);
    }
    destroyWindowAndRenderer();
}
