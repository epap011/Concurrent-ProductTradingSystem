#### How to build the program ###
command: make OR make build


### How to run the program ###
command: a.out N
command: a.out N --debug_mode

### How to clean object files ###
command: make clean

### Recommendations ###
If debug_mode occurs it is highly recommended to give
a small number of N(3, 6, 9) because bigger numbers
produce too much information(chaos).If you enter a bigger N(>15)
be sure to increase the maximum number of lines that terminal keeps
in its buffer.In most cases default is something like 10000 lines.


The program works, but fast re-runs with N of Ω(100) the program may trigger the following assertion
"pthread_mutex_lock.c:94: _pthread_mutex_lock: Assertion `mutex->data.__owner == 0' failed"