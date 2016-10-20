#include "read_write_lib.h"

int fd_reading_n(int fd, string& buf) {
	char buffer[1025];
	ssize_t tmp_read;
	while ((tmp_read = read(fd, buffer, 1024)) != 0) {
//    printf("**read: %d\n", tmp_read);
		if (tmp_read < 0) {
			if (errno != EAGAIN) {
				printf("Reading problems: %m\n");
				return -1;
			}
			return 0;
		} else {
//      printf ("buffer: '%.*s'\n", tmp_read, buffer);
			buffer[tmp_read] = 0;
			buf += buffer;
		}
//    printf ("while: next iteration\n");
	}
//  printf("tmp_read: %d\n", tmp_read);
	return 1;
}

int reading_n(int fd, string& buf) {
	char buffer[1025];
	ssize_t tmp_read;
	while ((tmp_read = recv(fd, buffer, 1024, MSG_DONTWAIT)) != 0) {
		if (tmp_read < 0) {
			if (errno != EAGAIN) {
				printf("Reading problems: %m\n");
				return -1;
			}
			return 0;
		} else {
			buffer[tmp_read] = 0;
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
			if (tmp_write < (ssize_t)buf.size()) {
				buf = buf.substr(tmp_write, buf.size());
			} else {
        buf = "";
        break;
      }
		}
	}
	return 1;
}
