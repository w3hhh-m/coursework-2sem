#include "errors.h"
#include "structures.h"
#include "preparation_handler.h"

/**
 * @brief Draws a single pixel with the specified color values.
 * 
 * @param ptr Pointer to the pixel in the image.
 * @param color_values Array containing the RGB values of the pixel color.
 */
void draw_pixel(png_bytep ptr, int* color_values) {
    ptr[0] = color_values[0];
    ptr[1] = color_values[1];
    ptr[2] = color_values[2];
}

/**
 * @brief Draws a border around the specified rectangle in the image.
 * 
 * @param image Pointer to the Png structure representing the image.
 * @param x1 The x-coordinate of the top-left corner of the rectangle.
 * @param y1 The y-coordinate of the top-left corner of the rectangle.
 * @param x2 The x-coordinate of the bottom-right corner of the rectangle.
 * @param y2 The y-coordinate of the bottom-right corner of the rectangle.
 * @param border_color Array containing the RGB values of the border color.
 * @param thickness String representing the thickness of the border.
 */
void draw_border(Png *image, int x1, int y1, int x2, int y2, int* border_color, char* thickness) {
    /* Convert thickness string to integer */
    int border_thickness = atoi(thickness);
    if (border_thickness <= 0) {
        printf("Error: Border thickness is not a positive integer\n");
        exit(ERR_INSUFFICIENT_ARGUMENTS);
    }

    /* Draw horizontal lines */
    for (int t = 1; t <= border_thickness; t++) {
        /* Draw upper horizontal line */
        int y = y1 - t;
        if (y >= 0 && y < image->height) {
            png_bytep row = image->row_pointers[y];
            for (int x = x1 - t; x <= x2 + t; x++) {
                if (x >= 0 && x < image->width) {
                    png_bytep px = &(row[x * 3]);
                    draw_pixel(px, border_color);
                }
            }
        }

        /* Draw lower horizontal line */
        y = y2 + t;
        if (y >= 0 && y < image->height) {
            png_bytep row = image->row_pointers[y];
            for (int x = x1 - t; x <= x2 + t; x++) {
                if (x >= 0 && x < image->width) {
                    png_bytep px = &(row[x * 3]);
                    draw_pixel(px, border_color);
                }
            }
        }
    }

    /* Draw vertical lines */
    for (int t = 1; t <= border_thickness; t++) {
        /* Draw left vertical line */
        int x = x1 - t;
        if (x >= 0 && x < image->width) {
            for (int y = y1 - t; y <= y2 + t; y++) {
                if (y >= 0 && y < image->height) {
                    png_bytep px = &(image->row_pointers[y][(x * 3)]);
                    draw_pixel(px, border_color);
                }
            }
        }

        /* Draw right vertical line */
        x = x2 + t;
        if (x >= 0 && x < image->width) {
            for (int y = y1 - t; y <= y2 + t; y++) {
                if (y >= 0 && y < image->height) {
                    png_bytep px = &(image->row_pointers[y][(x * 3)]);
                    draw_pixel(px, border_color);
                }
            }
        }
    }
}

/**
 * @brief Draws rectangle ornaments on the image.
 * 
 * @param image Pointer to the Png structure representing the image.
 * @param ornament_thickness Thickness of the ornament rectangles.
 * @param ornament_count Number of ornament rectangles to draw.
 * @param color_values Array containing the RGB values of the ornament color.
 * @param thickness String representing the thickness of the border.
 */
void rectangle_ornament(Png *image, int ornament_thickness, int ornament_count, int* color_values, char* thickness) {
    int x1, y1, x2, y2;
    x1 = ornament_thickness;
    y1 = ornament_thickness;
    x2 = image->width - ornament_thickness - 1;
    y2 = image->height - ornament_thickness - 1;
    for (int i = 0; i < ornament_count; i++){
        draw_border(image, x1, y1, x2, y2, color_values, thickness);
        x1 += ornament_thickness * 2;
        y1 += ornament_thickness * 2;
        x2 -= ornament_thickness * 2;
        y2 -= ornament_thickness * 2;

        /* Check if rectangles can fit */
        if (x1 >= x2 || y1 >= y2){
            printf("Warning: Rectangles that cannot fit will be skipped\n");
            break;
        }
    }
}

