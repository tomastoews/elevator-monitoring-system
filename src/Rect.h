#ifndef RECT_H
#define RECT_H

#include <SDL2/SDL.h>

SDL_Rect* newRect(int x, int y, int w, int h);
void deleteRect(SDL_Rect* rect);

#endif