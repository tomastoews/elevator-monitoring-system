#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#define WIDTH 1280
#define HEIGHT 1024
#define SHAFTS_COUNT 3
#define VISIBLE_FLOORS_COUNT 17
#define TOTAL_FLOORS_COUNT 50 // 51 (Plus penthouse)
#define ELEVATORS_FULL_WIDTH (int)WIDTH*0.40
#define ELEVATORS_MARGIN_X 60
#define ELEVATORS_MARGIN_Y 50
#define TEXT_Y_POS_OFFSET 7
#define CELL_WIDTH ELEVATORS_FULL_WIDTH * 0.25
#define CELL_HEIGHT 50
#define CELL_GAP 20

bool running = true;
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

struct Text {
   SDL_Texture *texture;
   SDL_Rect *rect;
   char *text;
   int font_size;
   char *font_name;
};

struct LabeledCell {
   struct Text *text_struct;
   SDL_Rect *rect;
   SDL_Rect *frame_rect;
};

struct Point { 
   int x1; int y1; 
   int x2; int y2; 
};

SDL_Color color_gray = {
   53, 70, 79, 255
};

SDL_Color color_light_gray = {
   53+50, 70+50, 79+50, 255
};

SDL_Color color_white = {
   255, 255, 255, 255
};

char *primary_font_name = "Waukegan_LDO_Bold.TTF";
char *secondary_font_name = "WagnerModern.TTF";
char *title_font_name = "Beautiful_Marry.TTF";

// Elevator side
struct LabeledCell elevator_header_cells[SHAFTS_COUNT+1];
struct LabeledCell floor_description_cells[TOTAL_FLOORS_COUNT];
SDL_Rect shafts[TOTAL_FLOORS_COUNT][SHAFTS_COUNT];

int floor_display_index = TOTAL_FLOORS_COUNT;

int car_movement_animation_trigger_frames = 5000;
int car_movement_animation_passed_frames = 0;

// Status Side
struct LabeledCell status_header_cells[SHAFTS_COUNT+1];

struct Text general_texts[10];

struct LabeledCell status_floor_label_cell;
struct LabeledCell status_direction_label_cell;
struct LabeledCell status_status_label_cell;

struct LabeledCell status_car1_floor_cell;
struct LabeledCell status_car2_floor_cell;
struct LabeledCell status_car3_floor_cell;

struct LabeledCell status_car1_direction_cell;
struct LabeledCell status_car2_direction_cell;
struct LabeledCell status_car3_direction_cell;

struct LabeledCell status_car1_status_cell;
struct LabeledCell status_car2_status_cell;
struct LabeledCell status_car3_status_cell;

void free_labeled_cell(struct LabeledCell *cell_ptr) {
   free(cell_ptr->rect);
   free(cell_ptr->frame_rect);
   free(cell_ptr->text_struct->rect);
   // free(cell_ptr->text_struct->text); // <- Error: invalid pointer / Invalid free()
   SDL_DestroyTexture(cell_ptr->text_struct->texture);
}

void cleanup() { 
   for (int i = 0; i < SHAFTS_COUNT+1; i++) {
      free_labeled_cell(&elevator_header_cells[i]);
   }
   for (int i = 0; i <= TOTAL_FLOORS_COUNT; i++) {
      free_labeled_cell(&floor_description_cells[i]);
   }
   for (int i = 0; i < SHAFTS_COUNT+1; i++) {
      free_labeled_cell(&status_header_cells[i]);
   }
   for (int i = 0; i < sizeof(general_texts)/sizeof(struct Text); i++) {
      free(general_texts[i].rect);
      SDL_DestroyTexture(general_texts[i].texture);
   }
   free_labeled_cell(&status_floor_label_cell);
   free_labeled_cell(&status_direction_label_cell);
   free_labeled_cell(&status_status_label_cell);

   free_labeled_cell(&status_car1_floor_cell);
   free_labeled_cell(&status_car2_floor_cell);
   free_labeled_cell(&status_car3_floor_cell);

   free_labeled_cell(&status_car1_direction_cell);
   free_labeled_cell(&status_car2_direction_cell);
   free_labeled_cell(&status_car3_direction_cell);

   free_labeled_cell(&status_car1_status_cell);
   free_labeled_cell(&status_car2_status_cell);
   free_labeled_cell(&status_car3_status_cell);

   SDL_DestroyRenderer(renderer);
   SDL_DestroyWindow(window);
   SDL_Quit();
}

