#include "socket_lib.h"

my_fd::my_fd() {}

my_fd::my_fd(int fd, function<int(my_fd&)> task, my_socket* from) {
  this->fd = fd;
  this->task = task;
  this->from = from;
}

int my_fd::get_fd() {
  return this->fd;
}

function<int(my_fd&)> my_fd::get_task() {
  return this->task;
}

my_socket* my_fd::get_from() {
  return this->from;
}

int my_fd::complete_task(my_fd& fd) {
  int ans = this->task(fd);
  if (ans == 1) {
    printf("Can't complete task.\n");
  }
  return ans;
}

my_fd& my_fd::operator =(const my_fd& other) {
  this->fd = other.fd;
  this->task = other.task;
  this->from = other.from;
  return *this;
}
