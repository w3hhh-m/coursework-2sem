#ifndef TASK_HANDLER_H
#define TASK_HANDLER_H

#include "structures.h"

void print_help();

void print_png_info(Png *image);

void task_switcher(Options options, Png *image);

void color_replace(Png *image, char* old_color, char* new_color);

void copy_area(Png *image, char* left_up, char* right_down, char* dest_left_up);

void filled_rects(Png *image, char* string_color, char* string_border_color, char* thickness);

void ornament(Png *image, char* pattern, char* string_color, char* thickness, char* count);

#endif