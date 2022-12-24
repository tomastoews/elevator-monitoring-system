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
};

struct LabeledCell {
   struct Text *text_struct;
   SDL_Rect *rect;
   SDL_Rect *frame_rect;
};

SDL_Color color_gray = {
   53, 70, 79, 255
};

SDL_Color color_white = {
   255, 255, 255, 255
};

// Elevator side
struct LabeledCell elevator_header_cells[SHAFTS_COUNT+1];
struct LabeledCell floor_description_cells[TOTAL_FLOORS_COUNT];
SDL_Rect shafts[TOTAL_FLOORS_COUNT][SHAFTS_COUNT];

int floor_display_index = TOTAL_FLOORS_COUNT; // VISIBLE_FLOORS_COUNT;

// Status Side
struct LabeledCell status_header_cells[SHAFTS_COUNT+1];

struct Text title_text;
struct Text sub_title_text;

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
   *text_width = strlen(text)*(*text_height/2); // Correct formula to determine font dimensions.
}

void update_text(struct Text *text_struct, char *text) {
   printf("Update text to: >%s<\n", text);
   TTF_Font *font = TTF_OpenFont("WagnerModern.TTF", text_struct->font_size);
   SDL_Surface *surface = TTF_RenderText_Solid(font, text, color_white);
   SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
   SDL_DestroyTexture(text_struct->texture);
   text_struct->texture = texture;
   // free(text_struct->text); // FIXME -> free(): invalid pointer.
   text_struct->text = text;
   set_text_size(&text_struct->rect->w, &text_struct->rect->h, text, text_struct->font_size);
   SDL_FreeSurface(surface);
   TTF_CloseFont(font);
}

void set_text(struct Text *text_struct, char* text, int font_size, int x, int y, int text_width, int text_height) {
   TTF_Font *font = TTF_OpenFont("WagnerModern.TTF", font_size);
   SDL_Surface *surface = TTF_RenderText_Solid(font, text, color_white);
   SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
   SDL_Rect *rect = malloc(sizeof(SDL_Rect));
   rect->x = x;
   rect->y = y;
   rect->w = text_width;
   rect->h = text_height;
   text_struct->rect = rect;
   text_struct->texture = texture;
   text_struct->text = text;
   text_struct->font_size = font_size;
   SDL_FreeSurface(surface);
   TTF_CloseFont(font);
}

struct LabeledCell* construct_labeled_cell(int x, int y, int width, int height, char *text, int font_size) {
   // 1. Create rect
   SDL_Rect *rect = malloc(sizeof(SDL_Rect));
   populate_rect(rect, x, y, width, height);
      
   // 2. Create frame_rect 
   SDL_Rect *frame_rect = malloc(sizeof(SDL_Rect));
   populate_rect(frame_rect, rect->x, rect->y, rect->w, rect->h);
   
   // 3. Create text_struct
   struct Text *text_struct = malloc(sizeof(struct Text)); 
   int text_width, text_height;
   set_text_size(&text_width, &text_height, text, font_size);
   int text_x, text_y;
   set_centered_position_in_cell(&text_x, &text_y, text_width, text_height, rect);
   set_text(text_struct, text, font_size, text_x, text_y, text_width, text_height);
   
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
      struct LabeledCell *cell = construct_labeled_cell(cell_x, cell_y, CELL_WIDTH, CELL_HEIGHT, text, 20);
      memmove(&elevator_header_cells[i], cell, sizeof(struct LabeledCell));
   }

   // Create floor description cells
   for (int i = 0; i <= TOTAL_FLOORS_COUNT; i++) {
      int cell_x = start_x;
      int cell_y = start_y+CELL_HEIGHT+CELL_GAP+(CELL_HEIGHT*i);
      char *text = malloc(8);
      memset(text, 0, strlen(text));
      strcat(text, "Floor ");
      char num_chars[2];
      sprintf(num_chars, "%d", i);
      strcat(text, num_chars);
      struct LabeledCell *cell = construct_labeled_cell(cell_x, cell_y, CELL_WIDTH, CELL_HEIGHT, text, 15);
      memmove(&floor_description_cells[i], cell, sizeof(struct LabeledCell));
   }
}

