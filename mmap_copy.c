#include "mmap_copy.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>

int mmap_copy_file(const char *FileIn, const char *FileOut) {
    int in_fd = open(FileIn, O_RDONLY);
    if (in_fd == -1) {
        perror("Failed to open input file");
        return EXIT_FAILURE;
    }

    // Lưu trữ thông tin của file input (kích thước, quyền truy cập, thời gian sửa đổi, v.v).
    struct stat st;
    if (fstat(in_fd, &st) == -1) {
        perror("Failed to get file status");
        close(in_fd);
        return EXIT_FAILURE;
    }

    size_t file_size = st.st_size;
    printf("Input file size: %ld bytes\n", file_size);

    int out_fd = open(FileOut, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (out_fd == -1) {
        perror("Failed to open output file");
        close(in_fd);
        return EXIT_FAILURE;
    }

    if (ftruncate(out_fd, file_size) == -1) {
        perror("Failed to resize output file");
        close(in_fd);
        close(out_fd);
        return EXIT_FAILURE;
    }

    // Ánh xạ file input vào bộ nhớ
    void *in_map = mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, in_fd, 0);
    if (in_map == MAP_FAILED) {
        perror("mmap input failed");
        close(in_fd);
        close(out_fd);
        return EXIT_FAILURE;
    }

    // Ánh xạ file output vào bộ nhớ
    void *out_map = mmap(NULL, file_size, PROT_READ | PROT_WRITE, MAP_SHARED, out_fd, 0);
    if (out_map == MAP_FAILED) {
        perror("mmap output failed");
        munmap(in_map, file_size);
        close(in_fd);
        close(out_fd);
        return EXIT_FAILURE;
    }

    // Kiểm tra overlap
    if ((in_map <= out_map && (char *)in_map + file_size > (char *)out_map) ||
        (out_map <= in_map && (char *)out_map + file_size > (char *)in_map)) {
        fprintf(stderr, "Memory regions overlap. Remapping output...\n");
        munmap(out_map, file_size);

        // Ánh xạ lại out_map ở vị trí khác
        out_map = mmap(NULL, file_size, PROT_READ | PROT_WRITE, MAP_SHARED, out_fd, 0);
        if (out_map == MAP_FAILED || out_map == in_map) {
            perror("Remapping output failed");
            munmap(in_map, file_size);
            close(in_fd);
            close(out_fd);
            return EXIT_FAILURE;
        }
    }

    // Sao chép nội dung từ input sang output
    memcpy(out_map, in_map, file_size);

    // Hiển thị nội dung file output 
    write(STDOUT_FILENO, out_map, file_size);
    printf("\n");

    munmap(in_map, file_size);
    munmap(out_map, file_size);
    close(in_fd);
    close(out_fd);

    return EXIT_SUCCESS;
}
