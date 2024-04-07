#include "Rect.h"

SDL_Rect* newRect(int x, int y, int w, int h) {
    SDL_Rect *rect = malloc(sizeof(SDL_Rect));
    rect->x = x;
    rect->y = y;
    rect->w = w;
    rect->h = h;
    return rect;
}

void deleteRect(SDL_Rect* rect) {
    free(rect);
    rect = NULL;
}
