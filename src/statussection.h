#ifndef STATUS_SECTION_H
#define STATUS_SECTION_H

#include "global.h"
#include "LabeledCell.h"
#include "TextLabel.h"

extern LabeledCell* status_header_cells[SHAFTS_COUNT+1];

extern LabeledCell* status_floor_label_cell;
extern LabeledCell* status_direction_label_cell;
extern LabeledCell* status_status_label_cell;

extern LabeledCell* status_car1_floor_cell;
extern LabeledCell* status_car2_floor_cell;
extern LabeledCell* status_car3_floor_cell;

extern LabeledCell* status_car1_direction_cell;
extern LabeledCell* status_car2_direction_cell;
extern LabeledCell* status_car3_direction_cell;

extern LabeledCell* status_car1_status_cell;
extern LabeledCell* status_car2_status_cell;
extern LabeledCell* status_car3_status_cell;

void createStatusHeaderCells();
void createStatusCells();
void drawStatusCells();

#endif