#include <stdlib.h>
#include <stdio.h>

#include <helpers.h>

const int MAX_SIZE = 2000;

int main() {
	char buffer[2000];
	ssize_t read_res, write_res;
	while ((read_res = read_(STDIN_FILENO, buffer, MAX_SIZE)) != 0) {
		if (read_res == -1) {
			perror("Read error.");
			exit(1);
		} else {
			write_res = write_(STDOUT_FILENO, buffer, read_res);
			if (write_res == -1) {
				perror("Write error.");
				exit(1);
			}
		}
	}
  return 0;
}
