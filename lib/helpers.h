#define _GNU_SOURCE
#define _POSIX_SOURCE

#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

typedef struct {
	char* func;
	char** args;
} execargs_t;

ssize_t read_(int fd, void *buf, size_t count);
ssize_t write_(int fd, const void *buf, size_t count);

ssize_t read_until(int fd, void *buf, size_t count, char delimiter);

int spawn(const char* file, char* const argv[]);

execargs_t* new_exec(char* func, char** args);
void free_exec(execargs_t* args);
int exec(execargs_t* args);
int runpiped(execargs_t** programs, size_t n);
