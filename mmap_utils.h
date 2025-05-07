#ifndef MMAP_UTILS_H
#define MMAP_UTILS_H
#include <sys/types.h>

int mmap_write(const char *filepath, off_t offset, const char *new_text);

#endif // MMAP_UTILS_H
