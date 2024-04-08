#ifndef CORE_FUNCTIONS_H
#define CORE_FUNCTIONS_H

#include "structures_and_errors.h"

/* Function declarations */
void color_replace(Png *image, char* old_color, char* new_color);
void copy_area(Png *image, char* left_up, char* right_down, char* dest_left_up);
void filled_rects(Png *image, char* string_color, char* string_border_color, char* thickness);
void ornament(Png *image, char* pattern, char* string_color, char* thickness, char* count);

#endif /* CORE_FUNCTIONS_H */
