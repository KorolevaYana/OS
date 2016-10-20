.PHONY: all

all: netsh

netsh: read_write_lib.h my_epoll.h socket_lib.h daemon.h 

SOURCES := netsh.cpp daemon.cpp read_write_lib.cpp server.cpp client.cpp my_socket.cpp my_epoll.cpp my_fd.cpp

netsh: $(SOURCES)
	g++ -Wall -Wextra -std=c++11 -g -o $@ $(SOURCES)

