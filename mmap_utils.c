#include "mmap_utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

int mmap_write(const char *filepath, off_t offset, const char *new_text) {
    int fd;
    void *map;
    struct stat sb;
    size_t page_size = sysconf(_SC_PAGE_SIZE);
    off_t pa_offset;
    size_t map_size;

    fd = open(filepath, O_RDWR);
    if (fd == -1)
        handle_error("open");

    if (fstat(fd, &sb) == -1)
        handle_error("fstat");

    off_t file_size = sb.st_size;

    printf("Page size       : %ld bytes\n", page_size);
    printf("File size       : %ld bytes\n", file_size);
    printf("Requested offset: 0x%lx (%ld)\n", offset, offset);

    if (offset >= file_size) {
        fprintf(stderr, "Offset is past the end of file.\n");
        close(fd);
        return EXIT_FAILURE;
    }

    pa_offset = offset & ~(page_size - 1);
    size_t delta = offset - pa_offset;
    map_size = file_size - pa_offset;

    if (map_size % page_size != 0) {
        map_size = ((map_size / page_size) + 1) * page_size;
        printf("Adjusted mapping size to %ld (multiple of page size)\n", map_size);
    }

    map = mmap(NULL, map_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, pa_offset);
    if (map == MAP_FAILED)
        handle_error("mmap");

    char *data = (char *)map + delta;
    size_t read_len = file_size - offset;

    printf("\n--- Original file content from offset 0x%lx ---\n", offset);
    fwrite(data, 1, read_len, stdout);
    printf("\n----------------------------------------------\n");

    size_t new_len = strlen(new_text);

    if (new_len + delta <= map_size) {
        memcpy((char *)map + delta, new_text, new_len);
        printf("Written: \"%s\"\n", new_text);

        if (msync(map, map_size, MS_SYNC) == -1)
            handle_error("msync");
    } else {
        fprintf(stderr, "Not enough mapped space to write the new text!\n");
    }

    munmap(map, map_size);
    close(fd);
    return EXIT_SUCCESS;
}
