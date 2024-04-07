#include "global.h"
#include "LabeledCell.h"
#include "TextLabel.h"
#include "utils.h"
#include "statussection.h"

LabeledCell* status_header_cells[SHAFTS_COUNT+1];

LabeledCell* status_floor_label_cell = NULL;
LabeledCell* status_direction_label_cell = NULL;
LabeledCell* status_status_label_cell = NULL;

LabeledCell* status_car1_floor_cell = NULL;
LabeledCell* status_car2_floor_cell = NULL;
LabeledCell* status_car3_floor_cell = NULL;

LabeledCell* status_car1_direction_cell = NULL;
LabeledCell* status_car2_direction_cell = NULL;
LabeledCell* status_car3_direction_cell = NULL;

LabeledCell* status_car1_status_cell = NULL;
LabeledCell* status_car2_status_cell = NULL;
LabeledCell* status_car3_status_cell = NULL;

void createStatusHeaderCells() {
   int start_x = WIDTH-(ELEVATORS_MARGIN_X+(CELL_WIDTH*4));
   int start_y = ELEVATORS_MARGIN_Y+(3*CELL_HEIGHT);
   for (int i = 0; i < SHAFTS_COUNT+1; i++) {
      int cell_x = start_x+(CELL_WIDTH*i);
      int cell_y = start_y;
      int size = (i == 0) ? 5 : 1;
      char* text = NULL;
      if (i == 0) {
         text = "CAR #";
      }
      else {
         text = intToString(i);
      }
      status_header_cells[i] = newLabeledCell(cell_x, cell_y, CELL_WIDTH, CELL_HEIGHT, text, primary_font_file, 15);
   } 
}

void createStatusCells() {
   int start_x = WIDTH-(ELEVATORS_MARGIN_X+(CELL_WIDTH*4));
   int start_y = status_header_cells[0]->rect->y+CELL_HEIGHT+CELL_GAP;
   
   // Create status floor label cells
   status_floor_label_cell = newLabeledCell(start_x, start_y, CELL_WIDTH, CELL_HEIGHT, "FLOOR", primary_font_file, 15);
   status_direction_label_cell = newLabeledCell(start_x, start_y+(CELL_HEIGHT), CELL_WIDTH, CELL_HEIGHT, "DIRECTION", primary_font_file, 15);
   status_status_label_cell = newLabeledCell(start_x, start_y+(CELL_HEIGHT*2), CELL_WIDTH, CELL_HEIGHT, "STATUS", primary_font_file, 15);

   // Create car floor status cells
   status_car1_floor_cell = newLabeledCell(start_x+CELL_WIDTH, start_y, CELL_WIDTH, CELL_HEIGHT, "1", primary_font_file, 15);
   status_car2_floor_cell = newLabeledCell(start_x+(CELL_WIDTH*2), start_y, CELL_WIDTH, CELL_HEIGHT, "1",  primary_font_file,15);
   status_car3_floor_cell = newLabeledCell(start_x+(CELL_WIDTH*3), start_y, CELL_WIDTH, CELL_HEIGHT, "1", primary_font_file, 15);

   // Create car direction status cells
   status_car1_direction_cell = newLabeledCell(start_x+CELL_WIDTH, start_y+CELL_HEIGHT, CELL_WIDTH, CELL_HEIGHT, "UP", primary_font_file, 15);
   status_car2_direction_cell = newLabeledCell(start_x+(CELL_WIDTH*2), start_y+CELL_HEIGHT, CELL_WIDTH, CELL_HEIGHT, "UP", primary_font_file, 15);
   status_car3_direction_cell = newLabeledCell(start_x+(CELL_WIDTH*3), start_y+CELL_HEIGHT, CELL_WIDTH, CELL_HEIGHT, "UP", primary_font_file, 15);

   // Create car status cells
   status_car1_status_cell = newLabeledCell(start_x+CELL_WIDTH, start_y+(CELL_HEIGHT*2), CELL_WIDTH, CELL_HEIGHT, "AUTOMATIC", primary_font_file, 15);
   status_car2_status_cell = newLabeledCell(start_x+(CELL_WIDTH*2), start_y+(CELL_HEIGHT*2), CELL_WIDTH, CELL_HEIGHT, "AUTOMATIC", primary_font_file, 15);
   status_car3_status_cell = newLabeledCell(start_x+(CELL_WIDTH*3), start_y+(CELL_HEIGHT*2), CELL_WIDTH, CELL_HEIGHT, "AUTOMATIC", primary_font_file, 15);
}

void drawStatusCells() {
   // Draw status header cells
   for (int i = 0; i < SHAFTS_COUNT+1; i++) {
      drawLabeledCell(status_header_cells[i]);   
   }

   // Draw row labels
   drawLabeledCell(status_floor_label_cell);
   drawLabeledCell(status_direction_label_cell);
   drawLabeledCell(status_status_label_cell);

   // Draw status cells
   drawLabeledCell(status_car1_floor_cell);
   drawLabeledCell(status_car2_floor_cell);
   drawLabeledCell(status_car3_floor_cell);

   drawLabeledCell(status_car1_direction_cell);
   drawLabeledCell(status_car2_direction_cell);
   drawLabeledCell(status_car3_direction_cell);

   drawLabeledCell(status_car1_status_cell);
   drawLabeledCell(status_car2_status_cell);
   drawLabeledCell(status_car3_status_cell);
}
