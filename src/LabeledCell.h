#ifndef LABELED_CELL_H
#define LABELED_CELL_H

#include <SDL2/SDL.h>
#include "TextLabel.h"

typedef struct LabeledCell {
   TextLabel *text_struct;
   SDL_Rect *rect;
   SDL_Rect *frame_rect;
} LabeledCell;

LabeledCell* newLabeledCell(int x, int y, int width, int height, char *text, char *font_file, int font_size);
void setCenteredPositionInCell(int *x, int *y, int text_width, int text_height, SDL_Rect *rect);
void drawLabeledCell(LabeledCell *cell);
void deleteLabeledCell(LabeledCell *cell);

#endif