#include "errors.h"
#include "structures.h"
#include "drawing_handler.h"
#include "preparation_handler.h"

/**
 * @brief Prints the help message explaining the usage of the program and its options.
 * 
 * This function does not return a value.
 * 
 * The help message includes:
 * - Information about the course work and its creator.
 * - Usage syntax.
 * - Description of available options, including short and long forms, along with their corresponding explanations.
 */
void print_help() {
    printf("Course work for option 5.16, created by Matvei Kolesnichenko.\n");
    printf("Usage: ./program [OPTIONS] [input_file]\n\n");
    printf("Options:\n");
    printf("  -h, --help                Display this help message\n");
    printf("  --info                    Print detailed information about the input PNG file\n");
    printf("  -i, --input <filename>    Specify the input PNG file\n");
    printf("  -o, --output <filename>   Specify the output PNG file (default: out.png)\n\n");
    printf("  --copy                    Copy a specified region of the image\n");
    printf("  --left_up <x.y>           Specify the coordinates of the top left corner of the source area\n");
    printf("  --right_down <x.y>        Specify the coordinates of the bottom right corner of the source area\n");
    printf("  --dest_left_up <x.y>      Specify the coordinates of the top left corner of the destination area\n\n");
    printf("  --color_replace           Replace all pixels of a specified color with another color\n");
    printf("  --old_color <r.g.b>       Specify the color to be replaced\n");
    printf("  --new_color <r.g.b>       Specify the color to replace with\n\n");
    printf("  --ornament                Create a patterned frame\n");
    printf("  --pattern <rectangle|circle|semicircles>\n");
    printf("                            Specify the pattern of the frame\n");
    printf("  --color <r.g.b>           Specify the color of the frame\n");
    printf("  --thickness <value>       Specify the thickness of the frame\n");
    printf("  --count <value>           Specify the number of repetitions of the pattern\n\n");
    printf("  --filled_rects            Find all filled rectangles of a specified color and draw an outline\n");
    printf("  --color <r.g.b>           Specify the color of the frame\n");
    printf("  --border_color <r.g.b>    Specify the color of the outline\n");
    printf("  --thickness <value>       Specify the thickness of the outline\n");
}

/**
 * @brief Prints information about a PNG image.
 * 
 * @param image A pointer to the Png structure containing information about the PNG image.
 * 
 * This function does not return a value.
 * 
 * @note This function prints various details about the PNG image, including its width, height, color type, bit depth, and number of passes.
 * - The color type is printed as a string representation.
 * - Bit depth indicates the number of bits per sample or per channel in the image.
 * - Number of passes refers to the number of passes required for interlaced PNG images.
 */
void print_png_info(Png *image) {
    printf("Image Width: %d\n", image->width);
    printf("Image Height: %d\n", image->height);

    printf("Color Type: ");
    switch (image->color_type) {
        case PNG_COLOR_TYPE_GRAY:
            printf("Grayscale\n");
            break;
        case PNG_COLOR_TYPE_RGB:
            printf("RGB\n");
            break;
        case PNG_COLOR_TYPE_PALETTE:
            printf("Palette\n");
            break;
        case PNG_COLOR_TYPE_GRAY_ALPHA:
            printf("Grayscale with Alpha\n");
            break;
        case PNG_COLOR_TYPE_RGBA:
            printf("RGB with Alpha\n");
            break;
        default:
            printf("Unknown\n");
            break;
    }

    printf("Bit Depth: %d\n", image->bit_depth);
    printf("Number of passes: %d\n", image->number_of_passes);
}

/**
 * @brief Replaces all pixels of the specified old color with the new color.
 * 
 * @param image A pointer to the Png structure representing the image.
 * @param old_color A string representing the old color in the format "R,G,B".
 * @param new_color A string representing the new color in the format "R,G,B".
 * 
 * This function does not return a value.
 */
