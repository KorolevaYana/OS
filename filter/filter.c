#include <stdio.h>

#include <helpers.h>

int main(int argc, char* argv[]) {
	char buf[4096];
	char* name = argv[1];
	ssize_t tmp_read = 0;
	int i;
	
	for (i = 1; i < argc; i++) 
		argv[i - 1] = argv[i];
	
	while((tmp_read = read_until(STDIN_FILENO, buf, 4096, '\n')) > 0)	{
		if (tmp_read == -1) {
			printf("Problems with reading string.");
			return 0;
		}

		if (buf[tmp_read - 1] == '\n') 
 			buf[--tmp_read] = 0;
		argv[argc - 1] = buf;

		int result = spawn(name, argv);
		if (result == 0) {	
			ssize_t tmp_write;
			buf[tmp_read++] = '\n';
			tmp_write = write_(STDOUT_FILENO, buf, tmp_read);

			if (tmp_write == -1) {
				printf("Problems with writing string.");
				return 0;
			}				
		} else if (result == -1) {
			printf("Problems with executing file.");
			return 0;
		}
	}
	return 0;
}
