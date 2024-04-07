#include <SDL2/SDL.h>
#include "global.h"
#include "Rect.h"
#include "LabeledCell.h"

LabeledCell* newLabeledCell(int x, int y, int width, int height, char *text, char *font_file, int font_size) {
    LabeledCell *cell = malloc(sizeof(LabeledCell));
    cell->rect = newRect(x, y, width, height);
    cell->frame_rect = newRect(x, y, width, height);
    cell->text_struct = newTextLabel(text, 15, primary_font_file, &color_white, x, y, cell->rect);
    return cell;
}

void setCenteredPositionInCell(int *x, int *y, int text_width, int text_height, SDL_Rect *rect) {
   *x = (int)(rect->x+(rect->w/2)-(text_width/2));
   *y = (int)(rect->y+(rect->h/2)-(text_height/2) -1); // - TEXT_Y_POS_OFFSET;
}

void drawLabeledCell(LabeledCell *cell) {
   // Cell
   SDL_SetRenderDrawColor(renderer, color_gray.r, color_gray.g, color_gray.b, color_gray.a);
   SDL_RenderFillRect(renderer, cell->rect);
   // Cell frame
   SDL_SetRenderDrawColor(renderer, color_light_gray.r, color_light_gray.g, color_light_gray.b, color_light_gray.a);
   SDL_RenderDrawRect(renderer, cell->frame_rect);
   // Text
   SDL_SetRenderDrawColor(renderer, color_white.r, color_white.g, color_white.b, color_white.a);
   SDL_RenderCopy(renderer, cell->text_struct->texture, NULL, cell->text_struct->rect);
}

void deleteLabeledCell(LabeledCell *cell) {
   deleteRect(cell->rect);
   deleteRect(cell->frame_rect);
   deleteTextLabel(cell->text_struct);
   free(cell);
   cell = NULL;
}
