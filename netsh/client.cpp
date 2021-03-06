#include "socket_lib.h"

client::client() : state(0) {}

client::client(const my_socket& heart): heart(heart), state(0) {}

client::~client() {
}

int client::connect_heart() {
	if (connect(this->heart.fd, 
				(sockaddr*)&(this->heart.addr), sizeof(this->heart.addr)) < 0) {
		printf("Can't connect.%m\n");
		return 1;
	} 
	return 0;
}

int client::complete_task(my_socket &task_arg) {
	return this->heart.complete_task(task_arg);
}

my_socket& client::get_heart() {
	return this->heart;
}
