#include <sys/types.h>
#include <sys/wait.h>
#include <cstdlib>
#include <vector>
#include <utility>
#include <string>
#include <fcntl.h>
#include <unistd.h>

#include "my_epoll.h"
#include "read_write_lib.h"

struct my_daemon {
	int port;

	my_daemon(char* a);
	void run();
};
