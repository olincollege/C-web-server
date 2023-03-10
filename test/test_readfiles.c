#include <criterion/criterion.h>
#include <criterion/new/assert.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

#include "../src/readfiles.h"

char file_contents[] = "This is a test file meant for testing the readfiles code. "
                       "It even ends with a carriage return AND line feed!\r\n";

void setup_file() {
    FILE *file = fopen("testfile.txt", "w");// open file in write mode
    if (file == NULL) {
        perror("Failed to open test file for writing");
        return;
    }
    fprintf(file, "%s", file_contents);// write text to file
    fclose(file);                      // close file
}

void cleanup_file() {
    int status = remove("testfile.txt");// delete file
    if (status != 0) {
        perror("Failed to delete test file");
        return;
    }
}

void setup_dir() {
    int folder = mkdir("sampledir", 0777);// create directory
    if (folder == -1) {
        perror("Failed to create directory");
        return;
    }
    FILE *file = fopen("sampledir/testfile.txt", "w");
    if (file == NULL) {
        perror("Failed to open test file for writing");
        return;
    }
    fprintf(file, "%s", file_contents);// write text to file
    fclose(file);
}

void cleanup_dir() {
    int status = remove("sampledir/testfile.txt");// delete file
    if (status != 0) {
        perror("Failed to delete test file");
        return;
    }
    status = rmdir("sampledir");// delete directory
    if (status != 0) {
        perror("Failed to delete sample directory");
        return;
    }
}

// Test reading a valid file
Test(read_file, valid_file, .init = setup_file, .fini = cleanup_file) {
    readfiles_error err = -1;
    fwmd *f = create_file_with_metadata("testfile.txt", &err);

    cr_assert(eq(int, err, READFILES_OK));
    cr_assert(eq(int, f->is_dir, 0));
    cr_assert(eq(int, f->buf_size, sizeof(file_contents) - 1));// -1 to account for null terminator
    cr_assert(eq(str, f->content_buf, file_contents));
}

// Test reading an invalid file
Test(read_file, invalid_path, .init = setup_file, .fini = cleanup_file) {
    readfiles_error err = -1;
    fwmd *f = create_file_with_metadata("nonexistentfile.txt", &err);

    cr_assert(eq(int, err, READFILES_FILE_NOT_FOUND));
    cr_assert(eq(ptr, f, NULL));
}

Test(read_file, valid_dir, .init = setup_dir, .fini = cleanup_dir) {
    readfiles_error err = -1;
    fwmd *f = create_file_with_metadata("sampledir", &err);

    cr_assert(eq(int, err, READFILES_OK));
    cr_assert(eq(int, f->is_dir, 1));
    cr_assert(eq(int, f->buf_size, strlen("testfile.txt") + 3));
}
