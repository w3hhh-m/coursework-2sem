#include <png.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <math.h>

/* Error code indicating that the specified file could not be found. */
#define ERR_FILE_NOT_FOUND 40

/* Error code indicating an error occurred while reading the file. */
#define ERR_FILE_READ_ERROR 41

/* Error code indicating an error occurred while writing to the file. */
#define ERR_FILE_WRITE_ERROR 42

/* Error code indicating an error occurred while closing the file. */
#define ERR_FILE_CLOSE_ERROR 43

/* Error code indicating that insufficient command-line arguments were provided. */
#define ERR_INSUFFICIENT_ARGUMENTS 45

/* Error code indicating a failure in memory allocation. */
#define ERR_MEMORY_ALLOCATION_FAILURE 46

/*
Structure representing a PNG image.
*/
typedef struct Png {
    int width;                  // Width of the image in pixels
    int height;                 // Height of the image in pixels
    png_byte color_type;        // Color type of the image (e.g., RGB, Grayscale)
    png_byte bit_depth;         // Bit depth of the image
    png_structp png_ptr;        // Pointer to the libpng structure for reading/writing PNG data
    png_infop info_ptr;         // Pointer to the libpng structure for storing PNG information
    int number_of_passes;       // Number of passes required for interlacing (typically used for progressive rendering)
    png_bytep *row_pointers;   // Pointer to an array of pointers, each pointing to a row of image data
} Png;

/*
Structure representing an area copied during the execution of the 'copy_area' function.
*/
typedef struct Area {
    int width;                  // Width of the area in pixels
    int height;                 // Height of the area in pixels
    png_bytep *row_pointers;   // Pointer to an array of pointers, each pointing to a row of image data
} Area;

/*
Structure representing options provided to the program.
*/
typedef struct {
    char* input_file;           // Filename of the input PNG file
    char* output_file;          // Filename of the output PNG file
    int flag_help;              // Flag indicating if the help message should be displayed
    int flag_input;             // Flag indicating if the input file has been specified
    int flag_output;            // Flag indicating if the output file has been specified
    int flag_copy;              // Flag indicating if the 'copy' function should be executed
    int flag_color_replace;     // Flag indicating if the 'color_replace' function should be executed
    int flag_ornament;          // Flag indicating if the 'ornament' function should be executed
    int flag_filled_rects;      // Flag indicating if the 'filled_rects' function should be executed
    int flag_left_up;           // Flag indicating if the top-left coordinate of the source area has been specified
    int flag_right_down;        // Flag indicating if the bottom-right coordinate of the source area has been specified
    int flag_dest_left_up;      // Flag indicating if the top-left coordinate of the destination area has been specified
    int flag_old_color;         // Flag indicating if the old color for color replacement has been specified
    int flag_new_color;         // Flag indicating if the new color for color replacement has been specified
    int flag_pattern;           // Flag indicating if the pattern for ornamentation has been specified
    int flag_color;             // Flag indicating if the color for ornamentation or filled rectangles has been specified
    int flag_thickness;         // Flag indicating if the thickness for ornamentation or filled rectangles has been specified
    int flag_count;             // Flag indicating if the count for ornamentation has been specified
    int flag_border_color;      // Flag indicating if the border color for filled rectangles has been specified
    int flag_info;              // Flag indicating if detailed information about the input PNG file should be printed
    char* left_up_value;        // Value of the top-left coordinate of the source area
    char* right_down_value;     // Value of the bottom-right coordinate of the source area
    char* dest_left_up_value;   // Value of the top-left coordinate of the destination area
    char* old_color_value;      // Value of the old color for color replacement
    char* new_color_value;      // Value of the new color for color replacement
    char* pattern_value;        // Value of the pattern for ornamentation
    char* color_value;          // Value of the color for ornamentation or filled rectangles
    char* thickness_value;      // Value of the thickness for ornamentation or filled rectangles
    char* count_value;          // Value of the count for ornamentation
    char* border_color_value;   // Value of the border color for filled rectangles
} Options;