void color_replace(Png *image, char* old_color, char* new_color) {
    int x, y;

    /* Getting colors as arrays */
    int* old_color_values = process_color(old_color);
    int* new_color_values = process_color(new_color);

    /* Error handling */
    if (!old_color_values || !new_color_values) {
        printf("Error: Can not process color\n");
        exit(ERR_INSUFFICIENT_ARGUMENTS);
    }

    /* Nested loop to iterate through each pixel of the image */
    for (y = 0; y < image->height; y++) {
        png_bytep row = image->row_pointers[y];
        for (x = 0; x < image->width; x++) {
            png_bytep ptr = &(row[x * 3]); // Getting pixel
            /* Changing by colors */
            if ((ptr[0] == old_color_values[0]) && (ptr[1] == old_color_values[1]) && (ptr[2] == old_color_values[2])) {
                draw_pixel(ptr, new_color_values);
            }
        }
    }
}

/**
 * @brief Copies the specified area from the original image to a new structure, then copies it back to the original image at a different location.
 * 
 * @param image A pointer to the Png structure representing the original image.
 * @param left_up A string containing the coordinates of the top-left corner of the area to be copied in the format "x,y".
 * @param right_down A string containing the coordinates of the bottom-right corner of the area to be copied in the format "x,y".
 * @param dest_left_up A string containing the coordinates of the top-left corner of the destination location in the original image for the copied area.
 * 
 * This function does not return a value.
 */
void copy_area(Png *image, char* left_up, char* right_down, char* dest_left_up) {
    int x, y;
    int area_x = 0, area_y = 0;

    /* Getting coordinates as arrays */
    int* left_up_coordinates = process_coordinates(left_up);
    int* right_down_coordinates = process_coordinates(right_down);
    int* dest_left_up_coordinates = process_coordinates(dest_left_up);

    /* Error handling */
    if (!left_up_coordinates || !right_down_coordinates || !dest_left_up_coordinates){
        printf("Error: Can not process coordinates\n");
        exit(ERR_INSUFFICIENT_ARGUMENTS);
    }

    /* Making structure and its information */
    Area* copied_area = malloc(sizeof(Area));
    if (copied_area == NULL) {
        printf("Error: Can not allocate memory for copied area\n");
        exit(ERR_MEMORY_ALLOCATION_FAILURE);
    }
    copied_area->height = abs(right_down_coordinates[1] - left_up_coordinates[1] + 1);
    copied_area->width = abs(right_down_coordinates[0] - left_up_coordinates[0] + 1);
    copied_area->row_pointers = malloc(sizeof(png_bytep) * copied_area->height);
    if (copied_area->row_pointers == NULL) {
        printf("Error: Can not allocate memory for copied area row_pointers\n");
        exit(ERR_MEMORY_ALLOCATION_FAILURE);
    }

    /* Allocating memory for each pixel */
    for (y = 0; y < copied_area->height; y++) {
        copied_area->row_pointers[y] = malloc(sizeof(png_byte) * copied_area->width * 3);
        if (copied_area->row_pointers[y] == NULL) {
            printf("Error: Can not allocate memory for copied area pixel\n");
            exit(ERR_MEMORY_ALLOCATION_FAILURE);
        }
    }

    /* Switching left and right if needed */
    if (left_up_coordinates[0] > right_down_coordinates[0]){
        int tmp[2];
        tmp[0] = right_down_coordinates[0];
        tmp[1] = right_down_coordinates[1];
        right_down_coordinates[0] = left_up_coordinates[0];
        right_down_coordinates[1] = left_up_coordinates[1];
        left_up_coordinates[0] = tmp[0];
        left_up_coordinates[1] = tmp[1];
    }
    
    /* Copying area from original image to structure */
    for (y = left_up_coordinates[1]; y <= right_down_coordinates[1]; y++) {
        for (x = left_up_coordinates[0]; x <= right_down_coordinates[0]; x++) {
            /* Handling inappropriate coordinate */
            if (x < 0 || x >= image->width || y < 0 || y >= image->height) {
                copied_area->row_pointers[area_y][area_x*3] = 0;
                copied_area->row_pointers[area_y][area_x*3 + 1] = 0;
                copied_area->row_pointers[area_y][area_x*3 + 2] = 0;
            } else {
                /* Copying every pixel by R, G, B */
                copied_area->row_pointers[area_y][area_x * 3] = image->row_pointers[y][x * 3];
                copied_area->row_pointers[area_y][area_x * 3 + 1] = image->row_pointers[y][x * 3 + 1];
                copied_area->row_pointers[area_y][area_x * 3 + 2] = image->row_pointers[y][x * 3 + 2];
            }
            area_x++;
        }
        area_x = 0;
        area_y++;
    }

    area_x = 0;
    area_y = 0;

    /* Copying area from structure back to original image */
    for (y = dest_left_up_coordinates[1]; y < dest_left_up_coordinates[1] + copied_area->height; y++) {
        for (x = dest_left_up_coordinates[0]; x < dest_left_up_coordinates[0] + copied_area->width; x++) {
            /* Handling inappropriate coordinate */
            if (x < 0 || x >= image->width || y < 0 || y >= image->height) {
                continue;
            }
            if (copied_area->row_pointers[area_y][area_x * 3] != 0 && copied_area->row_pointers[area_y][area_x * 3 + 1] != 0 && copied_area->row_pointers[area_y][area_x * 3 + 2] != 0) {
                /* Copying every pixel by R, G, B */
                image->row_pointers[y][x * 3] = copied_area->row_pointers[area_y][area_x * 3];
                image->row_pointers[y][x * 3 + 1] = copied_area->row_pointers[area_y][area_x * 3 + 1];
                image->row_pointers[y][x * 3 + 2] = copied_area->row_pointers[area_y][area_x * 3 + 2];
            }
            area_x++;
        }
        area_x = 0;
        area_y++;
    }
}

