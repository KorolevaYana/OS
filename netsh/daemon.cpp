#include "daemon.h"
#define fst first
#define snd second


my_daemon::my_daemon(char* a) {
	port = atoi(a);
}

vector<pair<string, vector<string>>> parse_command(string line) {
	int cur = 0;
	vector<pair<string, vector<string>>> ans;
	while (line[cur] != '\n')	{
		if (line[cur] == ' ') {
			cur++;
		}
		string name;	
		vector<string> tmp;
		while (line[cur] != ' ') {
			name += line[cur++];
		}
		if (line[cur] == ' ') {
			cur++;
		}
		while (line[cur] != '|') {
		  string s;
			while (line[cur] != ' ') {
				s += line[cur];
				cur++;
			}
			tmp.push_back(s);
			cur++;
		}
		cur++;
	}
	return ans;
}

void my_daemon::run() {
	my_epoll just_epoll;
	
	my_socket meow("127.0.0.1", port, [](my_socket smth) {
				printf("Accepted!");
				return 0;
			});

	server listener(meow, 100, [this](my_socket tmp) {
				string cur_buf = tmp.additional;
				int flag = reading_n(tmp.get_fd(), cur_buf);
				if (flag == -1) {
					return 1;
				}
				if (tmp.reading_flag) {
					bool nl_flag = false;
					int i = 0;
					for (;i < (int)cur_buf.size(); i++) {
						if (cur_buf[i] == '\n') {
							nl_flag = true;
							break;
						}
					}
					if (nl_flag == true) {
						tmp.reading_flag = false;
						vector<pair<string, vector<string>>> prog = parse_command(tmp.additional);
						int in[2], out[2];
						pipe2(in, 0);
						tmp.pipe_in = in[0];
						int cur_out = 0;
						for (int i = 0; i < (int)prog.size(); i++) {
							pipe2(out, 0);
							int pid = fork();
							if (pid == 0) {
								dup2(in[0], cur_out);
								dup2(out[1], 1);
								char* argvv[prog[i].snd.size()];
								for (int j = 0; j < (int)prog[i].snd.size(); j++) {
									argvv[j] = (char*)prog[i].snd[j].c_str();
								}
								execv(prog[i].first.c_str(), argvv);
							}
							cur_out = out[1];
						}
						tmp.pipe_out = cur_out;
					} else {
						if (flag) {
							printf("Wrong input");
							return 1;
						} else {
							tmp.additional = cur_buf;
						}
					} 
				} else {
					int w_flag = writing(tmp.pipe_in, tmp.additional);
				}
			});

	if (listener.bind_heart()) {
		return;
	}
	just_epoll.add_server(listener);
	just_epoll.run();
}
