#include <string>
#include <cstring>
#include <vector>
#include <utility>
#include <functional>
#include <tuple>
#include <sys/mman.h>

#define PAGE_SIZE 1024

using namespace std;


struct process;

struct arg {
  union val_variant {
    string* s;
    int i;
    pair<int, int>* p;
    pair<function<process(int)>, int>* p_fi; 
    void* v;
  } val;

  enum arg_types {
    STRING,
    INT,
    P_INT,
    NOTHING,
    P_FUNC_INT,
    VOID
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

  arg(void* val, arg_types arg_t) {
    this->val.v = val;
    this->arg_t = arg_t;
  }
};

enum signal {
  READ_TAG,
  WRITE_TAG,
  EXIT_TAG,
  FORK_TAG,
  EXEC_TAG,
  MMAP_TAG,
  UNMAP_TAG
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

vector<tuple<function<process(int)>, int, vector<tuple<void*, int, int>>>> processes;
void kernel(function<process(int)> name, int arg, vector<string>& std_in) {
  int cur_std_in = 0;
  processes.push_back(make_tuple(name, arg, vector<tuple<void*, int, int>>()));
  while(!processes.empty()) {
    auto tmp = processes[processes.size() - 1];
    processes.pop_back();
    process current = (get<0>(tmp))(get<1>(tmp));
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
          processes.push_back(make_tuple(current.cont, 1, get<2>(tmp))); 
          cur_std_in++;
        } else {
          processes.push_back(make_tuple(current.cont, 0, get<2>(tmp)));
        }
        break;
      case WRITE_TAG:
        if (current.args.arg_t != arg::STRING) {
          printf("Wrong syscall args: WRITE_TAG expected STRING.\n");
          break;
        }
//        fprintf (stderr, "[debug] write s = %p\n", current.args.val.s);
        processes.push_back(make_tuple(current.cont, printf("%s", (*current.args.val.s).c_str()), get<2>(tmp)));
        break;
      case EXIT_TAG:
        if (current.args.arg_t != arg::INT) {
          printf("Wrong syscall args: EXIT_TAG expected INT.\n");
          break;
        }
        printf("Exit code: %d\n", current.args.val.i);
        break;
      case FORK_TAG:
        {
          if (current.args.arg_t != arg::NOTHING) {
              printf("Wrong syscall args: FORK_TAG expected NOTHING.\n");
            break;
          }
          vector<tuple<void*, int, int>> par_mem = get<2>(tmp);
          auto ch_mem = par_mem;
          for (int i = 0; i < (int)par_mem.size(); i++) {
            auto p = par_mem[i];
            if (get<1>(p)) { //anonymous
              void* tmp_p = mmap(NULL, get<2>(p) * PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
              memcpy(tmp_p, get<0>(p), get<2>(p) * PAGE_SIZE);
              ch_mem[i] = make_tuple(tmp_p, get<1>(p), get<2>(p));
            } // else -- shared
          }
          processes.push_back(make_tuple(current.cont, 1, par_mem));
          processes.push_back(make_tuple(current.cont, 0, ch_mem));
          break;
        }
      case EXEC_TAG:
        if (current.args.arg_t != arg::P_FUNC_INT) {
          printf("Wrong syscall args: EXEC_TAG expected PAIR_FUNC_INT.\n");
          break;
        }
        processes.push_back(make_tuple(current.args.val.p_fi->first, current.args.val.p_fi->second, get<2>(tmp)));
        break;
      case MMAP_TAG:
        {
          if (current.args.arg_t != arg::P_INT) {
            printf("Wrong syscall args: MMAP_TAG expected P_INT.\n");
            break;
          }
          void* tmp_p = mmap(NULL, current.args.val.p->first * PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
          if (tmp_p != (void*)-1) {
            auto tmp_v = get<2>(tmp);
            tmp_v.push_back(make_tuple(tmp_p, current.args.val.p->second, current.args.val.p->first));
            processes.push_back(make_tuple(current.cont, 1, tmp_v));
          }
          else 
            processes.push_back(make_tuple(current.cont, 0, get<2>(tmp)));
          break;
        }
      case UNMAP_TAG:
        {
          if (current.args.arg_t != arg::VOID) {
            printf("Wrong syscall args: UNMAP_TAG expected VOID.\n");
            break;
          }
          auto tmp_v = get<2>(tmp);
          for (auto i = tmp_v.begin(); i != tmp_v.end(); i++) {
            if (get<0>(*i) == current.args.val.v) {
              tmp_v.erase(i);
              break;
            }
          }
          processes.push_back(make_tuple(current.cont, 1, tmp_v));
          break;
        }
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

process mapping(int a) {
  pair<int, int> *p = new pair<int, int>(4, 1);
  return process(MMAP_TAG, arg(p, arg::P_INT), exit_func);        
}

int main() {
  vector<string> std_in;
  std_in.push_back("Hello\n");
  kernel(exec_printer, 0, std_in);
//  kernel(printer_maker("Hello2\n"), 0, std_in);
//  kernel(mapping, 0, std_in);
  return 0;
}




