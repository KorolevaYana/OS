#include <string.h>
#include <helpers.h>
#include <bufio.h>
#include <errno.h>

int main(int argc, char* argv[]) {
	ssize_t tmp_get;
	char buffer[4096];

	buf_t* buf = buf_new(4096);

	while(1) {
		memset(buffer,0, 4096);
		write_(STDOUT_FILENO, "$", 1);
		if ((tmp_get = buf_getline(STDIN_FILENO, buf, '\n', buffer)) < 0) {
			if (errno == EINTR) {
				continue;
			}
			return 1;
		} else if (tmp_get == 0) {
			return 0;
		}

		buffer[tmp_get] = 0;

		int cnt = 0;
		for (int i = 0; i < tmp_get; i++) {
			if (buffer[i] == '|') {
				cnt++;
			}
		}

		execargs_t* programs[cnt];

		int last = 0;
		int prog_cnt = 0;
		int args_cnt = 0;
		for (int i = 0; i < (int)tmp_get; i++) {
			if (buffer[i] == '|' || buffer[i] == 0) {
				int space = last - 1;
				for (int j = last; j < i; j++) {
					if (buffer[j] == ' ' && space != j - 1) {
						args_cnt++;
					}
					space = j;
				}
		
				int args_last = last;
				char* arguments[args_cnt + 1];

				args_cnt = 0;
				for (int j = last; j <= i; j++) {
					if (buffer[j] == 0) {
						if (args_last != j) {
							arguments[args_cnt++] = buffer + args_last;
						}
						args_last = j + 1;
					}
				}
				arguments[args_cnt] = NULL;
				
				if ((programs[prog_cnt++] = new_exec(arguments)) == NULL) {
					for (int j = 0; j < prog_cnt; j++) {
						free_exec(programs[j]);
					}
					return 1;
				}
				last = i + 1;
			}
		}

		if (runpiped(programs, prog_cnt)) {
			for (int i = 0; i < prog_cnt; i++) {
				free_exec(programs[i]);
			}
			return 1;
		}
	}	
}
