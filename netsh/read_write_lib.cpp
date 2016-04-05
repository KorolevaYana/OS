#include "read_write_lib.h"

int reading_n(int fd, string& buf) {
	char buffer[1024];
	ssize_t tmp_read;
	while ((tmp_read = read(fd, buffer, 1024)) != 0) {
		if (tmp_read < 0) {
			if (errno != EAGAIN) {
				printf("Reading problems: %m\n");
				return -1;
			}
			return 0;
		} else {
			if (tmp_read < 1024) {
				buffer[tmp_read - 1] = 0;
			}
			buf += buffer;
		}
	}
	return 1;
}

int writing(int fd, string& buf) {
	ssize_t tmp_write;
	while ((tmp_write = write(fd, buf.c_str(), buf.size())) != -1) {
		if (tmp_write < 0) {
			if (errno != EAGAIN) {
				printf("Writing problems: %m\n");
				return -1;
			}
			return 0;
		} else {
			if (tmp_write < buf.size()) {
				buf = buf.substr(tmp_write, buf.size());
			}
		}
	}
	return 1;
}
