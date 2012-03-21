#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <unistd.h>
#include <fcntl.h>
#include <syslog.h>
#include <time.h>
#include <math.h>

int run_cpu();
int run_memory();
unsigned long long run_throughput();
int run_throughput_boost();