void init() {
   SDL_Init(SDL_INIT_VIDEO);
   TTF_Init();
   SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, /*windowFlags*/0, &window, &renderer);
   SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
   SDL_RenderClear(renderer);
   SDL_RenderPresent(renderer);
}

void populate_rect(SDL_Rect *rect, int x, int y, int w, int h) {
   rect->x = x;
   rect->y = y;
   rect->w = w;
   rect->h = h;
}

void set_centered_position_in_cell(int *x, int *y, int text_width, int text_height, SDL_Rect *rect) {
   *x = (int)(rect->x+(rect->w/2)-(text_width/2));
   *y = (int)(rect->y+(rect->h/2)-(text_height/2) -1); // - TEXT_Y_POS_OFFSET;
}

void set_text_size(int *text_width, int *text_height, char *text, int font_size) {
   *text_height = font_size+10;
   *text_width = strlen(text)*(*text_height/2); // Correct way to determine font dimensions.
}

void update_text(struct Text *text_struct, char *text, SDL_Color *color) {
   TTF_Font *font = TTF_OpenFont(text_struct->font_name, text_struct->font_size);
   SDL_Surface *surface = TTF_RenderText_Blended(font, text, *color);
   SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
   SDL_DestroyTexture(text_struct->texture);
   text_struct->texture = texture;
   // free(text_struct->text); // FIXME -> free(): invalid pointer.
   text_struct->text = text;
   set_text_size(&text_struct->rect->w, &text_struct->rect->h, text, text_struct->font_size);
   SDL_FreeSurface(surface);
   TTF_CloseFont(font);
}

void set_text(struct Text *text_struct, char* text, int x, int y, int text_width, int text_height, SDL_Color *color) {
   TTF_Font *font = TTF_OpenFont(text_struct->font_name, text_struct->font_size);
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

struct Text* construct_text_struct(char *text, int font_size, char *font_name, SDL_Color *color, int text_x, int text_y, SDL_Rect *container_rect) {
   struct Text *text_struct = malloc(sizeof(struct Text));
   text_struct->font_size = font_size;
   text_struct->font_name = font_name;
   int text_width, text_height;
   set_text_size(&text_width, &text_height, text, font_size);
   if (container_rect != NULL) {
      set_centered_position_in_cell(&text_x, &text_y, text_width, text_height, container_rect);
   }
   set_text(text_struct, text, text_x, text_y, text_width, text_height, color);
   return text_struct;
}

struct LabeledCell* construct_labeled_cell(int x, int y, int width, int height, char *text, char *font_name, int font_size) {
   // 1. Create rect
   SDL_Rect *rect = malloc(sizeof(SDL_Rect));
   populate_rect(rect, x, y, width, height);
      
   // 2. Create frame_rect 
   SDL_Rect *frame_rect = malloc(sizeof(SDL_Rect));
   populate_rect(frame_rect, rect->x, rect->y, rect->w, rect->h);
   
   // 3. Create text_struct
   struct Text *text_struct = construct_text_struct(text, 15, primary_font_name, &color_white, x, y, rect);
         
   // 4. Build cell struct and populate it with all member data
   struct LabeledCell *cell = malloc(sizeof(struct LabeledCell));
   cell->text_struct = text_struct;
   cell->rect = rect;
   cell->frame_rect = frame_rect;

   return cell;
}

void create_elevator_cells() {
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
	 char num_char[1];
	 sprintf(num_char, "%d", i);
	 strcat(text, "CAR ");
         strcat(text, num_char);
      }
      struct LabeledCell *cell = construct_labeled_cell(cell_x, cell_y, CELL_WIDTH, CELL_HEIGHT, text, primary_font_name, 20);
      memmove(&elevator_header_cells[i], cell, sizeof(struct LabeledCell));
   }

   // Create floor description cells
   for (int i = 0; i <= TOTAL_FLOORS_COUNT; i++) {
      int cell_x = start_x;
      int cell_y = start_y+CELL_HEIGHT+CELL_GAP+(CELL_HEIGHT*i);
      char *text = malloc(8);
      memset(text, 0, 8);
      strcat(text, "Floor ");
      char num_chars[2];
      sprintf(num_chars, "%d", i);
      strcat(text, num_chars);
      struct LabeledCell *cell = construct_labeled_cell(cell_x, cell_y, CELL_WIDTH, CELL_HEIGHT, text, primary_font_name, 15);
      memmove(&floor_description_cells[i], cell, sizeof(struct LabeledCell));
   }
}