/**
 * @brief Finds all filled rectangles in the image and draws borders around them.
 * 
 * @param image A pointer to the Png structure representing the image.
 * @param string_color A string representing the color of the filled rectangles in the format "rrr.ggg.bbb".
 * @param string_border_color A string representing the color of the border in the format "rrr.ggg.bbb".
 * @param thickness A string representing the thickness of the border.
 * 
 * This function does not return a value.
 */
void filled_rects(Png *image, char* string_color, char* string_border_color, char* thickness) {
    /* Allocate memory for tracking visited pixels */
    int** visited = malloc(sizeof(int*) * image->height);
    if (visited == NULL) {
        printf("Error: Can not allocate memory for rectangles visited pixels\n");
        exit(ERR_MEMORY_ALLOCATION_FAILURE);
    }
    for (int i = 0; i < image->height; i++) {
        visited[i] = malloc(sizeof(int) * image->width);
        if (visited[i] == NULL) {
            printf("Error: Can not allocate memory for rectangles visited pixels\n");
            exit(ERR_MEMORY_ALLOCATION_FAILURE);
        }
        for (int j = 0; j < image->width; j++) {
            visited[i][j] = 0;
        }
    }

    /* Getting color as array */
    int* color_values = process_color(string_color);

    /* Error handling: Cannot process rectangle color */
    if (!color_values) {
        printf("Error: Can not process rectangle color\n");
        exit(ERR_INSUFFICIENT_ARGUMENTS);
    }

    /* Processing border color */
    int* border_color = process_color(string_border_color);

    /* Error handling: Cannot process border color */
    if (!border_color) {
        printf("Error: Can not process border color\n");
        exit(ERR_INSUFFICIENT_ARGUMENTS);
    }

    /* Nested loop to iterate through all pixels */
    for (int y = 0; y < image->height; y++) {
        png_bytep row = image->row_pointers[y];
        for (int x = 0; x < image->width; x++) {
            /* Skip visited or other color pixels */
            if (visited[y][x] || !(row[x * 3] == color_values[0] && row[x * 3 + 1] == color_values[1] && row[x * 3 + 2] == color_values[2])) {
                continue;
            }
            int start_x = x;
            int start_y = y;
            int end_x = start_x;
            int end_y = start_y;

            /* Finding end horizontally */
            while (end_x < image->width && visited[end_y][end_x] == 0 && row[end_x * 3] == color_values[0] && row[end_x * 3 + 1] == color_values[1] && row[end_x * 3 + 2] == color_values[2]) {
                end_x++;
            }

            /* Finding end vertically */
            while (end_y < image->height) {
                int found_different_color = 0;
                for (int i = start_x; i < end_x; i++) {
                    if (visited[end_y][i] || !(image->row_pointers[end_y][i * 3] == color_values[0] && image->row_pointers[end_y][i * 3 + 1] == color_values[1] && image->row_pointers[end_y][i * 3 + 2] == color_values[2])) {
                        found_different_color = 1;
                        break;
                    }
                }
                if (found_different_color) {
                    break;
                }
                end_y++;
            }

            draw_border(image, start_x, start_y, end_x - 1, end_y - 1, border_color, thickness);

            /* Marking visited pixels */
            for (int i = start_y; i < end_y; i++) {
                for (int j = start_x; j < end_x; j++) {
                    visited[i][j] = 1;
                }
            }
        }
    }
}

