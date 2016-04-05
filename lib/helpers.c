#include "helpers.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

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

execargs_t* new_exec(char** args) {
	int args_len = 0;
	while (args[args_len] != NULL) args_len++;

	char ** tmp_args = (char**)malloc(sizeof(char*) * (args_len + 1));
	if (tmp_args == NULL) {
		return NULL;
	}
	
	for (int i = 0; i < args_len; i++) {
		int j = 0;
		tmp_args[i] = malloc(sizeof(char) * (strlen(args[i]) + 1));
		for (int k = 0; k <= strlen(args[i]); k++) {
			tmp_args[i][k] = args[i][k];
		}
	}
	tmp_args[args_len] = NULL;
	execargs_t* tmp = malloc(sizeof(execargs_t));

	tmp->args = tmp_args;
	return tmp;
}

void free_exec(execargs_t* args) {
	int i = 0;
	while(args->args[i] != NULL) {
		free(args->args[i]);
		i++;
	}
	free(args->args);
}

int exec(execargs_t* args) {
	return execvp(args->args[0], args->args);
}

int runpiped(execargs_t** programs, size_t n) {
	struct {
		int fd[2];
	}	pipes[n - 1];

	pid_t pids[n];

	for (int i = 0; i < n - 1; i++) {
		int tmp = pipe2(pipes[i].fd, O_CLOEXEC);
		printf("%i\n", i);
		if (tmp < 0) {
			printf("Pipe troubles\n");
			for (int j = 0; j < i; j++) {
				close(pipes[j].fd[0]);
				close(pipes[j].fd[1]);
			}
			return 1;
		}
	}

	for (int i = 0; i < n; i++) {
		printf("%d\n", i);
		int tmp_pid = fork();
		if (tmp_pid == 0) {
			if (i > 0) {
				dup2(pipes[i - 1].fd[1], STDIN_FILENO);
			}
			if (i < n - 1) {
				dup2(pipes[i + 1].fd[0], STDOUT_FILENO);
			}
			exec(programs[i]);
		} else if (tmp_pid > 0) {
			pids[i] = tmp_pid;
		} else {
			break;
		}
	}

	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGINT);
	sigaddset(&set, SIGCHLD);

	sigset_t tmp_set;
	sigprocmask(SIG_BLOCK, &set, &tmp_set);

	siginfo_t info;
	char flag = 1;
	int cnt = 0;
	while(flag) {
		sigwaitinfo(&set, &info);
		if (info.si_signo == SIGCHLD) {
				int res;
			while ((res = waitpid(-1, 0, WNOHANG)) > 0) {
			  for (int i = 0; i < n; i++) {
					if (pids[i] == res) {
						pids[i] = 0;
						break;
					}
				}
				cnt++;
				if (cnt == n) {
					flag = 0;
					break;
				}
			}
		} else if (info.si_signo == SIGINT) {
			break;
		}
	}

	for (int i = 0; i < n; i++) {
		if (pids[i] > 0) {
			kill(pids[i], SIGKILL);
			waitpid(pids[i], 0, 0);
		
		}
	}
	

	sigprocmask(SIG_SETMASK, &tmp_set, 0);
	return 1;
}








