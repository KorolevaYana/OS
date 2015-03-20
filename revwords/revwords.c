#include <stdio.h>

#include <helpers.h>

void reverse(char *buf, int size) {
	for (int i = 0; i < size / 2; i++) {
		char tmp = *(buf + i);
		char tmp1 = *(buf + (size - i) - 1);
		*(buf + i) = tmp1;
		*(buf + (size - i) - 1) = tmp;
	}
}

int main() {
	char buf[4100];

	long long i = 1;
	ssize_t tmp_read, tmp_write;
	while ((tmp_read = read_until(STDIN_FILENO, buf, 1024, ' ')) != 0) {
		if (tmp_read == -1) {
			printf("Problems with reading %lld word.", i);
			return 1;
		}

		if (buf[tmp_read - 1] == ' ')
	  	reverse(buf, tmp_read - 1);
		else
	  	reverse(buf, tmp_read);
	
		tmp_write = write_(STDOUT_FILENO, buf, tmp_read);
		if (tmp_write == -1) {
			printf("Problems with writing %lld word.", i);
			return 1;
		}
		i++;
	}

	return 0;
}
