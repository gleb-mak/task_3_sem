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
#include <termios.h>
#include <sys/ioctl.h>
#define ESC "\033"

int count;

void print_frame()
{
	struct winsize win;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &win);
	int col = win.ws_col;
	int row = win.ws_row;
	for (int i = 0; i < col; i++)
	{
		printf("*");
	}
	for (int i = 0; i < row - 3; i++)
	{
		printf("\n*");
		for (int j = 0; j < col - 2; j++)
		{
			printf(" ");
		}
		printf("*");
	}
	printf("\n");
	for (int i = 0; i < col; i++)
	{
		printf("*");
	}
	printf(ESC "[s");
	printf(ESC "[%dA", row/2);
	printf(ESC "[%dD", col/2);
	printf("%d * %d", col, row);
	printf(ESC "[u");
	printf("\n");
}

void mysig()
{
	print_frame();
	count++;
	return;
}

void hello()
{
    printf("\nhello\n");
	return;
}

int main()
{
	signal(SIGINT, hello);
    signal(SIGWINCH, mysig);
	getchar();
	if (count == 0)
	{
		print_frame();
	}
    return 0;
}

