#pragma once
#include <stdlib.h>

// Current size limit is 32MB
const size_t MAX_FILE_SIZE = 32000000;

typedef enum {
    FILETYPE_FILE = 1,
    FILETYPE_DIRECTORY = 2,
    FILETYPE_OTHER = 3
} type;

typedef struct file_with_metadata {
    int  type;
    char *file_ext;
    char *content_buf;
    size_t buf_size;
} fwmd;

/**
 * Get the size of a file in bytes.
 * 
 * @param filename 
 * @return size_t 
 */
size_t get_file_size(char *filename);

/**
 * Deterine if a file is a directory or not.
 * 
 * @param filename 
 * @return char 
 */
int get_type(char *filename);

/**
 * Get the extension of a file.
 * 
 * @param filename 
 * @return char* 
 */
char *get_file_ext(char *filename);

/**
 * Read the contents of a file and store it in a buffer.
 * 
 * @param path 
 * @param myfile 
 */
void read_file(char *path, fwmd *myfile);

/**
 * Read the contents of a directory and store it in a buffer.
 * 
 * @param path 
 * @param myfile 
 */
void read_directory(char *path, fwmd *myfile);

/**
 * Create a file_with_metadata struct and populate it with data.
 * 
 * @param path 
 */
void create_file_with_metadata(char *path);
