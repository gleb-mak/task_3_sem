#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#define F_RED		31
#define F_BLUE		34
#define reset_color()	printf("\033[0m")
#define set_display_color(color) 	printf("\033[%dm",color)

struct msgbuf {
    long type;
    char mtext[1];
};

void ship(int idmsg, int N)
{
    struct msgbuf buf;
    struct msgbuf msg[4];
	for (int i = 1; i < 5; i++)
	{
		msg[i - 1].type = i;
	}
	while (1)
	{
		for (int i = 0; i < N; i++)
		{
			msgsnd(idmsg, &msg[0], 1, 0);
			printf("cap: car can enter from south.\n");
			sync();
			msgrcv(idmsg, &buf, 1, 5, 0);
			printf("cap: car entered.\n");
			sync();
		}
		set_display_color(F_RED);
		printf("cap: ship is full. Sailing away to north.\n");
		sync();
		reset_color();
		for (int i = 0; i < N; i++)
		{
			msgsnd(idmsg, &msg[1], 1, 0);
			printf("cap: car can exit to north.\n");
			sync();
			msgrcv(idmsg, &buf, 1, 5, 0);
			printf("cap: car exited.\n");
			sync();
		}
		set_display_color(F_BLUE);
		printf("cap: all cars went out to north.\n");
		sync();
		reset_color();
		for (int i = 0; i < N; i++)
		{
			msgsnd(idmsg, &msg[2], 1, 0);
			printf("cap: car can enter from north.\n");
			sync();
			msgrcv(idmsg, &buf, 1, 5, 0);
			printf("cap: car entered.\n");
			sync();
		}
        set_display_color(F_BLUE);
        printf("cap: ship is full. Sailing away to south.\n");
        sync();
        reset_color();
		for (int i = 0; i < N; i++)
		{
			msgsnd(idmsg, &msg[3], 1, 0);
			printf("cap: car can exit to south.\n");
			sync();
			msgrcv(idmsg, &buf, 1, 5, 0);
			printf("cap: car exited.\n");
			sync();
		}
        set_display_color(F_RED);
        printf("cap: all cars went out to south.\n");
        sync();
        reset_color();
	}
}

void south_car(int idmsg)
{
    struct msgbuf msg[3];//1 - can enter from south, 2 - can exit to north. 3 - answer to ship
    struct msgbuf buf;
	msg[0].type = 1;
	msg[1].type = 2;
	msg[2].type = 5;
    msgrcv(idmsg, &buf, 1, 1, 0);
    printf("car %d: going in from south.\n", getpid());
    sync();
    msgsnd(idmsg, &msg[2], 1, 0);
    msgrcv(idmsg, &buf, 1, 2, 0);
	printf("car %d: going out to north.\n", getpid());
	sync();
	msgsnd(idmsg, &msg[2], 1, 0);
	//north_car(idmsg);
}

void north_car(int idmsg)
{
    struct msgbuf msg[3];//1 - can enter from north, 2 - can exit to south. 3 - answer to ship
    struct msgbuf buf;
    msg[0].type = 3;
    msg[1].type = 4;
    msg[2].type = 5;
    msgrcv(idmsg, &buf, 1, 3, 0);
    printf("car %d: going in from north.\n", getpid());
    sync();
    msgsnd(idmsg, &msg[2], 1, 0);
    msgrcv(idmsg, &buf, 1, 4, 0);
    printf("car %d: going out to north.\n", getpid());
    sync();
	msgsnd(idmsg, &msg[2], 1, 0);
    //south_car(idmsg);
}

int main(int argc, char *argv[])
{
    int n = 0;
    int idmsg = 0;
    int N = atoi(argv[1]);
	int north = atoi(argv[2]);
	int south = atoi(argv[3]);
    pid_t pid;
    if ((idmsg = msgget(IPC_PRIVATE, IPC_CREAT | 0600)) == -1)
    {
        perror("msgget");
        exit(1);
    }
    printf("creator:  Created message queue with id %d.\n", idmsg);
	pid = fork();
	if (pid == 0)
	{
		ship(idmsg, N);
		return 0;
	}
    for (int i = 0; i < north + south; i++)
    {
        pid = fork();
		if (i < north)
		{
			if (pid == 0)
			{
				north_car(idmsg);
				return 0;
			}
		}
		else
		{
			if (pid == 0)
			{
				south_car(idmsg);
				return 0;
			}
		}
    }
    for (int i = 0; i < north + south + 1; i++)
    {
        wait(&n);
    }
    msgctl(idmsg, IPC_RMID, 0);
    printf("creator:  Removed message queue with id %d.\n", idmsg);
    return 0;
}
