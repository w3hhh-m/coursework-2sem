#ifndef DRAWING_HANDLER_H
#define DRAWING_HANDLER_H

#include "structures.h"

void draw_pixel(png_bytep ptr, int* color_values);

void draw_border(Png *image, int x1, int y1, int x2, int y2, int* border_color, char* thickness);

void rectangle_ornament(Png *image, int ornament_thickness, int ornament_count, int* color_values, char* thickness);

void circle_ornament(Png *image, int* color_values);

void semicircles_ornament(Png *image, int ornament_thickness, int ornament_count, int* color_values);

#endif