void create_status_header_cells() {
   int start_x = WIDTH-(ELEVATORS_MARGIN_X+(CELL_WIDTH*4));
   int start_y = ELEVATORS_MARGIN_Y+(3*CELL_HEIGHT);
   for (int i = 0; i < SHAFTS_COUNT+1; i++) {
      int cell_x = start_x+(CELL_WIDTH*i);
      int cell_y = start_y;
      int size = (i == 0) ? 5 : 1;
      char *text = malloc(size);
      memset(text, 0, size);
      if (i == 0) {
         text = "CAR #";
      }
      else {
	 char num_char[1];
	 sprintf(num_char, "%d", i);
	 text = num_char;
      }
      struct LabeledCell *cell = construct_labeled_cell(cell_x, cell_y, CELL_WIDTH, CELL_HEIGHT, text, primary_font_name, 15);
      memmove(&status_header_cells[i], cell, sizeof(struct LabeledCell));
   } 
}

void create_status_cells() {
   int start_x = WIDTH-(ELEVATORS_MARGIN_X+(CELL_WIDTH*4));
   int start_y = status_header_cells[0].rect->y+CELL_HEIGHT+CELL_GAP;
   
   // Create status floor label cells
   struct LabeledCell *floor_label = construct_labeled_cell(start_x, start_y, CELL_WIDTH, CELL_HEIGHT, "FLOOR", primary_font_name, 15);
   memmove(&status_floor_label_cell, floor_label, sizeof(struct LabeledCell));

   struct LabeledCell *direction_cell = construct_labeled_cell(start_x, start_y+(CELL_HEIGHT), CELL_WIDTH, CELL_HEIGHT, "DIRECTION", primary_font_name, 15);
   memmove(&status_direction_label_cell, direction_cell, sizeof(struct LabeledCell));

   struct LabeledCell *status_cell = construct_labeled_cell(start_x, start_y+(CELL_HEIGHT*2), CELL_WIDTH, CELL_HEIGHT, "STATUS", primary_font_name, 15);
   memmove(&status_status_label_cell, status_cell, sizeof(struct LabeledCell));

   // Create car floor status cells
   struct LabeledCell *car1_floor_cell = construct_labeled_cell(start_x+CELL_WIDTH, start_y, CELL_WIDTH, CELL_HEIGHT, "1", primary_font_name, 15);
   memmove(&status_car1_floor_cell, car1_floor_cell, sizeof(struct LabeledCell));

   struct LabeledCell *car2_floor_cell = construct_labeled_cell(start_x+(CELL_WIDTH*2), start_y, CELL_WIDTH, CELL_HEIGHT, "1",  primary_font_name,15);
   memmove(&status_car2_floor_cell, car2_floor_cell, sizeof(struct LabeledCell));

   struct LabeledCell *car3_floor_cell = construct_labeled_cell(start_x+(CELL_WIDTH*3), start_y, CELL_WIDTH, CELL_HEIGHT, "1", primary_font_name, 15);
   memmove(&status_car3_floor_cell, car3_floor_cell, sizeof(struct LabeledCell));

   // Create car direction status cells
   struct LabeledCell *car1_direction_cell = construct_labeled_cell(start_x+CELL_WIDTH, start_y+CELL_HEIGHT, CELL_WIDTH, CELL_HEIGHT, "UP", primary_font_name, 15);
   memmove(&status_car1_direction_cell, car1_direction_cell, sizeof(struct LabeledCell));

   struct LabeledCell *car2_direction_cell = construct_labeled_cell(start_x+(CELL_WIDTH*2), start_y+CELL_HEIGHT, CELL_WIDTH, CELL_HEIGHT, "UP", primary_font_name, 15);
   memmove(&status_car2_direction_cell, car2_direction_cell, sizeof(struct LabeledCell));

   struct LabeledCell *car3_direction_cell = construct_labeled_cell(start_x+(CELL_WIDTH*3), start_y+CELL_HEIGHT, CELL_WIDTH, CELL_HEIGHT, "UP", primary_font_name, 15);
   memmove(&status_car3_direction_cell, car3_direction_cell, sizeof(struct LabeledCell));

   // Create car status cells
   struct LabeledCell *car1_status_cell = construct_labeled_cell(start_x+CELL_WIDTH, start_y+(CELL_HEIGHT*2), CELL_WIDTH, CELL_HEIGHT, "AUTOMATIC", primary_font_name, 15);
   memmove(&status_car1_status_cell, car1_status_cell, sizeof(struct LabeledCell));

   struct LabeledCell *car2_status_cell = construct_labeled_cell(start_x+(CELL_WIDTH*2), start_y+(CELL_HEIGHT*2), CELL_WIDTH, CELL_HEIGHT, "AUTOMATIC", primary_font_name, 15);
   memmove(&status_car2_status_cell, car2_status_cell, sizeof(struct LabeledCell));

   struct LabeledCell *car3_status_cell = construct_labeled_cell(start_x+(CELL_WIDTH*3), start_y+(CELL_HEIGHT*2), CELL_WIDTH, CELL_HEIGHT, "AUTOMATIC", primary_font_name, 15);
   memmove(&status_car3_status_cell, car3_status_cell, sizeof(struct LabeledCell));
}

