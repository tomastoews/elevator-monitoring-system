#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdbool.h>
#include <SDL2/SDL.h>

#define WIDTH 1280
#define HEIGHT 1024
#define SHAFTS_COUNT 3
#define VISIBLE_FLOORS_COUNT 17
#define TOTAL_FLOORS_COUNT 50 // 51 (Plus penthouse)
#define ELEVATORS_FULL_WIDTH (int)WIDTH*0.40
#define ELEVATORS_MARGIN_X 60
#define ELEVATORS_MARGIN_Y 50
#define TEXT_Y_POS_OFFSET 7
#define CELL_WIDTH ELEVATORS_FULL_WIDTH * 0.25
#define CELL_HEIGHT 50
#define CELL_GAP 20
#define GENERAL_TEXTS_COUNT 10

extern bool running;
extern SDL_Window *window;
extern SDL_Renderer *renderer;

extern char *primary_font_file;
extern char *secondary_font_file;
extern char *title_font_file;

extern SDL_Color color_gray;
extern SDL_Color color_light_gray;
extern SDL_Color color_white;

#endif