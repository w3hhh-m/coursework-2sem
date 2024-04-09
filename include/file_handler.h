#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

#include "structures.h"

void read_png_file(char *file_name, Png *image);

void write_png_file(char *file_name, Png *image);

#endif