/**
 * @brief Draws a circle ornament on the image.
 * 
 * @param image Pointer to the Png structure representing the image.
 * @param color_values Array containing the RGB values of the ornament color.
 */
void circle_ornament(Png *image, int* color_values) {
    int centerX = image->width / 2;
    int centerY = image->height / 2;
    int radius = (centerX < centerY) ? centerX : centerY;
    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            double distance = sqrt(pow(x - centerX, 2) + pow(y - centerY, 2));
            if (distance <= radius) {
                continue;
            }
            png_bytep ptr = &(image->row_pointers[y][x * 3]);
            draw_pixel(ptr, color_values);
        }
    }
}

/**
 * @brief Draws semicircle ornaments on the image.
 * 
 * @param image Pointer to the Png structure representing the image.
 * @param ornament_thickness Thickness of the semicircle ornaments.
 * @param ornament_count Number of semicircle ornaments to draw.
 * @param color_values Array containing the RGB values of the ornament color.
 */
void semicircles_ornament(Png *image, int ornament_thickness, int ornament_count, int* color_values) {
    int radiusX = ceil((double)(image->width - ornament_count * ornament_thickness) / (2 * ornament_count));
    int radiusY = ceil((double)(image->height - ornament_count * ornament_thickness) / (2 * ornament_count));
    int centerX = radiusX + ornament_thickness;
    for (int i = 0; i < ornament_count; i++){
        for (int x = centerX - radiusX - ornament_thickness; x < centerX + radiusX + ornament_thickness && x < image->width && x >= 0; x++){
            /* Draw upper semicircles */
            for (int y = 0; y < radiusX + ornament_thickness && y < image->height && y >= 0; y++){
                int centerY = 0;
                double distance = sqrt(pow(x - centerX, 2) + pow(y - centerY, 2));
                if (distance < radiusX || distance > radiusX + ornament_thickness) {
                    continue;
                }
                png_bytep ptr = &(image->row_pointers[y][x * 3]);
                draw_pixel(ptr, color_values);
            }
            /* Draw lower semicircles */
            for (int y = image->height - 1; y > image->height - 1 - radiusX - ornament_thickness && y >= 0 && y < image->height; y--){
                int centerY = image->height - 1;
                double distance = sqrt(pow(x - centerX, 2) + pow(y - centerY, 2));
                if (distance < radiusX || distance > radiusX + ornament_thickness) {
                    continue;
                }
                png_bytep ptr = &(image->row_pointers[y][x * 3]);
                draw_pixel(ptr, color_values);
            }
        }
        centerX += 2 * radiusX + ornament_thickness;
    }

    int centerY = radiusY + ornament_thickness;
    for (int i = 0; i < ornament_count; i++){
        for (int y = centerY - radiusY - ornament_thickness; y < centerY + radiusY + ornament_thickness && y < image->height && y >= 0; y++){
            /* Draw left semicircles */
            for (int x = 0; x < radiusY + ornament_thickness && x < image->width && x >= 0; x++){
                int centerX = 0;
                double distance = sqrt(pow(x - centerX, 2) + pow(y - centerY, 2));
                if (distance < radiusY || distance > radiusY + ornament_thickness) {
                    continue;
                }
                png_bytep ptr = &(image->row_pointers[y][x * 3]);
                draw_pixel(ptr, color_values);
            }
            /* Draw right semicircles */
            for (int x = image->width - 1; x > image->width - 1 - radiusY - ornament_thickness && x < image->width && x >= 0; x--){
                int centerX = image->width - 1;
                double distance = sqrt(pow(x - centerX, 2) + pow(y - centerY, 2));
                if (distance < radiusY || distance > radiusY + ornament_thickness) {
                    continue;
                }
                png_bytep ptr = &(image->row_pointers[y][x * 3]);
                draw_pixel(ptr, color_values);
            }
        }
        centerY += 2 * radiusY + ornament_thickness;
    }
}
