#include <criterion/criterion.h>
#include <criterion/new/assert.h>

#include "../src/readfiles.h"

const char file_contents[] = "This is a test file meant for testing the readfiles code. "
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

// Test reading a file
Test(read_file, valid_file, .init = setup_file, .fini = cleanup_file) {
    readfiles_error err;
    fwmd *f = create_file_with_metadata("testfile.txt", &err);

    cr_assert(eq(int, err, READFILES_OK));

    cr_assert(eq(int, f->is_dir, 0));
    cr_assert(eq(int, f->buf_size, sizeof(file_contents) - 1));// -1 to account for null terminator
    cr_assert(eq(str, f->content_buf, file_contents));
}
