#ifndef TEXT_H
#define TEXT_H

#include <SDL2/SDL.h>

typedef struct TextLabel {
   SDL_Texture *texture;
   SDL_Rect *rect;
   int direction;
   char *text;
   int font_size;
   char *font_file;
} TextLabel;

TextLabel* newTextLabel(char *text, int font_size, char *font_file, SDL_Color *color, int text_x, int text_y, SDL_Rect *container_rect);
static void setTextFromTextLabel(TextLabel *text_struct, char* text, int x, int y, int text_width, int text_height, SDL_Color *color);
void updateTextFromTextLabel(TextLabel *text_struct, char *text, SDL_Color *color);
void set_text_size(int *text_width, int *text_height, char *text, int font_size);
void deleteTextLabel(TextLabel* textLabel);

#endif