#pragma once

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstdio>
#include <functional>

struct client;
struct server;
struct my_fd;
struct my_socket;

#include "my_epoll.h"

using namespace std;

struct my_socket {
private:
	int fd;
	sockaddr_in addr;
	char* ip;
	int port;
	function<int(my_socket&)> task;
  my_epoll* epoll; 

public:
	string additional;
	bool reading_flag;
	int pipe_in, pipe_out;

  my_socket();
	my_socket(my_epoll* epoll);
	my_socket(const my_socket& other);
  my_socket(const char* ip, int port, 
			function<int(my_socket&)> task, my_epoll* epoll);

	~my_socket();

  void set_epoll(my_epoll* epoll);

	int	 complete_task(my_socket &task_arg);
	my_socket& operator =(const my_socket& other);
  
  my_epoll* get_epoll();
	int get_fd() const;
	void close_socket();

	friend client;
	friend server;
};

struct my_fd {
private:
  int fd;
  function<int(my_fd&)> task;
  my_socket* from;
public:
  my_fd();
  my_fd(int fd, function<int(my_fd&)> task, my_socket* from);
  
  int get_fd();
  function<int(my_fd&)> get_task();
  my_socket* get_from();
  
  int complete_task(my_fd& fd);
  my_fd& operator =(const my_fd& other);
};

struct client {
private:
	my_socket heart;
public:
  int state;
	client();
	client(const my_socket& heart);
  ~client();

	int connect_heart();	
	int complete_task(my_socket &task_arg);
	my_socket& get_heart();
};



struct server {
private:
	my_socket heart;
	function<int(my_socket&)> set_task;
	int max_connected;

public:
	server();
	server(const my_socket& heart, int max_connected,
			function<int(my_socket&)> set_task);
	~server();
	int bind_heart();
	client accept_heart();
	my_socket& get_heart();
};
