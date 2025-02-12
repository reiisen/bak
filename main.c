#include "SDL3/SDL_events.h"
#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_video.h"
#include <SDL3/SDL.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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
static SDL_FRect *food;

struct Snake {
    SDL_FRect rects[TILE_ROW * TILE_COL];
    SDL_FRect *head;
    SDL_FRect *tail;
    int count;
};

static struct Snake snake;

void initWindowAndRenderer() {
    SDL_CreateWindowAndRenderer(
        "title",
        640,
        480,
        SDL_WINDOW_BORDERLESS,
        &window,
        &renderer);
}

SDL_FRect *assignRect(SDL_FRect rect, SDL_FRect *new) {
    new->h = rect.h;
    new->w = rect.w;
    new->x = rect.x;
    new->y = rect.y;
    return new;
}

void initTile() {
    for (int i = 0; i < TILE_ROW; i++) {
        for (int j = 0; j < TILE_COL; j++) {
            tiles[i][j].w = TILE_WIDTH;
            tiles[i][j].h = TILE_HEIGHT;
            tiles[i][j].x = tiles[i][j].w * i;
            tiles[i][j].y = tiles[i][j].h * j;
        }
    }
}

void drawTile() {
    SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
    for (int i = 0; i < TILE_ROW; i++) {
        for (int j = 0; j < TILE_COL; j++) {
            SDL_RenderRect(renderer, &tiles[i][j]);
        }
    }
}

void initSnake() {
    srand(time(NULL));
    snake.rects[0].x = (rand() % 8) * TILE_WIDTH;
    snake.rects[0].y = (rand() % 6) * TILE_HEIGHT;
    snake.rects[0].w = TILE_WIDTH;
    snake.rects[0].h = TILE_HEIGHT;
    snake.count = 1;
    snake.head = &snake.rects[0];
    snake.tail = NULL;
}

void drawSnake() {
    SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0x00, 0xFF);
    SDL_RenderFillRects(renderer, snake.rects, snake.count);
}

void destroyWindowAndRenderer() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
}

void moveDebug(SDL_FRect dest) {
    printf("%i%s", snake.count, "\n");
    printf("Destination x: %f y: %f\n", dest.x, dest.y);
    int i = snake.count - 1;
    while (i > 0) {
        printf("Before assign\n");
        printf("%f%s", snake.tail->y, "\n");
        printf("%f%s", snake.head->y, "\n");
        assignRect(snake.rects[i - 1], &snake.rects[i]);
        i--;
        printf("After assign\n");
        printf("%f%s", snake.tail->y, "\n");
        printf("%f%s", snake.head->y, "\n");
    }
    printf("After assign HEAD\n");
    assignRect(dest, snake.head);
    printf("%f%s", snake.tail->y, "\n");
    printf("%f%s", snake.head->y, "\n");
}

void move(SDL_FRect dest) {
    int i = snake.count - 1;
    while (i > 0) {
        assignRect(snake.rects[i - 1], &snake.rects[i]);
        i--;
    }
    assignRect(dest, snake.head);
}

void grow(SDL_FRect food) {
    SDL_FRect newTail;
    int i = snake.count - 1;
    while (i > 0) {
        assignRect(snake.rects[i - 1], &snake.rects[i]);
        i--;
    }
    snake.count += 1;
    assignRect(snake.rects[snake.count - 1], &newTail);
    assignRect(food, snake.head);
}

void moveUp() {
    if (snake.tail == NULL) {
        SDL_FRect temp = { snake.head->x, snake.head->y, snake.head->w, snake.head->h };
        snake.head->y -= TILE_HEIGHT;
        snake.tail = assignRect(temp, &snake.rects[snake.count]);
        snake.count += 1;
    } else {
        SDL_FRect dest = { snake.head->x, snake.head->y, snake.head->w, snake.head->h };
        dest.y -= TILE_HEIGHT;
        if (SDL_HasRectIntersectionFloat(&dest, food)) {
            grow(dest);
        } else {
            move(dest);
        }
    }
}

void moveLeft() {
    if (snake.tail == NULL) {
        SDL_FRect temp = { snake.head->x, snake.head->y, snake.head->w, snake.head->h };
        snake.head->x -= TILE_WIDTH;
        snake.tail = assignRect(temp, &snake.rects[snake.count]);
        snake.count += 1;
    } else {
        SDL_FRect dest = { snake.head->x, snake.head->y, snake.head->w, snake.head->h };
        dest.x -= TILE_WIDTH;
        if (SDL_HasRectIntersectionFloat(&dest, food)) {
            grow(dest);
        } else {
            move(dest);
        }
    }
}

void moveDown() {
    if (snake.tail == NULL) {
        SDL_FRect temp = { snake.head->x, snake.head->y, snake.head->w, snake.head->h };
        snake.head->y += TILE_HEIGHT;
        snake.tail = assignRect(temp, &snake.rects[snake.count]);
        snake.count += 1;
    } else {
        SDL_FRect dest = { snake.head->x, snake.head->y, snake.head->w, snake.head->h };
        dest.y += TILE_HEIGHT;
        if (SDL_HasRectIntersectionFloat(&dest, food)) {
            grow(dest);
        } else {
            move(dest);
        }
    }
}

void moveRight() {
    if (snake.tail == NULL) {
        SDL_FRect temp = { snake.head->x, snake.head->y, snake.head->w, snake.head->h };
        snake.head->x += TILE_WIDTH;
        snake.tail = assignRect(temp, &snake.rects[snake.count]);
        snake.count += 1;
    } else {
        SDL_FRect dest = { snake.head->x, snake.head->y, snake.head->w, snake.head->h };
        dest.x += TILE_WIDTH;
        if (SDL_HasRectIntersectionFloat(&dest, food)) {
            grow(dest);
        } else {
            move(dest);
        }
    }
}

void initFood() {
    srand(time(NULL));
    SDL_FRect *target = &tiles[(rand() * 7) % 8][(rand() * 13) % 6];
    static SDL_FRect foodRect;
    foodRect.x = target->x + 20;
    foodRect.y = target->y + 20;
    foodRect.w = target->w - 40;
    foodRect.h = target->h - 40;
    food = &foodRect;
}

void drawFood() {
    SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0xFF, 0xFF);
    SDL_RenderFillRect(renderer, food);
}

void processKeyEvent() {
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_KEY_DOWN) {
            if (event.key.key == 0x00000071u) {
                stop = true;
            } else if (event.key.key == 0x00000077u) {
                moveUp();
            } else if (event.key.key == 0x00000061u) {
                moveLeft();
            } else if (event.key.key == 0x00000073u) {
                moveDown();
            } else if (event.key.key == 0x00000064u) {
                moveRight();
            }
        } else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
            color.r ^= 0xFF;
            color.g ^= 0xFF;
            color.b ^= 0xFF;
        }
    }
}

void printStats() {
    // printf("Snake Head X: %f\n", snake.head->x);
    // printf("Snake Head Y: %f\n", snake.head->y);
    // printf("Count: %i\n", snake.count);
    printf("Food X: %f\n", food->x);
    printf("Food Y: %f\n", food->y);
}

int main(int argc, char **argv) {
    initWindowAndRenderer();
    initTile();
    initSnake();
    initFood();
    while (!stop) {
        processKeyEvent();
        printStats();
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_RenderClear(renderer);
        drawTile();
        drawFood();
        drawSnake();
        SDL_RenderPresent(renderer);
    }
    destroyWindowAndRenderer();
}