/*
Structure representing a point with coordinates (x, y).
*/
typedef struct {
    int x;  // x-coordinate of the point
    int y;  // y-coordinate of the point
} Point;

/*
Structure representing a rectangle defined by its top-left and bottom-right points.
*/
typedef struct {
    Point top_left;         // Top-left corner of the rectangle
    Point bottom_right;     // Bottom-right corner of the rectangle
} Rectangle;

/*
Prints the help message explaining the usage of the program and its options.

This function does not return a value.

The help message includes:
- Information about the course work and its creator.
- Usage syntax.
- Description of available options, including short and long forms, along with their corresponding explanations.
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

/*
Handles command-line arguments passed to the program and populates the Options structure accordingly.

Parameters:
- argc: An integer representing the number of command-line arguments.
- argv: An array of strings containing the command-line arguments.
- options: A pointer to the Options structure where the parsed arguments will be stored.

Returns:
This function does not return a value.
*/
void handle_arguments(int argc, char *argv[], Options *options) {
    opterr = 0;

    const char* short_options = "hi:o:";
    const struct option long_options[] = {
        {"help", no_argument, NULL, 'h'},
        {"input", required_argument, NULL, 'i'},
        {"output", required_argument, NULL, 'o'},
        {"copy", no_argument, NULL, 256},
        {"color_replace", no_argument, NULL, 257},
        {"ornament", no_argument, NULL, 258},
        {"filled_rects", no_argument, NULL, 259},
        {"left_up", required_argument, NULL, 260},
        {"right_down", required_argument, NULL, 261},
        {"dest_left_up", required_argument, NULL, 262},
        {"old_color", required_argument, NULL, 263},
        {"new_color", required_argument, NULL, 264},
        {"pattern", required_argument, NULL, 265},
        {"color", required_argument, NULL, 266},
        {"thickness", required_argument, NULL, 267},
        {"count", required_argument, NULL, 268},
        {"border_color", required_argument, NULL, 269},
        {"info", no_argument, NULL, 270},
        {NULL, 0, NULL, 0}
    };

    int res;

    if (argc == 1) {
        options->flag_help = 1;
        print_help();
        exit(EXIT_SUCCESS);
        return;
    }

    while ((res = getopt_long(argc, argv, short_options, long_options, NULL)) != -1) {
        switch (res) {
            case 'h': /* -h ot --help */
                if (argc != 2) {
                    printf("Too many arguments for --help (-h)\n");
                    exit(ERR_INSUFFICIENT_ARGUMENTS);
                }
                options->flag_help = 1;
                break;
            case 'i': /* -i or --input */
                options->input_file = optarg;
                options->flag_input = 1;
                break;
            case 'o': /* -o or --output */
                options->output_file = optarg;
                options->flag_output = 1;
                break;
            case 256: /* --copy */
                if (options->flag_info || options->flag_color_replace || options->flag_ornament || options->flag_filled_rects) {
                    printf("Error: Cannot use more than one function simultaneously\n");
                    exit(ERR_INSUFFICIENT_ARGUMENTS);
                }
                options->flag_copy = 1;
                break;
            case 257: /* --color_replace */
                if (options->flag_info || options->flag_copy || options->flag_ornament || options->flag_filled_rects) {
                    printf("Error: Cannot use more than one function simultaneously\n");
                    exit(ERR_INSUFFICIENT_ARGUMENTS);
                }
                options->flag_color_replace = 1;
                break;
            case 258: /* --ornament */
                if (options->flag_info || options->flag_copy || options->flag_color_replace || options->flag_filled_rects) {
                    printf("Error: Cannot use more than one function simultaneously\n");
                    exit(ERR_INSUFFICIENT_ARGUMENTS);
                }
                options->flag_ornament = 1;
                break;
            case 259: /* --filled_rects */
                if (options->flag_info || options->flag_copy || options->flag_ornament || options->flag_color_replace) {
                    printf("Error: Cannot use more than one function simultaneously\n");
                    exit(ERR_INSUFFICIENT_ARGUMENTS);
                }
                options->flag_filled_rects = 1;
                break;
            case 260: /* --left_up */
                if (!options->flag_copy) {
                    printf("Error: --copy was not given for --left_up\n");
                    exit(ERR_INSUFFICIENT_ARGUMENTS);
                }
                options->left_up_value = optarg;
                options->flag_left_up = 1;
                break;
            case 261: /* --right_down */
                if (!options->flag_copy) {
                    printf("Error: --copy was not given for --right_down\n");
                    exit(ERR_INSUFFICIENT_ARGUMENTS);
                }
                options->right_down_value = optarg;
                options->flag_right_down = 1;
                break;
            case 262: /* --dest_left_up */
                if (!options->flag_copy) {
                    printf("Error: --copy was not given for --dest_left_up\n");
                    exit(ERR_INSUFFICIENT_ARGUMENTS);
                }
                options->dest_left_up_value = optarg;
                options->flag_dest_left_up = 1;
                break;
            case 263: /* --old_color */
                if (!options->flag_color_replace) {
                    printf("Error: --color_replace was not given for --old_color\n");
                    exit(ERR_INSUFFICIENT_ARGUMENTS);
                }
                options->flag_old_color = 1;
                options->old_color_value = optarg;
                break;
            case 264: /* --new_color */
                if (!options->flag_color_replace) {
                    printf("Error: --color_replace was not given for --new_color\n");
                    exit(ERR_INSUFFICIENT_ARGUMENTS);
                }
                options->flag_new_color = 1;
                options->new_color_value = optarg;
                break;
            case 265: /* --pattern */
                if (!options->flag_ornament) {
                    printf("Error: --ornament was not given for --pattern\n");
                    exit(ERR_INSUFFICIENT_ARGUMENTS);
                }
                options->flag_pattern = 1;
                options->pattern_value = optarg;
                break;
            case 266: /* --color */
                if (!options->flag_ornament && !options -> flag_filled_rects) {
                    printf("Error: --ornament or --filled_rects was not given for --color\n");
                    exit(ERR_INSUFFICIENT_ARGUMENTS);
                }
                options->flag_color = 1;
                options->color_value = optarg;
                break;
            case 267: /* --thickness */
                if (!options->flag_ornament && !options -> flag_filled_rects) {
                    printf("Error: --ornament or --filled_rects was not given for --thickness\n");
                    exit(ERR_INSUFFICIENT_ARGUMENTS);
                }
                options->flag_thickness = 1;
                options->thickness_value = optarg;
                break;
            case 268: /* --count */
                if (!options->flag_ornament) {
                    printf("Error: --ornament was not given for --count\n");
                    exit(ERR_INSUFFICIENT_ARGUMENTS);
                }
                options->flag_count = 1;
                options->count_value = optarg;
                break;
            case 269: /* --border_color */
                if (!options->flag_filled_rects) {
                    printf("Error: --filled_rects was not given for --border_color\n");
                    exit(ERR_INSUFFICIENT_ARGUMENTS);
                }
                options->flag_border_color = 1;
                options->border_color_value = optarg;
                break;
            case 270: /* --info */
                options->flag_info = 1;
                break;
            case '?':
            default:
                printf("Error: Unknown option or missing argument\n");
                exit(ERR_INSUFFICIENT_ARGUMENTS);
                break;
        }
    }

    /* No function provided */
    if (!options->flag_info && !options->flag_help && !options->flag_copy && !options->flag_color_replace && !options->flag_ornament && !options->flag_filled_rects) {
        printf("Error: No function provided\n");
        exit(ERR_INSUFFICIENT_ARGUMENTS);
    }

    /* -h or --help */
    if (options->flag_help) {
        print_help();
        exit(EXIT_SUCCESS);
    }

    /* Not enough arguments for --copy */
    if (options->flag_copy && (!options->flag_left_up || !options->flag_right_down || !options->flag_dest_left_up)) {
        printf("Error: Insufficient arguments for --copy\n");
        exit(ERR_INSUFFICIENT_ARGUMENTS);
    }

    /* Not enough arguments for --color_replace */
    if (options->flag_color_replace && (!options->flag_old_color || !options->flag_new_color)) {
        printf("Error: Insufficient arguments for --color_replace\n");
        exit(ERR_INSUFFICIENT_ARGUMENTS);
    }

    /* Not enough arguments for --ornament */
    if (options->flag_ornament) {
        if (!options->flag_pattern || !options->flag_color) {
            printf("Error: Insufficient arguments for --ornament\n");
            exit(ERR_INSUFFICIENT_ARGUMENTS);
        }
        if (strcmp("rectangle", options->pattern_value) == 0 || strcmp("semicircles", options->pattern_value) == 0) {
            if (!options->flag_thickness || !options->flag_count) {
                printf("Error: Insufficient arguments for --ornament\n");
                exit(ERR_INSUFFICIENT_ARGUMENTS);
            }
        }
        else if (strcmp("circle", options->pattern_value) == 0) {
            options->count_value = "1";
            options->thickness_value = "1";
        }
    }

    /* Not enough arguments for --filled_rects */
    if (options->flag_filled_rects && (!options->flag_border_color || !options->flag_color || !options->flag_thickness)) {
        printf("Error: Insufficient arguments for --filled_rects\n");
        exit(ERR_INSUFFICIENT_ARGUMENTS);
    }

    /* Getting last argument (input file) or checking too many arguments */
    if (!options->flag_input) {
        if (optind == argc - 1) {
            options->input_file = argv[argc - 1];
        } else if (optind < argc - 1) {
            printf("Error: Too many arguments\n");
            exit(ERR_INSUFFICIENT_ARGUMENTS);
        } else {
            printf("Error: No input file provided\n");
            exit(ERR_INSUFFICIENT_ARGUMENTS);
        }
    } else {
        if (optind <= argc - 1) {
            printf("Error: Too many arguments\n");
            exit(ERR_INSUFFICIENT_ARGUMENTS);
        }
    }

    /* Input file has the same name as output file */
    if (strcmp(options->input_file, options->output_file) == 0) {
        printf("Error: Input and output files cannot have the same name\n");
        exit(ERR_INSUFFICIENT_ARGUMENTS);
    }
}

