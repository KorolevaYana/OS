#include "daemon.h"

int main(int argc, char** argv) { 
	if (argc != 2) {
		printf("Wrong amount of arguments.");
		return 0;
	}

	my_daemon lol(argv[1]);
	int pid1 = fork();
	if (pid1 == 0) {
		FILE* f = fopen("/tmp/netsh.pid", "w");
		fprintf(f, "%d", (int)setsid());
		int pid2 = fork();
		if (pid2 == 0) {
			lol.run();
		}
	}
}