/**
 * @brief Draws an ornament pattern on the given image.
 * 
 * @param image A pointer to the Png structure representing the image.
 * @param pattern A string specifying the type of ornament pattern ("rectangle", "circle", "semicircles").
 * @param string_color A string representing the color of the ornament in the format "rrr.ggg.bbb".
 * @param thickness A string representing the thickness of the ornament.
 * @param count A string representing the number of ornaments to be drawn.
 * 
 * This function does not return a value.
 */
void ornament(Png *image, char* pattern, char* string_color, char* thickness, char* count) {
    /* Getting color as array */
    int* color_values = process_color(string_color);

    /* Error handling: Cannot process ornament color */
    if (!color_values) {
        printf("Error: Can not process ornament color\n");
        exit(ERR_INSUFFICIENT_ARGUMENTS);
    }

    /* Getting thickness as integer */
    int ornament_thickness = atoi(thickness);

    /* Error handling: Ornament thickness is not a positive integer */
    if (ornament_thickness <= 0) {
        printf("Error: Ornament thickness is not a positive integer\n");
        exit(ERR_INSUFFICIENT_ARGUMENTS);
    }

    /* Getting count as integer */
    int ornament_count = atoi(count);

    /* Error handling: Ornament count is not a positive integer */
    if (ornament_count <= 0) {
        printf("Error: Ornament count is not a positive integer\n");
        exit(ERR_INSUFFICIENT_ARGUMENTS);
    }

    /* Rectangle pattern */
    if (strcmp(pattern, "rectangle") == 0){
        rectangle_ornament(image, ornament_thickness, ornament_count, color_values, thickness);

    /* Circle pattern */
    } else if (strcmp(pattern, "circle") == 0) {
        circle_ornament(image, color_values);

    /* Semicircles pattern */
    } else if (strcmp(pattern, "semicircles") == 0){
        semicircles_ornament(image, ornament_thickness, ornament_count, color_values);
    }
    /* Unknown pattern */
    else {
        printf("Error: Unknown pattern\n");
        exit(ERR_INSUFFICIENT_ARGUMENTS);
    }
}

/**
 * @brief Handles task switching based on provided options.
 * 
 * @param options Options structure containing flags and values for various tasks.
 * @param image Pointer to the Image structure representing the image.
 * 
 * This function does not return a value.
 */
void task_switcher(Options options, Png *image) {
    if (options.flag_info) {
        print_png_info(image);
        exit(EXIT_SUCCESS);
    }

    if (options.flag_copy) {
        copy_area(image, options.left_up_value, options.right_down_value, options.dest_left_up_value);
    }

    if (options.flag_color_replace) {
        color_replace(image, options.old_color_value, options.new_color_value);
    }

    if (options.flag_ornament) {
        ornament(image, options.pattern_value, options.color_value, options.thickness_value, options.count_value);
    }

    if (options.flag_filled_rects) {
        filled_rects(image, options.color_value, options.border_color_value, options.thickness_value);
    }
}
