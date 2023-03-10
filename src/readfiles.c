#include "readfiles.h"
#include <dirent.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

const char *readfiles_error_string(readfiles_error error) {
    switch (error) {
        case READFILES_OK:
            return "No error";
        case READFILES_FILE_NOT_FOUND:
            return "File not found";
        case READFILES_FILE_TOO_LARGE:
            return "File too large to read";
        case READFILES_INVALID_FILE_TYPE:
            return "Invalid file type";
        case READFILES_ERROR_OPENING_DIRECTORY:
            return "Error opening directory";
        case READFILES_ERROR_READING_DIRECTORY:
            return "Error reading directory";
        case READFILES_ERROR_READING_FILE:
            return "Error reading file";
        case READFILES_OUT_OF_MEMORY:
            return "Out of memory";
        default:
            return "Unknown error";
    }
}


size_t get_file_size(char *filename, readfiles_error *error) {
    struct stat st;
    if (stat(filename, &st) != 0) {
        *error = READFILES_FILE_NOT_FOUND;
        return 0;
    }
    // Current size limit is 32MB
    else if (st.st_size > 32000000) {
        *error = READFILES_FILE_TOO_LARGE;
        return 0;
    }
    return (size_t) st.st_size;
}

bool get_is_dir(char *filename, readfiles_error *error) {
    struct stat st;
    if (stat(filename, &st) != 0) {
        *error = READFILES_FILE_NOT_FOUND;
        return false;
    } else if (S_ISREG(st.st_mode)) {
        return false;
    } else if (S_ISDIR(st.st_mode)) {
        return true;
    } else {
        *error = READFILES_INVALID_FILE_TYPE;
        return false;
    }
}

char *get_file_ext(char *filename) {
    char *dot_location = strrchr(filename, '.');
    if (dot_location == NULL) {
        return NULL;
    }
    return dot_location + 1;
}

size_t read_file(char *path, fwmd *myfile, readfiles_error *error) {
    FILE *fp = fopen(path, "r");
    if (!fp) {
        *error = READFILES_ERROR_READING_FILE;
        return 0;
    }
    myfile->buf_size = get_file_size(path, error);
    if (*error != READFILES_OK) {
        fclose(fp);
        return 0;
    }
    myfile->content_buf = malloc(myfile->buf_size);
    if (myfile->content_buf == NULL) {
        *error = READFILES_OUT_OF_MEMORY;
        fclose(fp);
        return 0;
    }
    size_t bytes_read = fread(myfile->content_buf, 1, myfile->buf_size, fp);
    if (bytes_read != myfile->buf_size) {
        *error = READFILES_ERROR_READING_FILE;
        free(myfile->content_buf);
        myfile->content_buf = NULL;
        myfile->buf_size = 0;
    }
    fclose(fp);
    return bytes_read;
}

size_t read_directory(char *path, fwmd *myfile, readfiles_error *error) {
    DIR *dp;
    struct dirent *dirp;
    if ((dp = opendir(path)) == NULL) {
        *error = READFILES_ERROR_OPENING_DIRECTORY;
        return 0;
    }
    size_t buf_size = 0;
    char *buf = NULL;
    while ((dirp = readdir(dp)) != NULL) {
        if (strcmp(dirp->d_name, ".") == 0 || strcmp(dirp->d_name, "..") == 0)
            continue;
        size_t entry_len = strlen(dirp->d_name) +
                           3;// 3 extra characters for "- " and "\n"
        char *entry_buf = malloc(entry_len);
        if (entry_buf == NULL) {
            *error = READFILES_OUT_OF_MEMORY;
            free(buf);
            buf = NULL;
            buf_size = 0;
            break;
        }
        snprintf(entry_buf, entry_len, "- %s\n", dirp->d_name);
        size_t new_buf_size = buf_size + entry_len;
        char *new_buf = realloc(buf, new_buf_size);
        if (new_buf == NULL) {
            *error = READFILES_OUT_OF_MEMORY;
            free(buf);
            free(entry_buf);
            buf = NULL;
            buf_size = 0;
            break;
        }
        buf = new_buf;
        buf_size = new_buf_size;
        strcat(buf, entry_buf);
        free(entry_buf);
    }
    closedir(dp);
    if (*error == READFILES_OK) {
        myfile->buf_size = buf_size;
        myfile->content_buf = buf;
    } else {
        free(buf);
        myfile->content_buf = NULL;
        myfile->buf_size = 0;
    }
    return myfile->buf_size;
}

fwmd *create_file_with_metadata(char *path, readfiles_error *error) {
    fwmd *myfile = malloc(sizeof(fwmd));
    *error = READFILES_OK;
    if (myfile == NULL) {
        *error = READFILES_OUT_OF_MEMORY;
        return NULL;
    }
    myfile->file_ext = get_file_ext(path);
    myfile->is_dir = get_is_dir(path, error);
    if (*error != READFILES_OK) {
        free(myfile);
        return NULL;
    }
    size_t bytes_read;
    if (myfile->is_dir == false) {
        bytes_read = read_file(path, myfile, error);
    } else {
        bytes_read = read_directory(path, myfile, error);
    }
    if (*error != READFILES_OK) {
        free(myfile->content_buf);
        free(myfile);
        myfile = NULL;
    } else if (bytes_read == 0) {
        free(myfile->content_buf);
        free(myfile);
        myfile = NULL;
        *error = READFILES_ERROR_READING_FILE;
    }
    return myfile;
}

void free_file_with_metadata(fwmd *file, bool preserve_buf) {
    if (file != NULL) {
        if (!preserve_buf && file->content_buf != NULL) {
            free(file->content_buf);
        }

        free(file);
    }
}
