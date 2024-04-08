#include "structures_and_errors.h"
#include "assistive_functions.h"
#include <math.h>
/*
Replaces all pixels of the specified old color with the new color.

Parameters:
- image: A pointer to the Png structure representing the image.
- old_color: A string representing the old color in the format "R,G,B".
- new_color: A string representing the new color in the format "R,G,B".

Returns:
This function does not return a value.
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
                ptr[0] = new_color_values[0];
                ptr[1] = new_color_values[1];
                ptr[2] = new_color_values[2];
            }
        }
    }
}

/*
Copies the specified area from the original image to a new structure, then copies it back to the original image at a different location.

Parameters:
- image: A pointer to the Png structure representing the original image.
- left_up: A string containing the coordinates of the top-left corner of the area to be copied in the format "x,y".
- right_down: A string containing the coordinates of the bottom-right corner of the area to be copied in the format "x,y".
- dest_left_up: A string containing the coordinates of the top-left corner of the destination location in the original image for the copied area.

Returns:
This function does not return a value.
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
    copied_area->height = right_down_coordinates[1] - left_up_coordinates[1] + 1;
    copied_area->width = right_down_coordinates[0] - left_up_coordinates[0] + 1;
    if (copied_area->height <= 0 || copied_area->width <= 0) {
        printf("Error: Inappropriate area coordinates\n");
        exit(ERR_INSUFFICIENT_ARGUMENTS);
    }
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

    /* Copying area from original image to structure */
    for (y = left_up_coordinates[1]; y <= right_down_coordinates[1]; y++) {
        for (x = left_up_coordinates[0]; x <= right_down_coordinates[0]; x++) {
            /* Handling inappropriate coordinate */
            if (x < 0 || x >= image->width || y < 0 || y >= image->height) {
                copied_area->row_pointers[area_y][area_x * 3] = 255;
                copied_area->row_pointers[area_y][area_x * 3 + 1] = 255;
                copied_area->row_pointers[area_y][area_x * 3 + 2] = 255;
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
            if (copied_area->row_pointers[area_y][area_x * 3] != 255 && copied_area->row_pointers[area_y][area_x * 3 + 1] != 255 && copied_area->row_pointers[area_y][area_x * 3 + 2] != 255) {
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

    /* Freeing memory from area structure */
    for (y = 0; y < copied_area->height; y++) {
        free(copied_area->row_pointers[y]);
    }
    free(copied_area->row_pointers);
    free(copied_area);
}

/*
Finds all filled rectangles in the image and draws borders around them.

Parameters:
- image: A pointer to the Png structure representing the image.
- string_color: A string representing the color of the filled rectangles in the format "rrr.ggg.bbb".
- string_border_color: A string representing the color of the border in the format "rrr.ggg.bbb".
- thickness: A string representing the thickness of the border.

Returns:
This function does not return a value.
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
    
    /* Initialize counter for rectangles */
    int counter = 0;

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
            
            Point start_point = {x, y};
            Point end_point = start_point;
            
            /* Finding end horizontally */
            while (end_point.x < image->width && visited[end_point.y][end_point.x] == 0 && row[end_point.x * 3] == color_values[0] && row[end_point.x * 3 + 1] == color_values[1] && row[end_point.x * 3 + 2] == color_values[2]) {
                end_point.x++;
            }

            /* Finding end vertically */
            while (end_point.y < image->height) {
                int found_different_color = 0;
                for (int i = start_point.x; i < end_point.x; i++) {
                    if (visited[end_point.y][i] || !(image->row_pointers[end_point.y][i * 3] == color_values[0] && image->row_pointers[end_point.y][i * 3 + 1] == color_values[1] && image->row_pointers[end_point.y][i * 3 + 2] == color_values[2])) {
                        found_different_color = 1;
                        break;
                    }
                }
                if (found_different_color) {
                    break;
                }
                end_point.y++;
            }
            
            /* Making rectangle */
            Rectangle* rect = malloc(sizeof(Rectangle));
            rect->top_left.x = start_point.x;
            rect->top_left.y = start_point.y;
            rect->bottom_right.x = end_point.x - 1;
            rect->bottom_right.y = end_point.y - 1;
            draw_border(image, rect->top_left.x, rect->top_left.y, rect->bottom_right.x, rect->bottom_right.y, border_color, thickness);

            /* Marking visited pixels */
            for (int i = start_point.y; i < end_point.y; i++) {
                for (int j = start_point.x; j < end_point.x; j++) {
                    visited[i][j] = 1;
                }
            }
        }
    }
}