/*
Reads a PNG file and stores its information and pixel data in a Png structure.

Parameters:
- file_name: A string representing the file name/path of the PNG image to be read.
- image: A pointer to the Png structure where the image data and information will be stored.

Returns:
This function does not return a value.
*/
void read_png_file(char *file_name, Png *image) {
    int x, y;
    char header[8];
    
    /* Open file */
    FILE *fp = fopen(file_name, "rb");
    if (!fp) {
        printf("Error: Can not read file %s\n", file_name);
        exit(ERR_FILE_NOT_FOUND);
    }

    /* Read first 8 bytes to verify PNG file */
    fread(header, 1, 8, fp);
    if (png_sig_cmp(header, 0, 8)) {
        printf("Error: %s probably is not a PNG file\n", file_name);
        fclose(fp);
        exit(ERR_FILE_READ_ERROR);
    }

    /* Create PNG read structure */
    image->png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!image->png_ptr) {
        printf("Error: Can not create PNG struct\n");
        fclose(fp);
        exit(ERR_FILE_READ_ERROR);
    }

    /* Create PNG info structure */
    image->info_ptr = png_create_info_struct(image->png_ptr);
    if (!image->info_ptr) {
        printf("Error: Can not create PNG info struct\n");
        fclose(fp);
        png_destroy_read_struct(&image->png_ptr, NULL, NULL);
        exit(ERR_FILE_READ_ERROR);
    }

    /* Set up error handling */
    if (setjmp(png_jmpbuf(image->png_ptr))) {
        printf("Error: Unknown\n");
        fclose(fp);
        png_destroy_read_struct(&image->png_ptr, &image->info_ptr, NULL);
        exit(ERR_FILE_READ_ERROR);
    }

    /* Initialize IO */
    png_init_io(image->png_ptr, fp);
    png_set_sig_bytes(image->png_ptr, 8);
    png_read_info(image->png_ptr, image->info_ptr);
    image->width = png_get_image_width(image->png_ptr, image->info_ptr);
    image->height = png_get_image_height(image->png_ptr, image->info_ptr);
    image->color_type = png_get_color_type(image->png_ptr, image->info_ptr);
    image->bit_depth = png_get_bit_depth(image->png_ptr, image->info_ptr);
    image->number_of_passes = png_set_interlace_handling(image->png_ptr);

    /* Check if color type is RGB */
    if (png_get_color_type(image->png_ptr, image->info_ptr) != PNG_COLOR_TYPE_RGB) {
        printf("Error: Not RGB color type in the file\n");
        exit(ERR_FILE_READ_ERROR);
    }

    /* Allocate memory for image rows */
    image->row_pointers = malloc(sizeof(png_bytep) * image->height);
    if (image->row_pointers == NULL) {
        printf("Error: Can not allocate memory for image->row_pointers while reading\n");
        exit(ERR_MEMORY_ALLOCATION_FAILURE);
    }
    for (y = 0; y < image->height; y++) {
        image->row_pointers[y] = malloc(png_get_rowbytes(image->png_ptr, image->info_ptr));
        if (image->row_pointers[y] == NULL){
            printf("Error: Can not allocate memory for pixel while reading\n");
            exit(ERR_MEMORY_ALLOCATION_FAILURE);
        }
    }
    
    /* Read image rows */
    png_read_image(image->png_ptr, image->row_pointers);
    
    /* Close file */
    fclose(fp);
}

