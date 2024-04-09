#include "errors.h"
#include "structures.h"

/**
 * @brief Reads a PNG file and stores its information and pixel data in a Png structure.
 * 
 * @param file_name A string representing the file name/path of the PNG image to be read.
 * @param image A pointer to the Png structure where the image data and information will be stored.
 */
void read_png_file(char *file_name, Png *image) {
    int y;
    char header[8];

    /* Open file */
    FILE *fp = fopen(file_name, "rb");
    if (!fp) {
        printf("Error: Can not read file %s\n", file_name);
        exit(ERR_FILE_NOT_FOUND);
    }

    /* Read first 8 bytes to verify PNG file */
    fread(header, 1, 8, fp);
    if (png_sig_cmp((const unsigned char *)header, 0, 8)) {
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

/**
 * @brief Writes a PNG image to a file.
 * 
 * @param file_name A string representing the file name/path where the PNG image will be saved.
 * @param image A pointer to the Png structure containing information about the PNG image.
 */
void write_png_file(char *file_name, Png *image) {

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
