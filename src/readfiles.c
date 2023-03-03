#include "readfiles.h"
#include <dirent.h>
#include <stdbool.h>
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

    }
    // Current size limit is 32MB
    else if (st.st_size > 32000000) {
        perror("File too large to read");
        exit(2);
    }
    return st.st_size;
}

bool get_is_dir(char *filename) {
    struct stat st;
    stat(filename, &st);
    if (S_ISREG(st.st_mode)) {
        return false;
    } else if (S_ISDIR(st.st_mode)) {
        return true;
    } else {
        perror("Incompatible File Type");
        exit(3);
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
    struct dirent *dirp;
    if ((dp = opendir(path)) == NULL) {
        perror("Error opening directory");
        exit(1);
    }
    FILE *temp = fopen("__temp_buffer_storage_.txt", "w");
    if (!temp) {
        perror("Could not open file");
        exit(1);
    }
    while ((dirp = readdir(dp)) != NULL) {
        if (strcmp(dirp->d_name, "__temp_buffer_storage_.txt") != 0) {
            fprintf(temp, "- %s\n", dirp->d_name);
        }
    }
    fclose(temp);
    closedir(dp);
    read_file("__temp_buffer_storage_.txt", myfile);
    remove("__temp_buffer_storage_.txt");
}

void create_file_with_metadata(char *path) {
    fwmd myfile = {0, 0, 0, 0};
    myfile.file_ext = get_file_ext(path);
    myfile.is_dir = get_is_dir(path);
    if (myfile.is_dir == false) {
        read_file(path, &myfile);
        // Return struct here instead of printing
        printf("File Extension %s\n", myfile.file_ext);
        printf("File Size: %zu bytes\n\n", myfile.buf_size);
        printf("%s", myfile.content_buf);
        free(myfile.content_buf);
    } else {
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
