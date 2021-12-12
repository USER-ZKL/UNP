#ifndef UNPH
#define UNPH
#include<sys/types.h>
typedef void Sigfunc(int);
Sigfunc* mysignal(int signo,Sigfunc *func);
ssize_t readline(int,void*,size_t);
ssize_t Writen(int,const void*,size_t);
#endif
