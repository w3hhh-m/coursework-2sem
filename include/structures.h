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
 */
typedef struct Png {
    int width; /**< Width of the image in pixels */
    int height; /**< Height of the image in pixels */
    png_byte color_type; /**< Color type of the image (e.g., RGB, Grayscale) */
    png_byte bit_depth; /**< Bit depth of the image */
    png_structp png_ptr; /**< Pointer to the libpng structure for reading/writing PNG data */
    png_infop info_ptr; /**< Pointer to the libpng structure for storing PNG information */
    int number_of_passes; /**< Number of passes required for interlacing (typically used for progressive rendering) */
    png_bytep *row_pointers; /**< Pointer to an array of pointers, each pointing to a row of image data */
} Png;

/**
 * @brief Structure representing an area copied during the execution of the 'copy_area' function.
 */
typedef struct Area {
    int width; /**< Width of the area in pixels */
    int height; /**< Height of the area in pixels */
    png_bytep *row_pointers; /**< Pointer to an array of pointers, each pointing to a row of area data */
} Area;

/**
 * @brief Structure representing options provided to the program.
 */
typedef struct {
    char* input_file; /**< Filename of the input PNG file */
    char* output_file; /**< Filename of the output PNG file */
    int flag_help; /**< Flag indicating if the help message should be displayed */
    int flag_input; /**< Flag indicating if the input file has been specified */
    int flag_output; /**< Flag indicating if the output file has been specified */
    int flag_copy; /**< Flag indicating if the 'copy' function should be executed */
    int flag_color_replace; /**< Flag indicating if the 'color_replace' function should be executed */
    int flag_ornament; /**< Flag indicating if the 'ornament' function should be executed */
    int flag_filled_rects; /**< Flag indicating if the 'filled_rects' function should be executed */
    int flag_left_up; /**< Flag indicating if the top-left coordinate of the source area has been specified */
    int flag_right_down; /**< Flag indicating if the bottom-right coordinate of the source area has been specified */
    int flag_dest_left_up; /**< Flag indicating if the top-left coordinate of the destination area has been specified */
    int flag_old_color; /**< Flag indicating if the old color for color replacement has been specified */
    int flag_new_color; /**< Flag indicating if the new color for color replacement has been specified */
    int flag_pattern; /**< Flag indicating if the pattern for ornamentation has been specified */
    int flag_color; /**< Flag indicating if the color for ornamentation or filled rectangles has been specified */
    int flag_thickness; /**< Flag indicating if the thickness for ornamentation or filled rectangles has been specified */
    int flag_count; /**< Flag indicating if the count for ornamentation has been specified */
    int flag_border_color; /**< Flag indicating if the border color for filled rectangles has been specified */
    int flag_info; /**< Flag indicating if detailed information about the input PNG file should be printed */
    char* left_up_value; /**< Value of the top-left coordinate of the source area */
    char* right_down_value; /**< Value of the bottom-right coordinate of the source area */
    char* dest_left_up_value; /**< Value of the top-left coordinate of the destination area */
    char* old_color_value; /**< Value of the old color for color replacement */
    char* new_color_value; /**< Value of the new color for color replacement */
    char* pattern_value; /**< Value of the pattern for ornamentation */
    char* color_value; /**< Value of the color for ornamentation or filled rectangles */
    char* thickness_value; /**< Value of the thickness for ornamentation or filled rectangles */
    char* count_value; /**< Value of the count for ornamentation */
    char* border_color_value; /**< Value of the border color for filled rectangles */
} Options;

#endif
