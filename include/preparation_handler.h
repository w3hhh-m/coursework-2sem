#ifndef PREPARATION_HANDLER_H
#define PREPARATION_HANDLER_H

#include "structures.h"

void handle_arguments(int argc, char *argv[], Options *options);

int* process_color(char* string_color);

int* process_coordinates(char* string_coordinates);

#endif