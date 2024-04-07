#include <SDL2/SDL_ttf.h>
#include "global.h"
#include "Rect.h"
#include "LabeledCell.h"
#include "TextLabel.h"

TextLabel* newTextLabel(char *text, int font_size, char *font_file, SDL_Color *color, int text_x, int text_y, SDL_Rect *container_rect) {
   TextLabel *text_struct = malloc(sizeof(TextLabel));
   text_struct->direction = 1;
   text_struct->font_size = font_size;
   text_struct->font_file = font_file;
   int text_width, text_height;
   set_text_size(&text_width, &text_height, text, font_size);
   if (container_rect != NULL) {
      setCenteredPositionInCell(&text_x, &text_y, text_width, text_height, container_rect);
   }
   setTextFromTextLabel(text_struct, text, text_x, text_y, text_width, text_height, color);
   return text_struct;
}

void setTextFromTextLabel(TextLabel *text_struct, char* text, int x, int y, int text_width, int text_height, SDL_Color *color) {
   TTF_Font *font = TTF_OpenFont(text_struct->font_file, text_struct->font_size);
   SDL_Surface *surface = TTF_RenderText_Blended(font, text, *color);
   SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
   SDL_Rect *rect = malloc(sizeof(SDL_Rect));
   rect->x = x;
   rect->y = y;
   rect->w = text_width;
   rect->h = text_height;
   text_struct->rect = rect;
   text_struct->texture = texture;
   text_struct->text = text;
   SDL_FreeSurface(surface);
   TTF_CloseFont(font);
}

void updateTextFromTextLabel(TextLabel *text_struct, char *text, SDL_Color *color) {
   TTF_Font *font = TTF_OpenFont(text_struct->font_file, text_struct->font_size);
   SDL_Surface *surface = TTF_RenderText_Blended(font, text, *color);
   SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
   SDL_DestroyTexture(text_struct->texture);
   text_struct->texture = texture;
   text_struct->text = text;
   set_text_size(&text_struct->rect->w, &text_struct->rect->h, text, text_struct->font_size);
   SDL_FreeSurface(surface);
   TTF_CloseFont(font);
}

void set_text_size(int *text_width, int *text_height, char *text, int font_size) {
   *text_height = font_size+10;
   *text_width = strlen(text)*(*text_height/2); // Correct way to determine font dimensions.
}

void deleteTextLabel(TextLabel* textLabel) {
    deleteRect(textLabel->rect);
    SDL_DestroyTexture(textLabel->texture);
    free(textLabel);
    textLabel = NULL;
}
