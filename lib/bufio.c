#include <bufio.h>

#ifdef DEBUG
#define DEBUG_ASSERT(a) if(a) { abort(); }
#else
#define DEBUG_ASSERT(a)
#endif

buf_t *buf_new(size_t capacity) {
	buf_t *result = (buf_t*)malloc(sizeof(buf_t));
	result->buffer = (char*)malloc(sizeof(char) * capacity);
	if (result == NULL)
		return NULL;
	result->size = 0;
	result->capacity = capacity;
	return result;
}

void buf_free(buf_t* buf) {
	DEBUG_ASSERT(buf == NULL)
	free(buf->buffer);
	free(buf);
}

size_t buf_capacity(buf_t* buf) {
	DEBUG_ASSERT(buf == NULL)
	return buf->capacity;
}

size_t buf_size(buf_t* buf) {
	DEBUG_ASSERT(buf == NULL)
	return buf->size;
}

ssize_t buf_fill(int fd, buf_t *buf, size_t required) {
	DEBUG_ASSERT(buf == NULL)
	DEBUG_ASSERT(required > buf->size)
	ssize_t current_size = 0;
	ssize_t tmp_read;
	while ((tmp_read = read(fd, buf->buffer + current_size, buf->capacity - current_size)) > 0) {
		current_size += tmp_read;
		buf->size += tmp_read;
		if (current_size >= required)
			break;
	}
	if (tmp_read < 0)
		return -1;
	return current_size;
}

ssize_t buf_flush(int fd, buf_t *buf, size_t required) {
	DEBUG_ASSERT(buf == NULL)
	DEBUG_ASSERT(required > buf->size)
	ssize_t current_size = 0;
	ssize_t tmp_write;
	while ((tmp_write = write(fd, buf->buffer, buf->capacity - current_size)) > 0) {
		for (int i = 0; i < buf->size; i++) {
			(buf->buffer)[i] = (buf->buffer)[tmp_write + i];
		}
		current_size += tmp_write;
		buf->size -= tmp_write;
		if (current_size >= required)
			break;
	}
	if (tmp_write < 0)
		return -1;
	return current_size;
}
