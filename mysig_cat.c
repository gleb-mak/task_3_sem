#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#define SIZE 20000

int ch1;
int num;

void fun_0(int signo, siginfo_t *info, void *context)
{
    num++;
    if (num == 8)
    {
        printf("%c", ch1);
		fflush(stdout);
        num = 0;
        ch1 = 0;
    }
	kill(getppid(), SIGUSR1);
}

void fun_1(int signo, siginfo_t *info, void *context)
{
    ch1 += 1 << (7 - num);
    num++;
    if (num == 8)
    {
        printf("%c", ch1);
		fflush(stdout);
        num = 0;
        ch1 = 0;
    }
	kill(getppid(), SIGUSR1);
}

void empty_fun(int signo)
{

}

void char_to_sig(pid_t pid, char ch)
{
	char mask;
	char tmp;
	union sigval data;
	sigset_t set;
	int sig;
	struct sigaction act;
	act.sa_handler = empty_fun;
	sigemptyset(&set);
	sigaddset(&set, SIGUSR1);
	sigaction(SIGUSR1, &act, NULL);
	for (int i = 0; i < 8; i++)
	{
		mask = 1 << (7 - i); 
		tmp = ch & mask;
		if (tmp == 0)
		{
			sigaddset(&set, SIGUSR1);
			kill(pid, SIGUSR1);
			sigwait(&set, &sig);
		}
		else
		{
			sigaddset(&set, SIGUSR1);
			kill(pid, SIGUSR2);
			sigwait(&set, &sig);
        }
	}
}

int sig_to_char()
{
    struct sigaction act0, act1;
    act0.sa_flags = SA_SIGINFO;
    act0.sa_sigaction = fun_0;
    act1.sa_flags = SA_SIGINFO;
    act1.sa_sigaction = fun_1;
	sigaction(SIGUSR1, &act0, NULL);
	sigaction(SIGUSR2, &act1, NULL);
	kill(getppid(), SIGUSR1);
	while(1)
	{
		pause();
	}
}

void copy(int dst, int src, int pid)
{
	sigset_t set;
	int sig;
	struct sigaction act;
	act.sa_handler = empty_fun;
	sigemptyset(&set);
	sigaddset(&set, SIGUSR1);
	sigaction(SIGUSR1, &act, NULL);
	sigwait(&set, &sig);
	int size_r = 0;
	char buf[SIZE] = { 0 };
	if ((size_r = read(dst, buf, SIZE)) < 0)
	{   
		perror("read failed");
		exit(1);
	}
	for (int i = 0; i < size_r; i++)
	{
		char_to_sig(pid, buf[i]);
	}
}

int main(int argc, char *argv[])
{
	int fd = 0;
	pid_t pid = fork();
	if (pid < 0)
	{
		perror("fork");
		exit(1);
	}
	else if (pid > 0) // parent - reader
	{
		signal(SIGUSR1, empty_fun);
		if (argc == 2)
		{
			if ((fd = open(argv[1], O_RDWR)) < 0)
			{
				perror("open failed");
				exit(1);
			}
			copy(fd, 1, pid);
			close(fd);
		}
		else
		{
			printf("incorrect entered data. enter file name.\n");
			return 0;
		}
	}
	else // child - writer
	{
		sig_to_char();
	}
	return 0;
}