void create_status_header_cells() {
   int start_x = WIDTH-(ELEVATORS_MARGIN_X+(CELL_WIDTH*4));
   int start_y = ELEVATORS_MARGIN_Y+(3*CELL_HEIGHT);
   for (int i = 0; i < SHAFTS_COUNT+1; i++) {
      int cell_x = start_x+(CELL_WIDTH*i);
      int cell_y = start_y;
      char *text; NULL;
      if (i == 0) {
	 text = malloc(5);
         memset(text, ' ', strlen(text));
         text = "CAR #";
      }
      else {
	 text = malloc(1);
	 memset(text, ' ', strlen(text));
	 text[1] = '\0';
	 char num_char[1];
	 sprintf(num_char, "%d", i);
	 text[0] = num_char[0];
      }
      struct LabeledCell *cell = construct_labeled_cell(cell_x, cell_y, CELL_WIDTH, CELL_HEIGHT, text, 15);
      memmove(&status_header_cells[i], cell, sizeof(struct LabeledCell));
   } 
}

void create_status_cells() {
   int start_x = WIDTH-(ELEVATORS_MARGIN_X+(CELL_WIDTH*4));
   int start_y = status_header_cells[0].rect->y+CELL_HEIGHT+CELL_GAP;
   
   // Create status floor label cells
   struct LabeledCell *floor_label = construct_labeled_cell(start_x, start_y, CELL_WIDTH, CELL_HEIGHT, "FLOOR", 15);
   memmove(&status_floor_label_cell, floor_label, sizeof(struct LabeledCell));

   struct LabeledCell *direction_cell = construct_labeled_cell(start_x, start_y+(CELL_HEIGHT), CELL_WIDTH, CELL_HEIGHT, "DIRECTION", 15);
   memmove(&status_direction_label_cell, direction_cell, sizeof(struct LabeledCell));

   struct LabeledCell *status_cell = construct_labeled_cell(start_x, start_y+(CELL_HEIGHT*2), CELL_WIDTH, CELL_HEIGHT, "STATUS", 15);
   memmove(&status_status_label_cell, status_cell, sizeof(struct LabeledCell));

   // Create car floor status cells
   struct LabeledCell *car1_floor_cell = construct_labeled_cell(start_x+CELL_WIDTH, start_y, CELL_WIDTH, CELL_HEIGHT, "1", 15);
   memmove(&status_car1_floor_cell, car1_floor_cell, sizeof(struct LabeledCell));

   struct LabeledCell *car2_floor_cell = construct_labeled_cell(start_x+(CELL_WIDTH*2), start_y, CELL_WIDTH, CELL_HEIGHT, "1", 15);
   memmove(&status_car2_floor_cell, car2_floor_cell, sizeof(struct LabeledCell));

   struct LabeledCell *car3_floor_cell = construct_labeled_cell(start_x+(CELL_WIDTH*3), start_y, CELL_WIDTH, CELL_HEIGHT, "1", 15);
   memmove(&status_car3_floor_cell, car3_floor_cell, sizeof(struct LabeledCell));

   // Create car direction status cells
   struct LabeledCell *car1_direction_cell = construct_labeled_cell(start_x+CELL_WIDTH, start_y+CELL_HEIGHT, CELL_WIDTH, CELL_HEIGHT, "UP", 15);
   memmove(&status_car1_direction_cell, car1_direction_cell, sizeof(struct LabeledCell));

   struct LabeledCell *car2_direction_cell = construct_labeled_cell(start_x+(CELL_WIDTH*2), start_y+CELL_HEIGHT, CELL_WIDTH, CELL_HEIGHT, "UP", 15);
   memmove(&status_car2_direction_cell, car2_direction_cell, sizeof(struct LabeledCell));

   struct LabeledCell *car3_direction_cell = construct_labeled_cell(start_x+(CELL_WIDTH*3), start_y+CELL_HEIGHT, CELL_WIDTH, CELL_HEIGHT, "UP", 15);
   memmove(&status_car3_direction_cell, car3_direction_cell, sizeof(struct LabeledCell));

   // Create car staus cells
   struct LabeledCell *car1_status_cell = construct_labeled_cell(start_x+CELL_WIDTH, start_y+(CELL_HEIGHT*2), CELL_WIDTH, CELL_HEIGHT, "AUTOMATIC", 15);
   memmove(&status_car1_status_cell, car1_status_cell, sizeof(struct LabeledCell));

   struct LabeledCell *car2_status_cell = construct_labeled_cell(start_x+(CELL_WIDTH*2), start_y+(CELL_HEIGHT*2), CELL_WIDTH, CELL_HEIGHT, "AUTOMATIC", 15);
   memmove(&status_car2_status_cell, car2_status_cell, sizeof(struct LabeledCell));

   struct LabeledCell *car3_status_cell = construct_labeled_cell(start_x+(CELL_WIDTH*3), start_y+(CELL_HEIGHT*2), CELL_WIDTH, CELL_HEIGHT, "AUTOMATIC", 15);
   memmove(&status_car3_status_cell, car3_status_cell, sizeof(struct LabeledCell));
}