void create_general_texts() {
   int help_x = status_status_label_cell.rect->x;
   int help_y = status_status_label_cell.rect->y+(CELL_HEIGHT*2);

   // Title text
   struct Text *title_text = construct_text_struct("The Tower", 60, title_font_name, &color_white, help_x, ELEVATORS_MARGIN_Y, NULL);
   memmove(&general_texts[0], title_text, sizeof(struct Text));

   // Sub header text
   struct Text *sub_title_text = construct_text_struct("ELEVATOR MONITORING SYSTEM", 10, primary_font_name, &color_white, help_x, ELEVATORS_MARGIN_Y+90, NULL);
   memmove(&general_texts[1], sub_title_text, sizeof(struct Text));

   // Help texts
   struct Text *text1 = construct_text_struct("KEY FUNCTIONS", 15, primary_font_name, &color_light_gray, help_x, help_y, NULL);
   memmove(&general_texts[2], text1, sizeof(struct Text));

   struct Text *text2 = construct_text_struct("SCROLL FLOORS : ARROW UP / DOWN", 15, primary_font_name, &color_light_gray, help_x, help_y+20+30, NULL);
   memmove(&general_texts[3], text2, sizeof(struct Text));

   struct Text *text3 = construct_text_struct("CAR 1 DIRECTION UP : NUMPAD 7", 15, primary_font_name, &color_light_gray, help_x,  help_y+20+60, NULL);
   memmove(&general_texts[4], text3, sizeof(struct Text));

   struct Text *text4 = construct_text_struct("CAR 1 DIRECTION DOWN : NUMPAD 4", 15, primary_font_name, &color_light_gray, help_x, help_y+20+90, NULL);
   memmove(&general_texts[5], text4, sizeof(struct Text));

   struct Text *text5 = construct_text_struct("CAR 2 DIRECTION UP : NUMPAD 8", 15, primary_font_name, &color_light_gray, help_x, help_y+20+120, NULL);
   memmove(&general_texts[6], text5, sizeof(struct Text));

   struct Text *text6 = construct_text_struct("CAR 2 DIRECTION DOWN : NUMPAD 5", 15, primary_font_name, &color_light_gray, help_x, help_y+20+150, NULL);
   memmove(&general_texts[7], text6, sizeof(struct Text));

   struct Text *text7 = construct_text_struct("CAR 3 DIRECTION UP : NUMPAD 9", 15, primary_font_name, &color_light_gray, help_x, help_y+20+180, NULL);
   memmove(&general_texts[8], text7, sizeof(struct Text));
  
   struct Text *text8 = construct_text_struct("CAR 3 DIRECTION DOWN : NUMPAD 6", 15, primary_font_name, &color_light_gray, help_x, help_y+20+210, NULL);
   memmove(&general_texts[9], text8, sizeof(struct Text));

}