/*
Writes a PNG image to a file.

Parameters:
- file_name: A string representing the file name/path where the PNG image will be saved.
- image: A pointer to the Png structure containing information about the PNG image.

Returns:
This function does not return a value.
*/
void write_png_file(char *file_name, Png *image) {
    int x, y;

    /* Open file */
    FILE *fp = fopen(file_name, "wb");
    if (!fp) {
        printf("Error: Can not create file: %s\n", file_name);
        exit(ERR_FILE_WRITE_ERROR);
    }

    /* Create PNG write structure */
    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr) {
        printf("Error: Can not create PNG write struct\n");
        fclose(fp);
        exit(ERR_FILE_WRITE_ERROR);
    }

    /* Create PNG info structure */
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        printf("Error: Can not create PNG info struct while writing\n");
        fclose(fp);
        png_destroy_write_struct(&png_ptr, NULL);
        exit(ERR_FILE_WRITE_ERROR);
    }

    /* Set up error handling */
    if (setjmp(png_jmpbuf(png_ptr))) {
        printf("Error: Unknown\n");
        fclose(fp);
        png_destroy_write_struct(&png_ptr, &info_ptr);
        exit(ERR_FILE_WRITE_ERROR);
    }

    /* Initialize IO */
    png_init_io(png_ptr, fp);
    png_set_IHDR(png_ptr, info_ptr, image->width, image->height, image->bit_depth, image->color_type, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
    png_write_info(png_ptr, info_ptr);

    /* Write image data */
    png_write_image(png_ptr, image->row_pointers);
    
    /* Handle errors */
    if (setjmp(png_jmpbuf(png_ptr))) {
        printf("Error: Unknown\n");
        fclose(fp);
        png_destroy_write_struct(&png_ptr, &info_ptr);
        exit(ERR_FILE_WRITE_ERROR);
    }

    /* Finalize writing */
    png_write_end(png_ptr, NULL);
    
    /* Clean up */
    fclose(fp);
    png_destroy_write_struct(&png_ptr, &info_ptr);
}