void draw_labeled_cell(struct LabeledCell *cell) {
   // Cell
   SDL_SetRenderDrawColor(renderer, color_gray.r, color_gray.g, color_gray.b, 0);
   SDL_RenderFillRect(renderer, cell->rect);
   // Cell frame
   SDL_SetRenderDrawColor(renderer, color_gray.r+50, color_gray.g+50, color_gray.b+50, 0);
   SDL_RenderDrawRect(renderer, cell->frame_rect);
   // Text
   SDL_SetRenderDrawColor(renderer, color_white.r, color_white.g, color_white.b, 0);
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
      // -> All positions have to be updated first
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

         int car1_floor = atoi(status_car1_floor_cell.text_struct->text);
         int car2_floor = atoi(status_car2_floor_cell.text_struct->text);
         int car3_floor = atoi(status_car3_floor_cell.text_struct->text);

	 // Check if the car of this shaft is on the current (x iteation) floor
	 int car_floor;
         if (x == 0) car_floor = car1_floor;
	 if (x == 1) car_floor = car2_floor;
	 if (x == 2) car_floor = car3_floor;
	 if (car_floor == i) {
	    SDL_Rect car;
	    car.x = shaft_section.x+5;
	    car.y = shaft_section.y+5;
	    car.w = shaft_section.w-10;
	    car.h = shaft_section.h-10;
	    SDL_SetRenderDrawColor(renderer, 100, 100, 255, 0);
	    SDL_RenderFillRect(renderer, &car);
	 }
      }
      iteration_counter++;
   }
   
   // Draw shaft cells
   for (int x = 0; x < SHAFTS_COUNT; x++) {
      for (int y = 0; y < VISIBLE_FLOORS_COUNT; y++) { 
        
      }
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

void draw_shafts() {
   
}

void draw_top_menu() {
   
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
	 case SDLK_n:
	    int n = atoi(status_car1_floor_cell.text_struct->text);
	    printf("Car 1 was on floor %d\n", n);
	    if (n < TOTAL_FLOORS_COUNT) n++;
	    char *num_char = malloc(1);
	    memset(num_char, ' ', strlen(num_char));
	    sprintf(num_char, "%d", n);
	    update_text(status_car1_floor_cell.text_struct, num_char);
	    printf("Stored value: <%s>\n", status_car1_floor_cell.text_struct->text);
	    printf("Car 1 is on foor %c\n", num_char[0]); 
	    break;
      }
   }		   
}

int main() {
   init();
   create_elevator_cells();
   create_status_header_cells();
   create_status_cells();

   while(running) {
      SDL_Event event;
      while (SDL_PollEvent(&event)) {
         handle_events(&event);
      }
      SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
      SDL_RenderClear(renderer);
      draw_elevator_cells();
      draw_status_cells();
      SDL_RenderPresent(renderer);
   }

   cleanup();
   return 0;
}
