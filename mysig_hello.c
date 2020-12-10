#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/sem.h>

void hello()
{
	printf("\nhello!\n");
}

int main()
{
	signal(SIGINT, hello);
	getchar();
	return 0;
}
