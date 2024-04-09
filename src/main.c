#include "structures.h"
#include "task_handler.h"
#include "file_handler.h"
#include "preparation_handler.h"

/**
 * @brief Main function to handle command-line arguments and process image tasks.
 * 
 * @param argc: The number of command-line arguments.
 * @param argv: An array of strings containing the command-line arguments.
 *
 * @return int An integer representing the exit status of the program.
 */
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
    /* Process tasks based on the provided options. */
    task_switcher(options, &image);
    /* Write the modified image to the output PNG file. */
    write_png_file(options.output_file, &image);

    return 0;
}
