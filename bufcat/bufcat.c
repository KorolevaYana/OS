#include <stdio.h>

#include <bufio.h>

int main() {
	buf_t* my_buf = buf_new(4096);
	ssize_t tmp_read;
	ssize_t tmp_write;
	
	while (tmp_read = buf_fill(STDIN_FILENO, my_buf, 2048)) {
		tmp_write = buf_flush(STDOUT_FILENO, my_buf, tmp_read);
		if (tmp_write < 0) {
			perror("Problems with writing.");
			return 1;
		}
		
	} 
	if (tmp_read < 0) {
		tmp_write = buf_flush(STDOUT_FILENO, my_buf, 4096);
		if (tmp_write < 0) {
			perror("Problems with writing.");
			return 3;
		}
		return 2;	
	}

	return 0;
}