void draw_labeled_cell(struct LabeledCell *cell) {
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

void draw_elevator_cells() {
   int full_width = (int)WIDTH*0.40;
   int start_x = ELEVATORS_MARGIN_X;
   int start_y = ELEVATORS_MARGIN_Y;

   // Draw elevator headers cells
   for (int i = 0; i < SHAFTS_COUNT+1; i++) {
      draw_labeled_cell(&elevator_header_cells[i]);
   }

   // Draw floor description cells
   int iteration_counter = 0;
   for (int i = floor_display_index; i > (floor_display_index-VISIBLE_FLOORS_COUNT); i--) {

      // Draw floor description cell
      // -> All positions have to be recalculated first
      struct LabeledCell *cell_ptr = &floor_description_cells[i];
      int pos_x = start_x;
      int pos_y = start_y+CELL_HEIGHT+CELL_GAP+(CELL_HEIGHT*iteration_counter);
      cell_ptr->rect->y = pos_y;
      cell_ptr->frame_rect->y = pos_y;
      set_centered_position_in_cell(
         &cell_ptr->text_struct->rect->x, &cell_ptr->text_struct->rect->y,
	 cell_ptr->text_struct->rect->w, cell_ptr->text_struct->rect->h, 
	 cell_ptr->rect
      );
      draw_labeled_cell(cell_ptr);
      
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
 	 char *car_direction = NULL;

	 if (x == 0) {
            car_floor = atoi(status_car1_floor_cell.text_struct->text);
	    car_direction = status_car1_direction_cell.text_struct->text;
	 }
	 else if (x == 1) {
	    car_floor = atoi(status_car2_floor_cell.text_struct->text);
	    car_direction = status_car2_direction_cell.text_struct->text;
	 }
	 else if (x == 2) {
	    car_floor = atoi(status_car3_floor_cell.text_struct->text);
	    car_direction = status_car3_direction_cell.text_struct->text;
	 }

	 // Check if the car of this shaft is on the current (x iteation) floor
	 if (car_floor == i) {
            // Draw triangle
            int lines_count = CELL_HEIGHT-20;
	    int triangle_width = CELL_WIDTH-20;
	    int triangle_height = CELL_HEIGHT-20;
	    int triangle_x = shaft_section.x+10;
	    int triangle_y = shaft_section.y+10;
	    struct Point line1 = { 0, 0, 0, 0 };
	    struct Point line2 = { 0, 0, 0, 0 };
	    struct Point line3 = { 0, 0, 0, 0 };

	    if (car_direction == "DOWN") {
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

void draw_status_cells() {
   // Draw status header cells
   for (int i = 0; i < SHAFTS_COUNT+1; i++) {
      draw_labeled_cell(&status_header_cells[i]);   
   }

   // Draw row labels
   draw_labeled_cell(&status_floor_label_cell);
   draw_labeled_cell(&status_direction_label_cell);
   draw_labeled_cell(&status_status_label_cell);

   // Draw status cells
   draw_labeled_cell(&status_car1_floor_cell);
   draw_labeled_cell(&status_car2_floor_cell);
   draw_labeled_cell(&status_car3_floor_cell);

   draw_labeled_cell(&status_car1_direction_cell);
   draw_labeled_cell(&status_car2_direction_cell);
   draw_labeled_cell(&status_car3_direction_cell);

   draw_labeled_cell(&status_car1_status_cell);
   draw_labeled_cell(&status_car2_status_cell);
   draw_labeled_cell(&status_car3_status_cell);
}

void draw_general_texts() {
   for (int i = 0; i < sizeof(general_texts)/sizeof(struct Text); i++) {
      SDL_RenderCopy(renderer, general_texts[i].texture, NULL, general_texts[i].rect);
   }
}

void draw_top_menu() {
   
}

void move_car(struct LabeledCell *status_car_direction_cell, struct LabeledCell *status_car_floor_cell) {
   int n = atoi(status_car_floor_cell->text_struct->text);
   if (status_car_direction_cell->text_struct->text == "UP" && n < TOTAL_FLOORS_COUNT) {
      n++;
   } else if (status_car_direction_cell->text_struct->text == "DOWN" && n > 1) {
      n--;
   }
   char *num_char = malloc(1);
   memset(num_char, ' ', strlen(num_char));
   sprintf(num_char, "%d", n);
   update_text(status_car_floor_cell->text_struct, num_char, &color_white); 
}

void update_animations() {
   if (car_movement_animation_passed_frames >= car_movement_animation_trigger_frames) {
      move_car(&status_car1_direction_cell, &status_car1_floor_cell);
      move_car(&status_car2_direction_cell, &status_car2_floor_cell);
      move_car(&status_car3_direction_cell, &status_car3_floor_cell);
      car_movement_animation_passed_frames = 0;
   }
   car_movement_animation_passed_frames++;
}

void increase_floor_display_index() {
   if (floor_display_index+1 <= TOTAL_FLOORS_COUNT) {
      floor_display_index++;
   }
}

void decrease_floor_display_index() {
   if ((floor_display_index-VISIBLE_FLOORS_COUNT) > 0) {
      floor_display_index--;
   }
}

void handle_events(SDL_Event *event) {
   if (event->type == SDL_QUIT) {
      running = false;
   }
   if (event->type == SDL_KEYDOWN) {
      switch (event->key.keysym.sym) {
         case SDLK_UP:
   	    increase_floor_display_index();
	    break;
	 case SDLK_DOWN:
	    decrease_floor_display_index();
	    break;	
	 case SDLK_KP_7: 
	    update_text(status_car1_direction_cell.text_struct, "UP", &color_white);
	    break;
	 case SDLK_KP_4:
            update_text(status_car1_direction_cell.text_struct, "DOWN", &color_white);
	    break;
	 case SDLK_KP_8:
            update_text(status_car2_direction_cell.text_struct, "UP", &color_white);
	    break;
	 case SDLK_KP_5:
            update_text(status_car2_direction_cell.text_struct, "DOWN", &color_white);
	    break;
	 case SDLK_KP_9:
	    update_text(status_car3_direction_cell.text_struct, "UP", &color_white);
	    break;
	 case SDLK_KP_6:
            update_text(status_car3_direction_cell.text_struct, "DOWN", &color_white);
	    break;
      }
   }		   
}

int main() {
   init();
   create_elevator_cells();
   create_status_header_cells();
   create_status_cells();
   create_general_texts();

   while(running) {
      SDL_Event event;
      while (SDL_PollEvent(&event)) {
         handle_events(&event);
      }
      SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
      SDL_RenderClear(renderer);
      draw_elevator_cells();
      draw_status_cells();
      draw_general_texts();
      update_animations();
      SDL_RenderPresent(renderer);
   }

   cleanup();
   return 0;
}
