#include "structures_and_errors.h"
#include "assistive_functions.h"
#include "file_functions.h"
#include "core_functions.h"

int main(int argc, char *argv[]) {
    /* Initialize options struct with default values. */
    Options options = {NULL};
    /* Set default output file name. */
    options.output_file = "out.png";
    /* Parse command-line arguments. */
    handle_arguments(argc, argv, &options);

    /* Initialize Png structure to hold information about the input PNG file. */
    Png image;
    /* Read the input PNG file. */
    read_png_file(options.input_file, &image);

    /* If the --info flag is specified, print detailed information about the input PNG file and exit. */
    if (options.flag_info) {
        print_png_info(&image);
        exit(EXIT_SUCCESS);
    }

    /* If the --copy flag is specified, copy a specified area of the image. */
    if (options.flag_copy) {
        copy_area(&image, options.left_up_value, options.right_down_value, options.dest_left_up_value);
    }

    /* If the --color_replace flag is specified, replace all pixels of a specified color with another color. */
    if (options.flag_color_replace) {
        color_replace(&image, options.old_color_value, options.new_color_value);
    }

    /* If the --ornament flag is specified, create a patterned frame around the image. */
    if (options.flag_ornament) {
        ornament(&image, options.pattern_value, options.color_value, options.thickness_value, options.count_value);
    }

    /* If the --filled_rects flag is specified, find all filled rectangles of a specified color and draw an outline. */
    if (options.flag_filled_rects) {
        filled_rects(&image, options.color_value, options.border_color_value, options.thickness_value);
    }

    /* Write the modified image to the output PNG file. */
    write_png_file(options.output_file, &image);

    return 0;
}
