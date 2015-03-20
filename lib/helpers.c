#include <stdio.h>
#include <stdlib.h>
#include "helpers.h"

int spawn(const char* file, char* const argv[]) {
	pid_t child_process = fork();
	int result;
	if (child_process == 0) {
		int exc = execvp(file, argv);
		if (exc == -1) 
			return -1;
	} else if (child_process > 0) {
		pid_t waiting = waitpid(child_process, &result, 0);
		if (waiting == -1)
			return -1;
		return result;
	} else {
		return -1;
	}
}

ssize_t read_(int fd, void *buf, size_t count) {
	ssize_t result = 0;
	while (1) {
		ssize_t tmp = read(fd, buf + result, count - result);
		if (tmp == -1)
			return -1;
		result += tmp;
		if (result == count || tmp == 0) {
			return result;
		} 
	}
}

ssize_t write_(int fd, const void *buf, size_t count) {
	ssize_t result = 0;
	while (1) {
		ssize_t tmp = write(fd, buf + result, count - result);
		if (tmp == -1)
			return -1;
		result += tmp;
		if (result == count)
			return result;
		}
}

ssize_t read_until(int fd, void * buf, size_t count, char delimiter) {
	size_t current = 0;
	char *c = (char*)(malloc(sizeof(char)));
	while (1){
		if (current == count)
			return current;
		ssize_t tmp = read_(fd, c, 1);
		if (tmp == -1)
			return -1;

		if (tmp == 0) {
			return current;
		}

		*((char*)(buf + current)) = *c;
		current++;

		if (*c == delimiter)
			return current;
	}
}

