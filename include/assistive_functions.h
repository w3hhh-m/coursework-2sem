#ifndef ASSISTIVE_FUNCTIONS_H
#define ASSISTIVE_FUNCTIONS_H

#include "structures_and_errors.h"

/* Function declarations */
void print_help();
int* process_color(char* string_color);
int* process_coordinates(char* string_coordinates);
void handle_arguments(int argc, char *argv[], Options *options);
void draw_border(Png *image, int x1, int y1, int x2, int y2, int* border_color, char* thickness);

#endif /* ASSISTIVE_FUNCTIONS_H */
