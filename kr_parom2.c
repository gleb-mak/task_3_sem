#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/shm.h>
#define F_RED		31
#define F_BLUE		34
#define reset_color()	printf("\033[0m")
#define set_display_color(color) 	printf("\033[%dm",color)

union semun {
      unsigned short  array[1000];
};

struct counts {
	int s;
	int n;
};

struct msgbuf {
    long type;
    char mtext[1];
};

int minus(int semid, int sem_num, int flg)
{
    struct sembuf semoparr[1];

    semoparr[0].sem_num = sem_num;
    semoparr[0].sem_op = -1;
    semoparr[0].sem_flg = 0;
    if (flg == IPC_NOWAIT)
    {
        semoparr[0].sem_flg = flg;
    }
    return semop(semid, semoparr, 1);
}

void plus(int semid, int sem_num)
{
    struct sembuf semoparr[1];

    semoparr[0].sem_num = sem_num;
    semoparr[0].sem_op = 1;
    semoparr[0].sem_flg = 0;
    if (semop(semid, semoparr, 1) == -1)
    {
        perror("plus");
        exit(1);
    }
}

void ship(int idmsg, int N, int semid, int shmid)
{
    struct counts * count = (struct counts *)shmat(shmid, NULL, 0);
    struct msgbuf buf;
    struct msgbuf msg[4];
	for (int i = 1; i < 5; i++)
	{
		msg[i - 1].type = i;
	}
	while (1)
	{
		printf("s count: %d\n", count->s);
		if (count->s < N)
		{
			return;
		}
		for (int i = 0; i < N; i++)
		{
			msgsnd(idmsg, &msg[0], 0, 0);
			printf("cap: car can enter from south.\n");
			msgrcv(idmsg, &buf, 1, 5, 0);
			printf("cap: car entered.\n");
		}
		set_display_color(F_RED);
		printf("cap: ship is full. Sailing away to north.\n");
		reset_color();
		for (int i = 0; i < N; i++)
		{
			msgsnd(idmsg, &msg[1], 0, 0);
			printf("cap: car can exit to north.\n");
			msgrcv(idmsg, &buf, 1, 5, 0);
			printf("cap: car exited.\n");
		}
		set_display_color(F_BLUE);
		printf("cap: all cars went out to north.\n");
		reset_color();
		if (count->n < N)
		{
			return;
		}
		for (int i = 0; i < N; i++)
		{
			msgsnd(idmsg, &msg[2], 0, 0);
			printf("cap: car can enter from north.\n");
			msgrcv(idmsg, &buf, 1, 5, 0);
			printf("cap: car entered.\n");
		}
        set_display_color(F_BLUE);
        printf("cap: ship is full. Sailing away to south.\n");
        reset_color();
		for (int i = 0; i < N; i++)
		{
			msgsnd(idmsg, &msg[3], 0, 0);
			printf("cap: car can exit to south.\n");
			msgrcv(idmsg, &buf, 1, 5, 0);
			printf("cap: car exited.\n");
		}
        set_display_color(F_RED);
        printf("cap: all cars went out to south.\n");
        reset_color();
	}
}

void south_car(int idmsg, int semid, int shmid)
{
	struct counts * count = (struct counts *)shmat(shmid, NULL, 0);
    struct msgbuf msg[3];//1 - can enter from south, 2 - can exit to north. 3 - answer to ship
    struct msgbuf buf;
	msg[0].type = 1;
	msg[1].type = 2;
	msg[2].type = 5;
    msgrcv(idmsg, &buf, 1, 1, 0);
    printf("car %d: going in from south.\n", getpid());
    msgsnd(idmsg, &msg[2], 0, 0);
    msgrcv(idmsg, &buf, 1, 2, 0);
	printf("car %d: going out to north.\n", getpid());
	msgsnd(idmsg, &msg[2], 0, 0);
	minus(semid, 0, 0);
	count->s--;
	plus(semid, 0);
}

void north_car(int idmsg, int semid, int shmid)
{
    struct counts * count = (struct counts *)shmat(shmid, NULL, 0);
    struct msgbuf msg[3];//1 - can enter from north, 2 - can exit to south. 3 - answer to ship
    struct msgbuf buf;
    msg[0].type = 3;
    msg[1].type = 4;
    msg[2].type = 5;
    msgrcv(idmsg, &buf, 1, 3, 0);
    printf("car %d: going in from north.\n", getpid());
    msgsnd(idmsg, &msg[2], 0, 0);
    msgrcv(idmsg, &buf, 1, 4, 0);
    printf("car %d: going out to north.\n", getpid());
	msgsnd(idmsg, &msg[2], 0, 0);
    minus(semid, 1, 0);
    count->n--;
    plus(semid, 1);
}

int main(int argc, char *argv[])
{
    int n = 0;
    int idmsg = 0;
    int N = atoi(argv[1]);
	int north = atoi(argv[2]);
	int south = atoi(argv[3]);
	struct counts *count;
    pid_t pid;
	pid_t pid_ship;
    int semid;
    int shmid;
    key_t shmkey = ftok("./mycp.c", 0);
    key_t semkey = ftok("./mycp.c", 1);
    union semun semset = {{1, 1}};
	setvbuf(stdout, NULL, _IONBF, 0);
    if ((semid = semget(semkey, 2, IPC_CREAT | 0600)) == -1)
    {
        perror("semget");
        exit(1);
    }
    if (semctl(semid, 0, SETALL, semset.array) == -1)
    {
        perror("semctl");
        exit(1);
    }
    if ((shmid = shmget(shmkey, 100000, IPC_CREAT | 0600)) == -1)
    {
        perror("shmget");
        exit(1);
    }
    count = (struct counts *)shmat(shmid, NULL, 0);
	count->s = south;
	count->n = north;
    if ((idmsg = msgget(IPC_PRIVATE, IPC_CREAT | 0600)) == -1)
    {
        perror("msgget");
        exit(1);
    }
    printf("creator:  Created message queue with id %d.\n", idmsg);
	pid_ship = fork();
	if (pid_ship == 0)
	{
		ship(idmsg, N, semid, shmid);
		return 0;
	}
    for (int i = 0; i < north + south; i++)
    {
        pid = fork();
		if (i < north)
		{
			if (pid == 0)
			{
				north_car(idmsg, semid, shmid);
				return 0;
			}
		}
		else
		{
			if (pid == 0)
			{
				south_car(idmsg, semid, shmid);
				return 0;
			}
		}
    }
    waitpid(pid_ship, &n, 0);
    msgctl(idmsg, IPC_RMID, 0);
    printf("creator:  Removed message queue with id %d.\n", idmsg);
    return 0;
}
