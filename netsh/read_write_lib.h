#include <unistd.h>
#include <cstdio>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>

using namespace std;

int reading_n(int fd, string& buf);
int writing(int fd, string& buf);
int fd_reading_n(int fd, string& buf);