/*
Prints information about a PNG image.

Parameters:
- image: A pointer to the Png structure containing information about the PNG image.

Returns:
This function does not return a value.

Notes:
- This function prints various details about the PNG image, including its width, height, color type, bit depth, and number of passes.
- The color type is printed as a string representation.
- Bit depth indicates the number of bits per sample or per channel in the image.
- Number of passes refers to the number of passes required for interlaced PNG images.
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

/*
Processes color provided as a string and returns it as an integer array.

Parameters:
- string_color: A string representing color in the format "R.G.B".

Returns:
- An integer array containing the red, green, and blue components of the color.
- NULL if the input string is invalid or if memory allocation fails.
*/
int* process_color(char* string_color) {
    /* Takes color as "255.0.0" and returns as {255, 0, 0} */
    int index = 0;

    /* If color starts or ends with '.' */
    if (string_color[strlen(string_color)-1] == '.' || string_color[0] == '.'){
        return NULL;
    }

    char *token = strtok(string_color, ".");
    int *arr = malloc(sizeof(int)*3);
    if (arr == NULL) {
        printf("Error: Can not allocate memory for array of colors\n");
        exit(ERR_MEMORY_ALLOCATION_FAILURE);
    }
    while (token != NULL && index < 3) {
        arr[index++] = atoi(token);
        token = strtok(NULL, ".");
    }

    /* If there are less than 3 numbers or one of them are invalid */
    if (token != NULL || index != 3 || arr[0] > 255 || arr[0] < 0 || arr[1] > 255 || arr[1] < 0 || arr[2] > 255 || arr[2] < 0){
        return NULL;
    }

    return arr;
}

