#include "structures_and_errors.h"

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
