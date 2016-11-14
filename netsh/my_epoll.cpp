#include "my_epoll.h" 

my_epoll::my_epoll() {
	this->fd = epoll_create(10);
	if (this->fd < 0) {
		printf("Can't create epoll socket: %m\n");
	} else {
		this->cur_events = 0;
		this->magic_const = 0;
	}
}

my_epoll::~my_epoll() {
	close(this->fd);
}
 
int my_epoll::add_client(client item, int flag) {
	struct epoll_event ev;

	ev.events = EPOLLRDHUP | EPOLLHUP | EPOLLET;
	if (flag == 0) {
		ev.events |= EPOLLIN;
	} else if (flag == 1) {
		ev.events |= EPOLLOUT;
	} else {
		printf("Invalid flag.\n");
		return 1;
	}
	
	int item_fd = item.get_heart().get_fd();

	ev.data.u64 = 0;
	ev.data.fd = item_fd;

	if (epoll_ctl(this->fd, EPOLL_CTL_ADD, item_fd, &ev) == -1) {
		printf("Can't add client. %m\n");
		return 1;
	}
	client_sockets[item_fd] = item;
	this->cur_events++;
	return 0;
}

int my_epoll::add_server(server item) {

	struct epoll_event ev;
	ev.events = EPOLLIN;

	int item_fd = item.get_heart().get_fd();

	ev.data.u64 = 0;
	ev.data.fd = item_fd;
	if (epoll_ctl(this->fd, EPOLL_CTL_ADD, item_fd, &ev) == -1) {
		printf("Can't add server. %m\n");
		return 1;
	}
	server_sockets[item_fd] = item;
	this->cur_events++;
	return 0;
}

int my_epoll::add_fd(my_fd item) {
  struct epoll_event ev;

//  fprintf (stderr, "add_fd ()\n");

	ev.events = EPOLLRDHUP | EPOLLHUP | EPOLLIN;
	
	int item_fd = item.get_fd();

	ev.data.u64 = 0;
	ev.data.fd = item_fd;

	if (epoll_ctl(this->fd, EPOLL_CTL_ADD, item_fd, &ev) == -1) {
		printf("Can't add fd. %m\n");
		return 1;
	}
	fds[item_fd] = item;
	this->cur_events++;
	return 0; 
}

int my_epoll::delete_client(my_socket* item) {
  printf("pipes: %d, %d", item->pipe_in, item->pipe_out);
  if (item->pipe_out != -1) {
    printf("[debug] close fd: %d\n", item->pipe_out);
    if (epoll_ctl(this->fd, EPOLL_CTL_DEL, item->pipe_out, 0) == -1) {
	  	printf("Can't delete fd. %m\n");
  		return 1;
	  }
  	fds.erase(item->pipe_out);
    close(item->pipe_out);
  }

  if (item->pipe_in != -1) {
    close(item->pipe_in);
  }
  
  printf ("[debug] close socket: %d\n", item->get_fd ());
	if (epoll_ctl(this->fd, EPOLL_CTL_DEL, item->get_fd(), 0) == -1) {
		printf("Can't delete client (%d from %d). %m\n", item->get_fd(), this->fd);
		return 1;
	}
  close(item->get_fd());
	client_sockets.erase(item->get_fd());
	return 0;
}

void my_epoll::set_magic_const(int val) {
	this->magic_const = val;
}

void my_epoll::run() { 
	struct epoll_event events[100];
  while (1) {
		int cnt = epoll_wait(fd, events, 100, -1);
		if (this->magic_const) {
	  	for (auto i = client_sockets.begin(); i != client_sockets.end(); i++) {
				close((*i).first);
			}
		  for (auto i = server_sockets.begin(); i != server_sockets.end(); i++) {
				close((*i).first);
			}
			
			server_sockets.clear();
			client_sockets.clear();
			break;
		}
		if (cnt == -1) { 
			if (errno != EINTR) {
				printf("Problems with epoll_wait: %m");
				return;
			} else {
				continue;
			}
		}

		for (int i = 0; i < cnt; i++) {
			int tmp_fd = events[i].data.fd;
			int flag = events[i].events;
//      printf("tmp_fd: %d, flag: %d\n", tmp_fd, flag);
			if (server_sockets.find(tmp_fd) != server_sockets.end()) {
				server tmp = server_sockets[tmp_fd];
				if (add_client(tmp.accept_heart(), 0)) {
					return;
				} 
			} else if (client_sockets.find(tmp_fd) != client_sockets.end()) {
				client &tmp = client_sockets[tmp_fd];
				
				if (flag & EPOLLIN) {
//          printf("state: %d, fd: %d\n", tmp.state, tmp.get_heart().get_fd());
          if (tmp.state == 0) {
            tmp.state = 1;
            int res = tmp.complete_task(tmp.get_heart());
            if (res == 2) {
              if (delete_client(&(tmp.get_heart()))) {
                return;
              }
            }
          } else {
            string buffer = "";
            int r = reading_n(tmp.get_heart().get_fd(), buffer);
//            fprintf (stderr, "[r=%d] buffer = (%lu) '%s'\n", r, buffer.length (), buffer.c_str ());
            writing(tmp.get_heart().pipe_in, buffer);
          }
          printf("EPOLLIN %d. pipein: %d, pipe_out: %d", tmp.get_heart().get_fd(), tmp.get_heart().pipe_in, tmp.get_heart().pipe_out);
        }        
        
        if (flag & EPOLLHUP) {  
	  			printf("Disconnected.\n");
  				if (delete_client(&(tmp.get_heart()))) {
					  return;
					}
        } 
			} else {
        my_fd tmp = fds[tmp_fd];
        if (flag & EPOLLIN) {
//          fprintf (stderr, "fds epollin\n");
          tmp.complete_task(tmp);
        }

        if (flag & EPOLLRDHUP) {
          fprintf(stderr, "EPOLLRDHUP: fd = %d\n", tmp.get_fd());
        }

        if ((flag & EPOLLHUP)) {
          fprintf(stderr, "EPOLLHUP: fd = %d\n", tmp.get_fd());
//          fprintf (stderr, "pipe in close: %d\n", tmp_socket.pipe_in);
          if (delete_client(tmp.get_from())) {
            return;
          }
        }
      }
		}
	}
}


