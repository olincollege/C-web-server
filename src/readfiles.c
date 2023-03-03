#include "readfiles.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "readfiles.h"

size_t get_file_size(char *filename) {
    struct stat st;
    if (stat(filename, &st) != 0) {
        perror("Could not open file");
        exit(1);

    } else if (st.st_size > MAX_FILE_SIZE) {
        perror("File too large to read");
        exit(2);
    }
    return st.st_size;
}

int get_type(char *filename) {
    struct stat st;
    stat(filename, &st);
    if (S_ISREG(st.st_mode)) {
        return FILETYPE_FILE;
    } else if (S_ISDIR(st.st_mode)) {
        return FILETYPE_DIRECTORY;
    } else {
        return FILETYPE_OTHER;
    }
}

char *get_file_ext(char *filename) {
    char *current = filename;
    char *dot_location;
    while (strchr(current, '.') != NULL) {
        dot_location = strchr(current, '.');
        current = dot_location + 1;
    }
    return dot_location;
}

void read_file(char *path, fwmd *myfile) {
    FILE *fp = fopen(path, "r");
    if (!fp) {
        perror("Could not open file");
        exit(1);
    }
    myfile->buf_size = get_file_size(path);
    myfile->content_buf = malloc(myfile->buf_size);
    char current = fgetc(fp);
    for (int i = 0; i < (int) myfile->buf_size; i++) {
        myfile->content_buf[i] = current;
        current = fgetc(fp);
    }
    fclose(fp);
}

void read_directory(char *path, fwmd *myfile) {
    DIR *dp;
    struct dirent *ent;
    if ((dp = opendir(path)) == NULL) {
        perror("Error opening directory");
        exit(1);
    }
    while ((ent = readdir(dp)) != NULL) {
        if (ent->d_type == DT_DIR) {
            if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) {
                continue;
            }
            char *item_str = malloc(strlen(ent->d_name) + 3);
            sprintf(item_str, "- %s/\n", ent->d_name);
            myfile->buf_size += strlen(item_str);
            myfile->content_buf = realloc(myfile->content_buf, myfile->buf_size + 1);
            strcat(myfile->content_buf, item_str);
            free(item_str);
        } else if (ent->d_type == DT_REG) {
            char *item_str = malloc(strlen(ent->d_name) + 2);
            sprintf(item_str, "- %s\n", ent->d_name);
            myfile->buf_size += strlen(item_str);
            myfile->content_buf = realloc(myfile->content_buf, myfile->buf_size + 1);
            strcat(myfile->content_buf, item_str);
            free(item_str);
        }
    }
    closedir(dp);
}


void create_file_with_metadata(char *path) {
    fwmd myfile = {0, 0, 0, 0};
    myfile.file_ext = get_file_ext(path);
    myfile.type = get_type(path);
    if (myfile.type == FILETYPE_FILE) {
        read_file(path, &myfile);
        // Return struct here instead of printing
        printf("File Extension %s\n", myfile.file_ext);
        printf("File Size: %zu bytes\n\n", myfile.buf_size);
        printf("%s", myfile.content_buf);
        free(myfile.content_buf);
    } else if (myfile.type == FILETYPE_DIRECTORY) {
        read_directory(path, &myfile);
        // Return struct here instead of printing
        printf("%s\n", path);
        printf("File Extension %s\n", myfile.file_ext);
        printf("File Size: %zu bytes\n", myfile.buf_size);
        printf("Content:\n%s\n", myfile.content_buf);
        free(myfile.content_buf);
    }
}

// Temporary function to run things in the command line
int main(void) {
    char path[200];
    fprintf(stdout, "Enter file path (relative to current path):\n");
    fgets(path, sizeof(path), stdin);
    char *newline_loc = strchr(path, '\n');
    *newline_loc = '\0';
    create_file_with_metadata(path);
    return 0;
}
