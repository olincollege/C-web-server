#ifndef READFILES_H
#define READFILES_H

#include <stdbool.h>
#include <stddef.h>

/**
 * @typedef fwmd
 * @brief File with metadata structure.
 * @field is_dir True if the file is a directory, false otherwise.
 * @field file_ext The file extension.
 * @field content_buf The contents of the file as a string.
 * @field buf_size The size of the buffer storing the file contents.
 */
typedef struct file_with_metadata {
    bool is_dir;
    char *file_ext;
    char *content_buf;
    size_t buf_size;
} fwmd;

/**
 * @typedef readfiles_error
 * @brief Enumeration of possible errors.
 */
typedef enum {
    READFILES_OK = 0,
    READFILES_FILE_NOT_FOUND,
    READFILES_FILE_TOO_LARGE,
    READFILES_INVALID_FILE_TYPE,
    READFILES_ERROR_OPENING_DIRECTORY,
    READFILES_ERROR_READING_DIRECTORY,
    READFILES_ERROR_READING_FILE,
    READFILES_OUT_OF_MEMORY
} readfiles_error;

/**
 * Converts a `readfiles_error` enum value to a human-readable error message.
 *
 * @param error The `readfiles_error` value to convert to a string.
 * @return A string representation of the `readfiles_error` value.
 */
const char *readfiles_error_string(readfiles_error error);

/**
 * @function get_file_size
 * @brief Gets the size of a file.
 * @param filename The name of the file to get the size of.
 * @param error Pointer to an error code to be set if there is an error.
 * @return The size of the file in bytes, or 0 if there was an error.
 */
size_t get_file_size(char *filename, readfiles_error *error);

/**
 * @function get_is_dir
 * @brief Determines whether a file is a directory.
 * @param filename The name of the file to check.
 * @param error Pointer to an error code to be set if there is an error.
 * @return True if the file is a directory, false otherwise.
 */
bool get_is_dir(char *filename, readfiles_error *error);

/**
 * @function get_file_ext
 * @brief Gets the extension of a file.
 * @param filename The name of the file to get the extension of.
 * @param error Pointer to an error code to be set if there is an error.
 * @return A pointer to the extension of the file, or NULL if there is no extension.
 */
char *get_file_ext(char *filename, readfiles_error *error);

/**
 * @function read_file
 * @brief Reads the contents of a file into a file with metadata structure.
 * @param path The path to the file to read.
 * @param myfile A pointer to the file with metadata structure to store the file contents in.
 * @param error Pointer to an error code to be set if there is an error.
 * @return The number of bytes read, or 0 if there was an error.
 */
size_t read_file(char *path, fwmd *myfile, readfiles_error *error);

/**
 * @function read_directory
 * @brief Reads the contents of a directory into a file with metadata structure.
 * @param path The path to the directory to read.
 * @param myfile A pointer to the file with metadata structure to store the directory contents in.
 * @param error Pointer to an error code to be set if there is an error.
 * @return The number of bytes read, or 0 if there was an error.
 */
size_t read_directory(char *path, fwmd *myfile, readfiles_error *error);

/**
 * @function create_file_with_metadata
 * @brief Creates a file with metadata structure for a file or directory.
 * @param path The path to the file or directory to create the structure for.
 * @param error Pointer to an error code to be set if there is an error.
 * @return
 */
fwmd *create_file_with_metadata(char *path, readfiles_error *error);

/**
 * @function free_file_with_metadata
 * @brief Frees the memory used by a file with metadata structure.
 * @param file A pointer to the file with metadata structure to free.
 * @param preserve_buf True if the buffer should not be freed, false otherwise.
 */
void free_file_with_metadata(fwmd *file, bool preserve_buf);

#endif /* READFILES_H */
