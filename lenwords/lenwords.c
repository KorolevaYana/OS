#include <stdio.h>
#include <string.h>

#include <helpers.h>

int main() {
	char buf[4100];
	char out[4100];

	long long i = 1;
	ssize_t tmp_read, tmp_write;

	while ((tmp_read = read_until(STDIN_FILENO, buf, 4100, ' ')) != 0) {
		if (tmp_read == -1) {
			printf("Problems with reading %lld word.", i);
			return 1;
		}

		if (buf[tmp_read - 1] == ' ') {
	  	sprintf(out, "%i ", (int)tmp_read - 1);
		} else {
			sprintf(out, "%i", (int)tmp_read);
		}

		tmp_write = write_(STDOUT_FILENO, out, strlen(out));
		if (tmp_write == -1) {
			printf("Problems with writing %lld word.", i);
			return 1;
		}
		i++;
	}

	return 0;
}
