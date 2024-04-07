#ifndef ELEVATOR_SECTION_H
#define ELEVATOR_SECTION_H

#include <stdbool.h>
#include "global.h"
#include "Car.h"
#include "LabeledCell.h"

extern int floor_display_index;
extern int car_movement_animation_trigger_frames;
extern int car_movement_animation_passed_frames;

extern SDL_Rect shafts[TOTAL_FLOORS_COUNT][SHAFTS_COUNT];
extern LabeledCell* floor_description_cells[TOTAL_FLOORS_COUNT];
extern LabeledCell* elevator_header_cells[SHAFTS_COUNT+1];

void createElevatorCells();
void moveCar(Car *car, LabeledCell *status_car_floor_cell);
void updateCarAnimations();
void increaseFloorDisplayIndex();
void decreaseFloorDisplayIndex();
void drawElevatorCells();

#endif