#include "socket_lib.h"

my_socket::my_socket(): additional(""), reading_flag(true), pipe_in(-1), pipe_out(-1){}

my_socket::my_socket(my_epoll* epoll): epoll(epoll), additional(""), reading_flag(true), pipe_in(-1), pipe_out(-1){}

my_socket::my_socket(const my_socket& other) : epoll(other.epoll), additional(""), reading_flag(other.reading_flag), pipe_in(-1), pipe_out(-1){
	this->fd = other.fd;
	this->addr = other.addr;
	this->task = other.task;
}

my_socket::my_socket(const char* ip, int port,
		std::function<int(my_socket&)> task, my_epoll* epoll) : epoll(epoll), additional(""), reading_flag(true), pipe_in(-1), pipe_out(-1) {
  this->fd = socket(AF_INET, SOCK_STREAM, 0);
  if (this->fd == -1) {
		printf("Problems with creating socket\n");
	} else {
		this->addr.sin_family = AF_INET;
		inet_aton(ip, &(this->addr).sin_addr);
		addr.sin_port = htons(port);
	  this->task = task;
	}
}

my_socket::~my_socket() {
}

void my_socket::set_epoll(my_epoll* epoll) {
  this->epoll = epoll;
}

my_epoll* my_socket::get_epoll() {
  return this->epoll;
}

void my_socket::close_socket() {
	close(this->fd);
}

int my_socket::complete_task(my_socket &task_arg) {
	int ans = this->task(task_arg);
	if (ans == 1) {
		printf("Can't complete task.\n");
	} 
	return ans;
}

my_socket& my_socket::operator =(const my_socket& other) {
	this->fd = other.fd;
	this->addr = other.addr;
	this->task = other.task;
  this->epoll = other.epoll;
  return *this;
}

int my_socket::get_fd() const {
	return this->fd;
}

