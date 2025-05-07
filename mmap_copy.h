#ifndef MMAP_COPY_H
#define MMAP_COPY_H

#include <stddef.h>

// Hàm chính thực hiện sao chép qua memory-mapped file
int mmap_copy_file(const char *FileIn, const char *FileOut);

#endif // MMAP_COPY_H
