#include <SDL2/SDL.h>
#include "statussection.h"
#include "TextLabel.h"
#include "infosection.h"

TextLabel* general_texts[GENERAL_TEXTS_COUNT];

void createInfoTexts() {
   int help_x = status_status_label_cell->rect->x;
   int help_y = status_status_label_cell->rect->y+(CELL_HEIGHT*2);

   // Title text
   general_texts[0] = newTextLabel("The Tower", 60, title_font_file, &color_white, help_x, ELEVATORS_MARGIN_Y, NULL);

   // Sub header text
   general_texts[1] = newTextLabel("ELEVATOR MONITORING SYSTEM", 10, primary_font_file, &color_white, help_x, ELEVATORS_MARGIN_Y+90, NULL);

   // Help texts
   general_texts[2] = newTextLabel("KEY FUNCTIONS", 15, primary_font_file, &color_light_gray, help_x, help_y, NULL);
   general_texts[3] = newTextLabel("SCROLL FLOORS : UP/DOWN ARROWS, SCROLL WHEEL", 15, primary_font_file, &color_light_gray, help_x, help_y+20+30, NULL);
   general_texts[4] = newTextLabel("CAR 1 DIRECTION UP : NUMPAD 7", 15, primary_font_file, &color_light_gray, help_x,  help_y+20+60, NULL);
   general_texts[5] = newTextLabel("CAR 1 DIRECTION DOWN : NUMPAD 4", 15, primary_font_file, &color_light_gray, help_x, help_y+20+90, NULL);
   general_texts[6] = newTextLabel("CAR 2 DIRECTION UP : NUMPAD 8", 15, primary_font_file, &color_light_gray, help_x, help_y+20+120, NULL);
   general_texts[7] = newTextLabel("CAR 2 DIRECTION DOWN : NUMPAD 5", 15, primary_font_file, &color_light_gray, help_x, help_y+20+150, NULL);
   general_texts[8] = newTextLabel("CAR 3 DIRECTION UP : NUMPAD 9", 15, primary_font_file, &color_light_gray, help_x, help_y+20+180, NULL);
   general_texts[9] = newTextLabel("CAR 3 DIRECTION DOWN : NUMPAD 6", 15, primary_font_file, &color_light_gray, help_x, help_y+20+210, NULL);
}

void drawInfoTexts() {
   for (int i = 0; i < GENERAL_TEXTS_COUNT; i++) {
      SDL_RenderCopy(renderer, general_texts[i]->texture, NULL, general_texts[i]->rect);
   }
}
