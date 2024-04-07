#include <SDL2/SDL.h>
#include "elevatorsection.h"
#include "statussection.h"
#include "global.h"
#include "Car.h"
#include "events.h"

void handleEvents(SDL_Event *event) {
   if (event->type == SDL_QUIT) {
      running = false;
   }
   if (event->type == SDL_KEYDOWN) {
      switch (event->key.keysym.sym) {
         case SDLK_UP:
            increaseFloorDisplayIndex();
            break;
         case SDLK_DOWN:
            decreaseFloorDisplayIndex();
            break;	
         case SDLK_KP_7:
            updateTextFromTextLabel(status_car1_direction_cell->text_struct, "UP", &color_white);
            car1.movementDirection = UP;
            break;
         case SDLK_KP_4:
            updateTextFromTextLabel(status_car1_direction_cell->text_struct, "DOWN", &color_white);
            car1.movementDirection = DOWN;
            break;
         case SDLK_KP_8:
            updateTextFromTextLabel(status_car2_direction_cell->text_struct, "UP", &color_white);
            car2.movementDirection = UP;
            break;
         case SDLK_KP_5:
            updateTextFromTextLabel(status_car2_direction_cell->text_struct, "DOWN", &color_white);
            car2.movementDirection = DOWN;
            break;
         case SDLK_KP_9:
            updateTextFromTextLabel(status_car3_direction_cell->text_struct, "UP", &color_white);
            car3.movementDirection = UP;
            break;
         case SDLK_KP_6:
            updateTextFromTextLabel(status_car3_direction_cell->text_struct, "DOWN", &color_white);
            car3.movementDirection = DOWN;
	         break;
      }
   } 
   else if(event->type == SDL_MOUSEWHEEL) {
      int start_x = ELEVATORS_MARGIN_X;
      int start_y = ELEVATORS_MARGIN_Y+CELL_HEIGHT+CELL_GAP;
      int end_x = ELEVATORS_MARGIN_X + ((SHAFTS_COUNT+1) * CELL_WIDTH);
      int end_y = ELEVATORS_MARGIN_Y + ((VISIBLE_FLOORS_COUNT+1) * CELL_HEIGHT);
      int mouseX, mouseY;
      SDL_GetMouseState(&mouseX, &mouseY);
      if ((mouseX > start_x && mouseX < end_x) && (mouseY > start_y && mouseY < end_y)) {
         int scrollDirection = event->key.keysym.sym;
         if (scrollDirection == 1) {
            increaseFloorDisplayIndex();
         } else {
            decreaseFloorDisplayIndex();
         }
      }
   }
}
