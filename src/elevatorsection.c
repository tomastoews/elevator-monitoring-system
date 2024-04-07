#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL.h>
#include "Point.h"
#include "utils.h"
#include "LabeledCell.h"
#include "statussection.h"
#include "elevatorsection.h"

int floor_display_index = TOTAL_FLOORS_COUNT;
int car_movement_animation_trigger_frames = 5000;
int car_movement_animation_passed_frames = 0;

SDL_Rect shafts[TOTAL_FLOORS_COUNT][SHAFTS_COUNT];
LabeledCell* floor_description_cells[TOTAL_FLOORS_COUNT];
LabeledCell* elevator_header_cells[SHAFTS_COUNT+1];

void createElevatorCells() {
   int full_width = ELEVATORS_FULL_WIDTH;
   int shafts_x = 60 + (full_width * 0.25);
   int start_x = ELEVATORS_MARGIN_X;
   int start_y = ELEVATORS_MARGIN_Y;

   // Create elevator header cells
   for (int i = 0; i < SHAFTS_COUNT+1; i++) {
      int cell_x = start_x+(CELL_WIDTH*i);
      int cell_y = start_y; 
      char *text = malloc(5);
      memset(text, 0, 5);
      if (i == 0) {
         text = "CAR #";
      }
      else {
         char *num_char = intToString(i);
         strcat(text, "CAR ");
         strcat(text, num_char);
         free(num_char);
      }
      elevator_header_cells[i] = newLabeledCell(cell_x, cell_y, CELL_WIDTH, CELL_HEIGHT, text, primary_font_file, 15);
   }

   // Create floor description cells
   for (int i = 0; i <= TOTAL_FLOORS_COUNT; i++) {
      int cell_x = start_x;
      int cell_y = start_y+CELL_HEIGHT+CELL_GAP+(CELL_HEIGHT*i);
      char *text = malloc(8);
      char *num_char = intToString(i);
      memset(text, 0, 8);
      strcat(text, "Floor ");
      strcat(text, num_char);
      free(num_char);
      floor_description_cells[i] = newLabeledCell(cell_x, cell_y, CELL_WIDTH, CELL_HEIGHT, text, primary_font_file, 15);
   }
}

void moveCar(Car *car, LabeledCell *status_car_floor_cell) {
   int n = atoi(status_car_floor_cell->text_struct->text);
   if (car->movementDirection == UP && n < TOTAL_FLOORS_COUNT) {
      n++;
   } else if (car->movementDirection == DOWN && n > 1) {
      n--;
   }
   updateTextFromTextLabel(status_car_floor_cell->text_struct, intToString(n), &color_white); 
}

void updateCarAnimations() {
   if (car_movement_animation_passed_frames >= car_movement_animation_trigger_frames) {
      moveCar(&car1, status_car1_floor_cell);
      moveCar(&car2, status_car2_floor_cell);
      moveCar(&car3, status_car3_floor_cell);
      car_movement_animation_passed_frames = 0;
   }
   car_movement_animation_passed_frames++;
}


void increaseFloorDisplayIndex() {
   if (floor_display_index+1 <= TOTAL_FLOORS_COUNT) {
      floor_display_index++;
   }
}

void decreaseFloorDisplayIndex() {
   if ((floor_display_index-VISIBLE_FLOORS_COUNT) > 0) {
      floor_display_index--;
   }
}

