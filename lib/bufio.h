#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct buf_t {
	size_t size, capacity;
	char* buffer;
} buf_t;

buf_t* buf_new(size_t capacity);
void buf_free(buf_t* buffer);

size_t buf_capacity(buf_t* buffer);
size_t buf_size(buf_t* buffer);

ssize_t buf_fill(int fd, buf_t* buf, size_t required);
ssize_t buf_flush(int fd, buf_t* buf, size_t required);
ssize_t buf_getline(int fd, buf_t* buf, char stop_symb, void* ebuf);

