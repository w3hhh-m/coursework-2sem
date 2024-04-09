#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <png.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <math.h>

/**
 * @brief Structure representing a PNG image.
 * 
 * @param width Width of the image in pixels
 * @param height Height of the image in pixels
 * @param color_type Color type of the image (e.g., RGB, Grayscale)
 * @param bit_depth Bit depth of the image
 * @param png_ptr Pointer to the libpng structure for reading/writing PNG data
 * @param info_ptr Pointer to the libpng structure for storing PNG information
 * @param number_of_passes Number of passes required for interlacing (typically used for progressive rendering)
 * @param row_pointers Pointer to an array of pointers, each pointing to a row of image data
 */
typedef struct Png {
    int width;
    int height;
    png_byte color_type;
    png_byte bit_depth;
    png_structp png_ptr;
    png_infop info_ptr;
    int number_of_passes;
    png_bytep *row_pointers;
} Png;

/**
 * @brief Structure representing an area copied during the execution of the 'copy_area' function.
 * 
 * @param width Width of the area in pixels
 * @param height Height of the area in pixels
 * @param row_pointers Pointer to an array of pointers, each pointing to a row of area data
 */
typedef struct Area {
    int width;
    int height;
    png_bytep *row_pointers;
} Area;

/**
 * @brief Structure representing options provided to the program.
 * 
 * @param input_file Filename of the input PNG file
 * @param output_file Filename of the output PNG file
 * @param flag_help Flag indicating if the help message should be displayed
 * @param flag_input Flag indicating if the input file has been specified
 * @param flag_output Flag indicating if the output file has been specified
 * @param flag_copy Flag indicating if the 'copy' function should be executed
 * @param flag_color_replace Flag indicating if the 'color_replace' function should be executed
 * @param flag_ornament Flag indicating if the 'ornament' function should be executed
 * @param flag_filled_rects Flag indicating if the 'filled_rects' function should be executed
 * @param flag_left_up Flag indicating if the top-left coordinate of the source area has been specified
 * @param flag_right_down Flag indicating if the bottom-right coordinate of the source area has been specified
 * @param flag_dest_left_up Flag indicating if the top-left coordinate of the destination area has been specified
 * @param flag_old_color Flag indicating if the old color for color replacement has been specified
 * @param flag_new_color Flag indicating if the new color for color replacement has been specified
 * @param flag_pattern Flag indicating if the pattern for ornamentation has been specified
 * @param flag_color Flag indicating if the color for ornamentation or filled rectangles has been specified
 * @param flag_thickness Flag indicating if the thickness for ornamentation or filled rectangles has been specified
 * @param flag_count Flag indicating if the count for ornamentation has been specified
 * @param flag_border_color Flag indicating if the border color for filled rectangles has been specified
 * @param flag_info Flag indicating if detailed information about the input PNG file should be printed
 * @param left_up_value Value of the top-left coordinate of the source area
 * @param right_down_value Value of the bottom-right coordinate of the source area
 * @param dest_left_up_value Value of the top-left coordinate of the destination area
 * @param old_color_value Value of the old color for color replacement
 * @param new_color_value Value of the new color for color replacement
 * @param pattern_value Value of the pattern for ornamentation
 * @param color_value Value of the color for ornamentation or filled rectangles
 * @param thickness_value Value of the thickness for ornamentation or filled rectangles
 * @param count_value Value of the count for ornamentation
 * @param border_color_value Value of the border color for filled rectangles
 */
typedef struct {
    char* input_file;
    char* output_file;
    int flag_help;
    int flag_input;
    int flag_output;
    int flag_copy;
    int flag_color_replace;
    int flag_ornament;
    int flag_filled_rects;
    int flag_left_up;
    int flag_right_down;
    int flag_dest_left_up;
    int flag_old_color;
    int flag_new_color;
    int flag_pattern;
    int flag_color;
    int flag_thickness;
    int flag_count;
    int flag_border_color;
    int flag_info;
    char* left_up_value;
    char* right_down_value;
    char* dest_left_up_value;
    char* old_color_value;
    char* new_color_value;
    char* pattern_value;
    char* color_value;
    char* thickness_value;
    char* count_value;
    char* border_color_value;
} Options;

#endif