void drawElevatorCells() {
   int full_width = (int)WIDTH*0.40;
   int start_x = ELEVATORS_MARGIN_X;
   int start_y = ELEVATORS_MARGIN_Y;

   // Draw elevator headers cells
   for (int i = 0; i < SHAFTS_COUNT+1; i++) {
      drawLabeledCell(elevator_header_cells[i]);
   }

   // Draw floor description cells
   int iteration_counter = 0;
   for (int i = floor_display_index; i > (floor_display_index-VISIBLE_FLOORS_COUNT); i--) {

      // Draw floor description cell
      // -> All positions have to be recalculated first
      LabeledCell *cell_ptr = floor_description_cells[i];
      int pos_x = start_x;
      int pos_y = start_y+CELL_HEIGHT+CELL_GAP+(CELL_HEIGHT*iteration_counter);
      cell_ptr->rect->y = pos_y;
      cell_ptr->frame_rect->y = pos_y;
      setCenteredPositionInCell(
         &cell_ptr->text_struct->rect->x, &cell_ptr->text_struct->rect->y,
         cell_ptr->text_struct->rect->w, cell_ptr->text_struct->rect->h, 
         cell_ptr->rect
      );
      drawLabeledCell(cell_ptr);
      
      // Draw shaft cells that are on this floor
      for (int x = 0; x < SHAFTS_COUNT; x++) {
         SDL_Rect shaft_section;
         shaft_section.x = start_x+CELL_WIDTH+(CELL_WIDTH*x);
         shaft_section.y = (start_y+CELL_HEIGHT+CELL_GAP)+(CELL_HEIGHT*iteration_counter);
         shaft_section.w = CELL_WIDTH;
         shaft_section.h = CELL_HEIGHT;
         SDL_SetRenderDrawColor(renderer, color_gray.r+100, color_gray.g+100, color_gray.b+100, 0);
         SDL_RenderDrawRect(renderer, &shaft_section);

         int car_floor;
         CarMovementDirection car_direction;

         if (x == 0) {
            car_floor = atoi(status_car1_floor_cell->text_struct->text);
            car_direction = car1.movementDirection;
         }
         else if (x == 1) {
            car_floor = atoi(status_car2_floor_cell->text_struct->text);
            car_direction = car2.movementDirection;
         }
         else if (x == 2) {
            car_floor = atoi(status_car3_floor_cell->text_struct->text);
            car_direction = car3.movementDirection;
         }

         // Check if the car of this shaft is on the current (x iteation) floor
         if (car_floor == i) {
            // Draw triangle
            int lines_count = CELL_HEIGHT-20;
            int triangle_width = CELL_WIDTH-20;
            int triangle_height = CELL_HEIGHT-20;
            int triangle_x = shaft_section.x+10;
            int triangle_y = shaft_section.y+10;
            Point line1 = { 0, 0, 0, 0 };
            Point line2 = { 0, 0, 0, 0 };
            Point line3 = { 0, 0, 0, 0 };

            if (car_direction == DOWN) {
               line1.x1 = triangle_x;
               line1.y1 = triangle_y;
               line1.x2 = triangle_x+triangle_width;
               line1.y2 = triangle_y;
               line2.x1 = triangle_x;
               line2.y1 = triangle_y;
               line2.x2 = triangle_x+triangle_width/2;
               line2.y2 = triangle_y+triangle_height;
               line3.x1 = triangle_x+triangle_width;
               line3.y1 = triangle_y;
               line3.x2 = triangle_x+triangle_width/2;
               line3.y2 = triangle_y+triangle_height;
            } else {
               line1.x1 = triangle_x;
               line1.y1 = triangle_y+triangle_height;
               line1.x2 = triangle_x+triangle_width;
               line1.y2 = triangle_y+triangle_height;
               line2.x1 = triangle_x;
               line2.y1 = triangle_y+triangle_height;
               line2.x2 = triangle_x+triangle_width/2;
               line2.y2 = triangle_y;
               line3.x1 = triangle_x+triangle_width;
               line3.y1 = triangle_y+triangle_height;
               line3.x2 = triangle_x+triangle_width/2;
               line3.y2 = triangle_y;
            }
            SDL_SetRenderDrawColor(renderer, color_white.r, color_white.g, color_white.b, 0);
            SDL_RenderDrawLine(renderer, line1.x1, line1.y1, line1.x2, line1.y2);
            SDL_RenderDrawLine(renderer, line2.x1, line2.y1, line2.x2, line2.y2);
            SDL_RenderDrawLine(renderer, line3.x1, line3.y1, line3.x2, line3.y2); 
         }
      }
      iteration_counter++;
   } 
}
