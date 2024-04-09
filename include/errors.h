#ifndef ERRORS_H
#define ERRORS_H

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

#endif