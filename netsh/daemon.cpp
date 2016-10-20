#include "daemon.h"
#define fst first
#define snd second


my_daemon::my_daemon(char* a) {
	port = atoi(a);
}

vector<pair<string, vector<string>>> parse_command(string line) {
//	printf("parse_command(line='%s')\n", line.c_str());
	int cur = 0;
	vector<pair<string, vector<string>>> ans;
	while (cur < (int)line.size() && line[cur] != '\n')	{
		if (line[cur] == ' ') {
			cur++;
		}
		string name;	
		vector<string> tmp;
		while (cur < (int)line.size() && line[cur] != ' ') {
			name += line[cur++];
		}
		if (cur < (int)line.size() && line[cur] == ' ') {
			cur++;
		}
		while (cur < (int)line.size() && line[cur] != '|') {
		  string s;
      bool flag = false;
			while (cur < (int)line.size()) {
        if (line[cur] == ' ' && !flag) {
          break;
        } else if (line[cur] == '\'') {
          flag = !flag;
          cur++;
        } else {
  				s += line[cur];
	  			cur++;
        }
			}
			tmp.push_back(s);
			cur++;
		}
		cur++;
		ans.push_back(make_pair(name, tmp));
	}
//	printf("ans size:%d, first:%s\n", (int)ans.size(), ans[0].first.c_str());
	return ans;
}

void execute(string &cur_buf, int nl, my_socket& tmp) {
  vector<pair<string, vector<string>>> prog = 
          parse_command(cur_buf.substr(0, nl));
  if (nl < (int)cur_buf.size() - 1) {
    tmp.additional = cur_buf.substr(nl + 1, cur_buf.size());
  } else {
    tmp.additional = "";
  }
  int in[2], out[2];
  pipe2(in, O_CLOEXEC);
  tmp.pipe_in = in[1];
//  fprintf (stderr, "[debug] pipe_in=%d\n", tmp.pipe_in);
  int cur_out = in[0];
//  printf("Meow\n");
  for (int i = 0; i < (int)prog.size(); i++) {
//    printf("command_name(%d): '%s'\n", i, prog[i].fst.c_str ());
//    for (int j = 0; j < prog[i].snd.size(); j++) 
//      printf("  arg: '%s'\n", prog[i].snd[j].c_str());
    pipe2(out, O_CLOEXEC);
    int pid = fork();
    if (pid == 0) {								
      dup2(cur_out, 0);
      dup2(out[1], 1);
      char* argvv[prog[i].snd.size() + 2];
      argvv[0] = (char*)prog[i].first.c_str ();
      for (int j = 0; j < (int)prog[i].snd.size(); j++) {
        argvv[j + 1] = (char*)prog[i].snd[j].c_str();
      }
      argvv[prog[i].snd.size() + 1] = 0;
      execvp(prog[i].first.c_str(), argvv);
      perror ("execvp");
      exit (1);
    }
    close (cur_out);
    close (out[1]);
    cur_out = out[0];
  }

  tmp.pipe_out = cur_out;  
  fcntl(tmp.pipe_in, F_SETFL, fcntl(tmp.pipe_in, F_GETFL) | O_NONBLOCK);
  fcntl(tmp.pipe_out, F_SETFL, fcntl(tmp.pipe_out, F_GETFL) | O_NONBLOCK);
  return;
}

void my_daemon::run() {
	my_epoll just_epoll;
	
	my_socket meow("127.0.0.1", port, [](my_socket smth) {
    printf("Accepted!\n");
    return 0;
  }, &just_epoll);

	server listener(meow, 100, [this](my_socket &tmp) {
    string cur_buf = tmp.additional;
    int flag = reading_n(tmp.get_fd(), cur_buf);
    if (flag == -1) {
      return 2;
    }
//    fprintf (stderr, "tmp.reading_flag = %d\n", tmp.reading_flag);
    if (tmp.reading_flag) {
      bool nl_flag = false;
      int nl = 0;
      int i = 0;
      for (;i < (int)cur_buf.size(); i++) {
//        printf("char: %d\n", cur_buf[i]);
        if (cur_buf[i] == '\r')
          cur_buf[i] = ' ';
        if (cur_buf[i] == '\n') {
          nl_flag = true;
          nl = i;
          break;
        }
      }
//      fprintf (stderr, "nl_flag = %s\n", nl_flag ? "true" : "false");
      if (nl_flag == true) {
        tmp.reading_flag = false;
        execute(cur_buf, nl, tmp);

        my_epoll* epoll = tmp.get_epoll();
        my_fd tmp_fd(tmp.pipe_out, [](my_fd& fd) {
          string buffer = "";
          if (fd_reading_n(fd.get_fd(), buffer) == -1) {
            return 1;
          } else {
            if (writing(fd.get_from()->get_fd(), buffer) == -1) {
              return 1;
            }
          }
          return 0;
        }, &tmp);

        epoll->add_fd(tmp_fd);
      } else {
        if (flag) {
          printf("Wrong input.\n");
          return 2;
        } else {
          tmp.additional = cur_buf;
        }
      } 
    }
    return 0;
  });

	if (listener.bind_heart()) {
		return;
	}
	just_epoll.add_server(listener);
	just_epoll.run();
}


