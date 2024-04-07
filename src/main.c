#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "global.h"
#include "utils.h"
#include "events.h"
#include "elevatorsection.h"
#include "statussection.h"
#include "infosection.h"
#include "LabeledCell.h"

void init() {
   SDL_Init(SDL_INIT_VIDEO);
   TTF_Init();
   SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, 0, &window, &renderer);
   SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
   SDL_RenderClear(renderer);
   SDL_RenderPresent(renderer);
}

void cleanup() { 
   for (int i = 0; i < SHAFTS_COUNT+1; i++) {
      deleteLabeledCell(elevator_header_cells[i]);
   }
   for (int i = 0; i <= TOTAL_FLOORS_COUNT; i++) {
      deleteLabeledCell(floor_description_cells[i]);
   }
   for (int i = 0; i < SHAFTS_COUNT+1; i++) {
      deleteLabeledCell(status_header_cells[i]);
   }
   for (int i = 0; i < GENERAL_TEXTS_COUNT; i++) {
      deleteTextLabel(general_texts[i]);
   }
   deleteLabeledCell(status_floor_label_cell);
   deleteLabeledCell(status_direction_label_cell);
   deleteLabeledCell(status_status_label_cell);

   deleteLabeledCell(status_car1_floor_cell);
   deleteLabeledCell(status_car2_floor_cell);
   deleteLabeledCell(status_car3_floor_cell);

   deleteLabeledCell(status_car1_direction_cell);
   deleteLabeledCell(status_car2_direction_cell);
   deleteLabeledCell(status_car3_direction_cell);

   deleteLabeledCell(status_car1_status_cell);
   deleteLabeledCell(status_car2_status_cell);
   deleteLabeledCell(status_car3_status_cell);

   SDL_DestroyRenderer(renderer);
   SDL_DestroyWindow(window);
   SDL_Quit();
}

int main() {
   init();
   createElevatorCells();
   createStatusHeaderCells();
   createStatusCells();
   createInfoTexts();

   while(running) {
      SDL_Event event;
      while (SDL_PollEvent(&event)) {
         handleEvents(&event);
      }
      SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
      SDL_RenderClear(renderer);
      drawElevatorCells();
      drawStatusCells();
      drawInfoTexts();
      updateCarAnimations();
      SDL_RenderPresent(renderer);
   }

   cleanup();
   return 0;
}