/*
Draws an ornament pattern on the given image.

Parameters:
- image: A pointer to the Png structure representing the image.
- pattern: A string specifying the type of ornament pattern ("rectangle", "circle", "semicircles").
- string_color: A string representing the color of the ornament in the format "rrr.ggg.bbb".
- thickness: A string representing the thickness of the ornament.
- count: A string representing the number of ornaments to be drawn.

Returns:
This function does not return a value.
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
        int x1, y1, x2, y2;
        x1 = ornament_thickness;
        y1 = ornament_thickness;
        x2 = image->width-ornament_thickness-1;
        y2 = image->height-ornament_thickness-1;
        for (int i = 0; i < ornament_count; i++){
            draw_border(image, x1, y1, x2, y2, color_values, thickness);
            x1 += ornament_thickness*2;
            y1 += ornament_thickness*2;
            x2 -= ornament_thickness*2;
            y2 -= ornament_thickness*2;

            /* Error handling: Too many rectangles */
            if (x1 >= x2 || y1 >= y2){
                printf("Warning: Rectangles that cannot fit will be skipped\n");
                break;
            }
        }

    /* Circle pattern */
    } else if (strcmp(pattern, "circle") == 0) {
        int centerX = image->width / 2;
        int centerY = image->height / 2;
        int radius = (centerX < centerY) ? centerX : centerY;
        for (int y = 0; y < image->height; y++) {
            for (int x = 0; x < image->width; x++) {
                double distance = sqrt(pow(x - centerX, 2) + pow(y - centerY, 2));
                if (distance <= radius) {
                    continue;
                }
                image->row_pointers[y][3 * x] = color_values[0];
                image->row_pointers[y][3 * x + 1] = color_values[1];
                image->row_pointers[y][3 * x + 2] = color_values[2];
            }
        }
    
    /* Semicircles pattern */
    } else if (strcmp(pattern, "semicircles") == 0){
        int radiusX = ceil((double)(image->width-ornament_count*ornament_thickness)/(2*ornament_count));
        int radiusY = ceil((double)(image->height-ornament_count*ornament_thickness)/(2*ornament_count));
        int centerX = radiusX+ornament_thickness;
        for (int i = 0; i < ornament_count; i++){
            for (int x = centerX-radiusX-ornament_thickness; x < centerX+radiusX+ornament_thickness && x < image->width && x >= 0; x++){
                /* Up semicircles */
                for (int y = 0; y < radiusX+ornament_thickness && y < image->height && y >= 0; y++){
                    int centerY = 0;
                    double distance = sqrt(pow(x - centerX, 2) + pow(y - centerY, 2));
                    if (distance < radiusX || distance > radiusX + ornament_thickness) {
                        continue;
                    }
                    image->row_pointers[y][3 * x] = color_values[0];
                    image->row_pointers[y][3 * x + 1] = color_values[1];
                    image->row_pointers[y][3 * x + 2] = color_values[2];
                }
                /* Down semicircles */
                for (int y = image->height-1; y > image->height-1-radiusX-ornament_thickness && y >= 0 && y < image->height; y--){
                    int centerY = image->height-1;
                    double distance = sqrt(pow(x - centerX, 2) + pow(y - centerY, 2));
                    if (distance < radiusX || distance > radiusX + ornament_thickness) {
                        continue;
                    }
                    image->row_pointers[y][3 * x] = color_values[0];
                    image->row_pointers[y][3 * x + 1] = color_values[1];
                    image->row_pointers[y][3 * x + 2] = color_values[2];
                }
            }
            centerX += 2 * radiusX + ornament_thickness;
        }

        int centerY = radiusY + ornament_thickness;
        for (int i = 0; i < ornament_count; i++){
            for (int y = centerY - radiusY - ornament_thickness; y < centerY+radiusY+ornament_thickness && y < image->height && y >= 0; y++){
                /* Left semicircles */
                for (int x = 0; x < radiusY+ornament_thickness && x < image->width && x >= 0; x++){
                    int centerX = 0;
                    double distance = sqrt(pow(x - centerX, 2) + pow(y - centerY, 2));
                    if (distance < radiusY || distance > radiusY + ornament_thickness) {
                        continue;
                    }
                    image->row_pointers[y][3 * x] = color_values[0];
                    image->row_pointers[y][3 * x + 1] = color_values[1];
                    image->row_pointers[y][3 * x + 2] = color_values[2];
                }
                /* Right semicircles */
                for (int x = image->width-1; x > image->width-1-radiusY-ornament_thickness && x < image->width && x >= 0; x--){
                    int centerX = image->width-1;
                    double distance = sqrt(pow(x - centerX, 2) + pow(y - centerY, 2));
                    if (distance < radiusY || distance > radiusY + ornament_thickness) {
                        continue;
                    }
                    image->row_pointers[y][3 * x] = color_values[0];
                    image->row_pointers[y][3 * x + 1] = color_values[1];
                    image->row_pointers[y][3 * x + 2] = color_values[2];
                }
            }
            centerY += 2 * radiusY + ornament_thickness;
        }
    }
    /* Unknown pattern */
    else {
        printf("Error: Unknown pattern\n");
        exit(ERR_INSUFFICIENT_ARGUMENTS);
    }
}