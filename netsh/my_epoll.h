#pragma once

#include <sys/epoll.h>
#include <functional>
#include <map>

struct my_epoll;

#include "read_write_lib.h"
#include "socket_lib.h"

struct my_epoll {
private:
	std::map<int, client> client_sockets;
	std::map<int, server> server_sockets;
  std::map<int, my_fd> fds;

	int cur_events;
	int fd;
	int magic_const;

public:
  my_epoll();  
 	~my_epoll();

	int add_client(client item, int flag);
	int add_server(server item);
  int add_fd(my_fd item);

  int delete_fd(my_fd tmp);
	int delete_client(client tmp);
	void run();
	void set_magic_const(int val);
};
