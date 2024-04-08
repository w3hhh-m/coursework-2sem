#ifndef FILE_FUNCTIONS_H
#define FILE_FUNCTIONS_H

#include "structures_and_errors.h"

/* Function declarations */
void read_png_file(char *file_name, Png *image);
void write_png_file(char *file_name, Png *image);
void print_png_info(Png *image);

#endif /* FILE_FUNCTIONS_H */
