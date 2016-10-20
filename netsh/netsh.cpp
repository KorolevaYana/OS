#include "daemon.h"

int main(int argc, char** argv) { 
	if (argc != 2) {
		printf("Wrong amount of arguments.\n");
		return 0;
	}

	my_daemon lol(argv[1]);
	int pid1 = fork();
	if (pid1 == 0) {
		int pid = setsid();
		if (pid < 0)
			return -1;
		int a[2];
		pipe2(a, 0);
		int pid2 = fork();
		if (pid2 == 0) {
			lol.run();
		} else if (pid2 == -1) {
			printf("Fork error.\n");
			return -1;
		} else {
		  FILE* f = fopen("/tmp/netsh.pid", "w");
			fprintf(f, "%d", pid2);
			fclose(f);
			exit(0);
		}
	} else if (pid1 == -1) {
		printf("Fork error.\n");
		return -1;
	} else {
		exit(0);
	}
}
