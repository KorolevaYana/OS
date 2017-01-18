#include <string>
#include <vector>
#include <utility>
#include <functional>
#include <iostream>

using namespace std;

struct process;

struct arg {
  union val_variant {
    string* s;
    int i;
    pair<int, int>* p; 
    pair<function<process(int)>, int>* p_fi; 
  } val;

  enum arg_types {
    STRING,
    INT,
    P_INT,
    NOTHING,
    P_FUNC_INT
  } arg_t;

  arg(string* val, arg_types arg_t) {
    this->val.s = val;
    this->arg_t = arg_t;
  }

  arg(int val, arg_types arg_t) {
    this->val.i = val;
    this->arg_t = arg_t;
  }

  arg(pair<int, int>* val, arg_types arg_t) {
    this->val.p = val;
    this->arg_t = arg_t;
  }
  
  arg(pair<function<process(int)>, int>* val, arg_types arg_t) {
    this->val.p_fi = val;
    this->arg_t = arg_t;
  }
};

enum signal {
  READ_TAG,
  WRITE_TAG,
  EXIT_TAG,
  FORK_TAG,
  EXEC_TAG
};

struct process {
  signal name;
  arg args;
  function<process(int)> cont;

  process(signal name, arg args, function<process(int)> cont): 
    name(name), args(args), cont(cont) {}
};

struct closure {
  string* s;

  closure() {
    this->s = new string();
  }

  closure(string s) {
    this->s = new string(s);
  }
};

vector<pair<function<process(int)>, int>> processes;
void kernel(function<process(int)> name, int arg, vector<string>& std_in) {
  int cur_std_in = 0;
  processes.push_back(make_pair(name, arg));
  while(!processes.empty()) {
    pair<function<process(int)>, int> tmp = processes[processes.size() - 1];
    processes.pop_back();
    process current = tmp.first(tmp.second);
    switch (current.name) {
      case READ_TAG:
        if (current.args.arg_t != arg::STRING) {
          printf("Wrong syscall args: READ_TAG expected STRING.\n");
          break;
        }
        if (cur_std_in < (int)std_in.size()) {
    //      fprintf (stderr, "[debug] s = %p, '%s'\n", current.args.val.s, std_in[cur_std_in].c_str ());
          (*current.args.val.s) = std_in[cur_std_in]; 
  //        fprintf (stderr, "[debug] s = %p\n", current.args.val.s);
          processes.push_back(make_pair(current.cont, 1)); 
          cur_std_in++;
        } else {
          processes.push_back(make_pair(current.cont, 0));
        }
        break;
      case WRITE_TAG:
        if (current.args.arg_t != arg::STRING) {
          printf("Wrong syscall args: WRITE_TAG expected STRING.\n");
          break;
        }
        fprintf (stderr, "[debug] write s = %p\n", current.args.val.s);
        processes.push_back(make_pair(current.cont, printf("%s", (*current.args.val.s).c_str())));
        break;
      case EXIT_TAG:
        if (current.args.arg_t != arg::INT) {
          printf("Wrong syscall args: EXIT_TAG expected INT.\n");
          break;
        }
        printf("Exit code: %d\n", current.args.val.i);
        break;
      case FORK_TAG:
        if (current.args.arg_t != arg::NOTHING) {
          printf("Wrong syscall args: FORK_TAG expected NOTHING.\n");
          break;
        }
        cerr << "lalpar\n";
        processes.push_back(make_pair(current.cont, 1));
        cerr << "lalch\n";
        processes.push_back(make_pair(current.cont, 0));
        cerr << "lal\n";
        break;
      case EXEC_TAG:
        if (current.args.arg_t != arg::P_FUNC_INT) {
          printf("Wrong syscall args: EXEC_TAG expected PAIR_FUNC_INT.\n");
          break;
        }
        processes.push_back(*current.args.val.p_fi);
        break;
      default:
        printf("Wrong syscall!");
    }
  }  
}

process exit_func(int a){
  return process(EXIT_TAG, arg(0, arg::INT), exit_func);
}

process hello(int a) {
  closure cl;
  return process(READ_TAG, arg(cl.s, arg::STRING), [cl](int a) {
    return process(WRITE_TAG, arg(cl.s, arg::STRING), exit_func); 
  });
};

process fork_1(int a) {
  return process(FORK_TAG, arg(0, arg::NOTHING), [](int a) {
    string *p = new string("Parent\n");
    string *c = new string("Child\n");
    return a ? process(WRITE_TAG, arg(p, arg::STRING), exit_func) : process(WRITE_TAG, arg(c, arg::STRING), exit_func);
  });
};

function<process(int)> printer_maker(string s) {
  closure cl(s);
  return [cl](int a) {
    return process(WRITE_TAG, arg(cl.s, arg::STRING), exit_func);
  };
}

process exec_printer(int a) {
  pair<function<process(int)>, int> *p = new pair<function<process(int)>, int>(printer_maker("Hello3\n"), 0);
  return process(EXEC_TAG, arg(p, arg::P_FUNC_INT), exit_func);
}


int main() {
  vector<string> std_in;
  std_in.push_back("Hello\n");
  kernel(exec_printer, 0, std_in);
//  kernel(printer_maker("Hello2\n"), 0, std_in);
  return 0;
}




