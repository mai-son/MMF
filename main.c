#include "mmap_utils.h"
#include "mmap_copy.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage:\n");
        fprintf(stderr, "  %s copy <source_file> <dest_file>\n", argv[0]);
        fprintf(stderr, "  %s write <file_path> <offset> <text>\n", argv[0]);
        return EXIT_FAILURE;
    }

    if (strcmp(argv[1], "copy") == 0) {
        if (argc < 4) {
            fprintf(stderr, "Usage: %s copy <source_file> <dest_file>\n", argv[0]);
            return EXIT_FAILURE;
        }
        const char *src = argv[2];
        const char *dst = argv[3];

        if (mmap_copy_file(src, dst) != 0) {
            fprintf(stderr, "Copy failed.\n");
            return EXIT_FAILURE;
        }
        printf("Copy successful.\n");

    } else if (strcmp(argv[1], "write") == 0) {
        if (argc < 5) {
            fprintf(stderr, "Usage: %s write <file_path> <offset> <text>\n", argv[0]);
            return EXIT_FAILURE;
        }
        const char *filepath = argv[2];
        off_t offset = strtol(argv[3], NULL, 0);  // Cho phép nhập offset dạng thập phân hoặc hexa
        const char *text = argv[4];

        if (mmap_write(filepath, offset, text) != 0) {
            fprintf(stderr, "Write failed.\n");
            return EXIT_FAILURE;
        }
        printf("Write successful.\n");

    } else {
        fprintf(stderr, "Unknown command: %s\n", argv[1]);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
