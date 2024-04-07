#include "global.h"

bool running = true;
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

char *primary_font_file = "fonts/Waukegan_LDO_Bold.TTF";
char *secondary_font_file = "fonts/WagnerModern.TTF";
char *title_font_file = "fonts/Beautiful_Marry.TTF";

SDL_Color color_gray = { 53, 70, 79, 255 };
SDL_Color color_light_gray = { 53+50, 70+50, 79+50, 255 };
SDL_Color color_white = { 255, 255, 255, 255 };
