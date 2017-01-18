READ_TAG = "read"
WRITE_TAG = "write"
EXIT_TAG = "exit"
FORK_TAG = "fork"
EXEC_TAG = "exec"
WAIT_TAG = "wait"

def kernel(main, args, stdin):
    print("Running process {} with args={}, stdin={}".format(main, args, stdin))
    processes = [(main, args, 0, -1)]
    nextpid = 1
    waiting = {}
    while processes:
        (prog, args, pid, parent) = processes.pop()
        (tag, sargs, cont) = prog(*args)
        print("   pid {}, tag = {}".format(pid, tag))
        if tag == READ_TAG:
            res = stdin[0]
            stdin = stdin[1:]
            processes.append((cont, [res], pid, parent))
        elif tag == WRITE_TAG:
            print('STDOUT: ', sargs[0])
            processes.append((cont, [], pid, parent))
        elif tag == EXIT_TAG:
            print("Exit code {}: {}".format(sargs[0], prog))
            if parent in waiting:
                processes.append(waiting[parent])
        elif tag == FORK_TAG:
            nextpid += 1
            processes.append((cont, [0], nextpid, pid))
            processes.append((cont, [1], pid, parent))
        elif tag == EXEC_TAG:
            processes.append((sargs[0], sargs[1:], pid, parent))
        elif tag == WAIT_TAG:
            waiting[pid] = (cont, [], pid, parent)
        else:
            print("ERROR: No such syscall")

def fork():
    return (FORK_TAG, [], fork_1)

def fork_1(flag):
    if flag:
        return (WAIT_TAG, [], contqqq)
    else:
        return (WRITE_TAG, ["Child"], fork_2)

def contqqq():
    return (WRITE_TAG, ["Parent"], fork_2)

def fork_2():
    return (EXIT_TAG, [0], None)
    
kernel(fork, [], [])


