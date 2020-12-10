#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>

int main(int argc, char *argv[])
{
	struct timespec t1;
    struct timespec t2;
	long long int dt = 0;
	int stat;
	char *argv_new[argc];
	for (int i = 1; i < argc; i++)
	{
		argv_new[i - 1] = argv[i];
	}
	argv_new[argc - 1] = 0;
	pid_t pid;
	clock_gettime(CLOCK_REALTIME, &t1);
	pid = fork();
	if (pid == 0)
	{
		execvp(argv[1], argv_new);
		return 0;
	}
	wait(&stat);
	clock_gettime(CLOCK_REALTIME, &t2);
	dt = (t2.tv_sec - t1.tv_sec) * 1000000000 + (t2.tv_nsec - t1.tv_nsec);
	printf("%lld sec  %lld microsec\n", dt / 1000000000, (dt % 1000000000) / 1000);
	return 0;
}
