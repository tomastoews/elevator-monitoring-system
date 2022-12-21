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
#define TOTAL_FLOORS_COUNT 51
#define ELEVATORS_FULL_WIDTH (int)WIDTH*0.40
#define ELEVATORS_MARGIN_X 60
#define ELEVATORS_MARGIN_Y 50
#define TEXT_Y_POS_OFFSET 7
#define CELL_WIDTH ELEVATORS_FULL_WIDTH * 0.25
#define CELL_HEIGHT 50

bool running = true;
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

struct Text {
   SDL_Texture *texture;
   SDL_Rect *rect;
   char *text;
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

struct LabeledCell elevator_header_cells[SHAFTS_COUNT+1];
struct LabeledCell floor_description_cells[TOTAL_FLOORS_COUNT];

// struct Text texts[1];
int texts_count = 1;

int floor_display_index = TOTAL_FLOORS_COUNT-1; // VISIBLE_FLOORS_COUNT;

SDL_Rect shafts[TOTAL_FLOORS_COUNT][SHAFTS_COUNT];

void cleanup() {
   for (int i = 0; i < texts_count; i++) {
      // SDL_DestroyTexture(texts[i].texture);
      // free(texts[i].rect);
      // free(texts[i].text); Need to do? Did not use malloc()
   }
   for (int i = 0; i < SHAFTS_COUNT+1; i++) {
      struct LabeledCell *cell_ptr = &elevator_header_cells[i];
      free(cell_ptr->rect);
      free(cell_ptr->frame_rect);
      free(cell_ptr->text_struct->rect);
      // free(cell_ptr->text_struct->text); // <- Error: invalid pointer / Invalid free()
      SDL_DestroyTexture(cell_ptr->text_struct->texture);
   }
   for (int i = 0; i < TOTAL_FLOORS_COUNT; i++) {
      struct LabeledCell *cell_ptr = &floor_description_cells[i];
      free(cell_ptr->rect);
      free(cell_ptr->frame_rect);
      free(cell_ptr->text_struct->rect);
      SDL_DestroyTexture(cell_ptr->text_struct->texture);
   }
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

void update_cell_position(struct LabeledCell *cell) {

}

void set_centered_position_in_cell(int *x, int *y, int text_width, int text_height, int origin_x, int origin_y) {
   *x = (int)(origin_x+(CELL_WIDTH/2)-(text_width/2));
   *y = (int)(origin_y+(CELL_HEIGHT/2)-(text_height/2) -1); // - TEXT_Y_POS_OFFSET;
}

void set_text(struct Text *text_struct, char* text, int font_size, int x, int y, int text_width, int text_height) {
   TTF_Font* font = TTF_OpenFont("WagnerModern.TTF", font_size);
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
   SDL_FreeSurface(surface);
   TTF_CloseFont(font);
}

struct LabeledCell* construct_labeled_cell(int x, int y, int width, int height, char *text, int font_size) {
   // 1. Create rect
   SDL_Rect *rect = malloc(sizeof(SDL_Rect));
   populate_rect(rect, x, y, CELL_WIDTH, CELL_HEIGHT);
      
   // 2. Create frame_rect 
   SDL_Rect *frame_rect = malloc(sizeof(SDL_Rect));
   populate_rect(frame_rect, rect->x, rect->y, rect->w, rect->h);
   
   // 3. Create text_struct
   struct Text *text_struct = malloc(sizeof(struct Text)); 
   int text_height = font_size+10;
   int text_width = strlen(text)*(text_height/2); // Correct formula to determine font dimensions.
   int text_x, text_y;
   set_centered_position_in_cell(&text_x, &text_y, text_width, text_height, x, y);
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
   int title_bar_gab = 20;

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
   for (int i = 0; i < TOTAL_FLOORS_COUNT; i++) {
      int cell_x = start_x;
      int cell_y = start_y+CELL_HEIGHT+title_bar_gab+(CELL_HEIGHT*i);
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

//void create_texts() {
//   char* text = "CAR #"; // Add 'n\' at the end?
//   int height = 30;
//   int width = strlen(text)*(height/2); // Correct formular to calculate font dimensions
//   int x, y;
//   set_centered_position_in_elevator_cell(&x, &y, width, height, &title_bar);
//   set_text(&texts[0], text, x, y, width, height);
//}

void draw_texts() {
   // SDL_SetRenderDrawColor(renderer, color_white.r , color_white.g, color_white.b, 0);
   // for (int i = 0; i < texts_count; i++) {
   //   SDL_RenderCopy(renderer, texts[i].texture, NULL, texts[i].rect);
   // }
}

void draw_elevator_cells() {
   int full_width = (int)WIDTH*0.40;
   int shafts_x = 60 + CELL_WIDTH;
   int start_x = ELEVATORS_MARGIN_X;
   int start_y = ELEVATORS_MARGIN_Y;
   int shaft_width = CELL_WIDTH;
   int floor_height = 50;
   int title_bar_gab = 20;

   // Draw elevator headers cells
   for (int i = 0; i < SHAFTS_COUNT+1; i++) { 
      // Cell
      SDL_SetRenderDrawColor(renderer, color_gray.r, color_gray.g, color_gray.b, 0);
      SDL_RenderFillRect(renderer, elevator_header_cells[i].rect);
      // Cell frame
      SDL_SetRenderDrawColor(renderer, color_gray.r+50, color_gray.g+50, color_gray.b+50, 0);
      SDL_RenderDrawRect(renderer, elevator_header_cells[i].frame_rect);
      // Text
      SDL_SetRenderDrawColor(renderer, color_white.r, color_white.g, color_white.b, 0);
      SDL_RenderCopy(renderer, elevator_header_cells[i].text_struct->texture, NULL, elevator_header_cells[i].text_struct->rect);
   }

   // Draw floor description cells
   int iteration_counter = 0;
   for (int i = floor_display_index; i > (floor_display_index-VISIBLE_FLOORS_COUNT); i--) {
      // All positions have to be updated
      struct LabeledCell *cell_ptr = &floor_description_cells[i];
      int pos_x = start_x;
      int pos_y = start_y+CELL_HEIGHT+title_bar_gab+(CELL_HEIGHT*iteration_counter);
      cell_ptr->rect->y = pos_y;
      cell_ptr->frame_rect->y = pos_y;
      set_centered_position_in_cell(
         &cell_ptr->text_struct->rect->x, &cell_ptr->text_struct->rect->y,
	 cell_ptr->text_struct->rect->w, cell_ptr->text_struct->rect->h, 
	 pos_x, pos_y
      );
      // Cell
      SDL_SetRenderDrawColor(renderer, color_gray.r, color_gray.g, color_gray.b, 0);
      SDL_RenderFillRect(renderer, floor_description_cells[i].rect);
      // Cell frame
      SDL_SetRenderDrawColor(renderer, color_gray.r+50, color_gray.g+50, color_gray.b+50, 0);
      SDL_RenderDrawRect(renderer, floor_description_cells[i].frame_rect);
      // Text
      SDL_SetRenderDrawColor(renderer, color_white.r, color_white.g, color_white.b, 0);
      SDL_RenderCopy(renderer, floor_description_cells[i].text_struct->texture, NULL, floor_description_cells[i].text_struct->rect);
      iteration_counter++;
   }
  
   // Draw shaft cells
   for (int x = 0; x < SHAFTS_COUNT; x++) {
      for (int y = 0; y < VISIBLE_FLOORS_COUNT; y++) {
         SDL_Rect floor1;
         floor1.x = shafts_x+(shaft_width*x);
         floor1.y = (start_y+CELL_HEIGHT+title_bar_gab)+(floor_height*y);
         floor1.w = shaft_width;
         floor1.h = floor_height;
         SDL_SetRenderDrawColor(renderer, color_gray.r+100, color_gray.g+100, color_gray.b+100, 0);
         SDL_RenderDrawRect(renderer, &floor1);
      }
   }

}

void draw_shafts() {
   
}

void draw_top_menu() {
   
}

void increase_floor_display_index() {
   if (floor_display_index+1 < TOTAL_FLOORS_COUNT) {
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
      }
   }		   
}

int main() {
   init();
   create_elevator_cells();
   // create_texts();
   while(running) {
      SDL_Event event;
      while (SDL_PollEvent(&event)) {
         handle_events(&event);
      }
      SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
      SDL_RenderClear(renderer);
      draw_elevator_cells();
      draw_texts();
      SDL_RenderPresent(renderer);
   }

   cleanup();
   return 0;
}