/*
Processes coordinates provided as a string and returns them as an integer array.

Parameters:
- string_coordinates: A string representing coordinates in the format "X.Y".

Returns:
- An integer array containing the X and Y coordinates.
- NULL if the input string is invalid or if memory allocation fails.
*/
int* process_coordinates(char* string_coordinates){
    /* Takes coordinates as "100.200" and returns as {100, 200} */
    int index = 0;

    /* If coordinates starts or ends with '.' */
    if (string_coordinates[strlen(string_coordinates)-1] == '.' || string_coordinates[0] == '.'){
        return NULL;
    }

    char *token = strtok(string_coordinates, ".");
    int *arr = malloc(sizeof(int)*2);
    if (arr == NULL) {
        printf("Error: Can not allocate memory for array of coordinates\n");
        exit(ERR_MEMORY_ALLOCATION_FAILURE);
    }
    while (token != NULL && index < 2) {
        arr[index++] = atoi(token);
        token = strtok(NULL, ".");
    }

    /* If there are less than 2 numbers */
    if (token != NULL || index != 2){
        return NULL;
    }

    return arr;
}

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
Draws a border around the specified rectangle in the image.

Parameters:
- image: A pointer to the Png structure representing the image.
- x1: The x-coordinate of the top-left corner of the rectangle.
- y1: The y-coordinate of the top-left corner of the rectangle.
- x2: The x-coordinate of the bottom-right corner of the rectangle.
- y2: The y-coordinate of the bottom-right corner of the rectangle.
- border_color: An array containing the RGB values of the border color.
- thickness: A string representing the thickness of the border.

Returns:
This function does not return a value.
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
                    px[0] = border_color[0];
                    px[1] = border_color[1];
                    px[2] = border_color[2];
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
                    px[0] = border_color[0];
                    px[1] = border_color[1];
                    px[2] = border_color[2];
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
                    px[0] = border_color[0];
                    px[1] = border_color[1];
                    px[2] = border_color[2];
                }
            }
        }

        /* Draw right vertical line */
        x = x2 + t;
        if (x >= 0 && x < image->width) {
            for (int y = y1 - t; y <= y2 + t; y++) {
                if (y >= 0 && y < image->height) {
                    png_bytep px = &(image->row_pointers[y][(x * 3)]);
                    px[0] = border_color[0];
                    px[1] = border_color[1];
                    px[2] = border_color[2];
                }
            }
        }
    }
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
