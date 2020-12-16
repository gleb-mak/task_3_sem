#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/sem.h>

union semun {
	  unsigned short  array[1000];
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

void mywait(int semid, int sem_num)
{
    struct sembuf semoparr[1];
    
	semoparr[0].sem_num = sem_num;
    semoparr[0].sem_op = 0;
	semoparr[0].sem_flg = 0;
    if (semop(semid, semoparr, 1) == -1)
	{
		perror("wait");
		exit(1);
	}
}

void boat(int semid, int N, int S)
{
	for (int i = 0; i < S; i++)
	{
		plus(semid, 6);
		for (int j = 0; j < N + 1; j++)
		{
			plus(semid, 5);
		}
		plus(semid, 4);
		minus(semid, 3, IPC_NOWAIT);
		minus(semid, 2, IPC_NOWAIT);
		mywait(semid, 0); /* wait until ship is full */
		for (int i = 1; i < N + 1; i++)
		{
			minus(semid, i + 6, IPC_NOWAIT);
			plus(semid, 1);
		}
		plus(semid, 2); /* ledder up */
		printf("captain: There isn't free place on ship\n");
		printf("captain: Ledder was raised. Ship are floating out\n");
		sync();
		printf("captain: Ship arrived.\n");
		sync();
		minus(semid, 2, IPC_NOWAIT); /* ledder down */
		printf("captain: Ledder down.\n");
		plus(semid, 3);
		minus(semid, 4, IPC_NOWAIT);
		sync();
		minus(semid, 5, IPC_NOWAIT);
		mywait(semid, 5);
		minus(semid, 6, IPC_NOWAIT);
	}
	return;
}

void passenger(int i, int semid, int N, int S)
{
	for (int j = 0; j < S; j++) /* while day isn't ended */
	{
		mywait(semid, 2); /* Wait until ledder down */
		mywait(semid, 3);
		plus(semid, i + 6);
		minus(semid, 1, 0); /* Went on ledder  */
		printf("passanger %d: went to the ledder.\n", i);
		sync();
		if (minus(semid, i + 6, IPC_NOWAIT) == -1)
		{
			printf("passenger %d: I was kiccked out from ledder\n", i);
			minus(semid, 5, IPC_NOWAIT);
			mywait(semid, 6);
			continue;
		}
		else
		{
			plus(semid, i + 6);
		}
		if (minus(semid, 0, IPC_NOWAIT) == 0) /* If ship doesn't full, go into ship */
		{
			plus(semid, 1);
			printf("passanger %d: Went on ship.\n", i);
			sync();
			mywait(semid, 2); /* Wait until ledder down */
			mywait(semid, 4);
			plus(semid, 0); /* Go out from ship */
			printf("passenger %d: Went out from ship.\n", i);
			sync();
			plus(semid, i + 6);
			minus(semid, 1, 0);
			plus(semid, 1); /* Go out from ledder */
			minus(semid, i + 6, IPC_NOWAIT);
			printf("passenger %d: Went on ground\n", i);
			sync();
			minus(semid, 5, IPC_NOWAIT);
			mywait(semid, 6);
			continue;
		}
		else
		{
			printf("passenger %d: went on ground\n", i);
			sync();
			minus(semid, i + 6, IPC_NOWAIT);
			minus(semid, 5, IPC_NOWAIT);
			mywait(semid, 6);
			continue;
		}
	}
	return;
}

int main(int argc, char *argv[])
{
    int n = 0;
    int semid = 0;
    int N = atoi(argv[1]);
	int K = atoi(argv[2]);
	int M = atoi(argv[3]);
	int S = atoi(argv[4]);
    pid_t pid;
	union semun semset;
	semset.array[0] = K;
	semset.array[1] = M;
	semset.array[2] = 1; 
	semset.array[3] = 0;
	semset.array[4] = 0;
	semset.array[5] = 0;
	semset.array[6] = 0;
	for (int i = 0; i < N; i++)
	{
		semset.array[i + 7] = 0;
	}
	if ((semid = semget(IPC_PRIVATE, N + 7, IPC_CREAT | 0600)) == -1)
    {
        perror("semget");
		exit(1);
	}
	if (semctl(semid, 0, SETALL, semset.array) == -1)
	{
		perror("semctl1");
		exit(1);
	}
	for (int i = 0; i < N + 1; i++)
	{
		if ((pid = fork()) == -1)
		{
			perror("fork");
			exit(1);
		}
		if (pid == 0)
		{
			if (i != 0)
			{
				passenger(i, semid, N, S);
				return 0;
			}
			else
			{
				boat(semid, N, S);
				return 0;
			}
		}
	}
	for (int i = 0; i < N + 1; i++)
	{
		if (wait(&n) == -1)
		{
			perror("wait");
			exit(1);
		}
	}
	if (semctl(semid, IPC_RMID, 0) == -1)
	{
		perror("delete sem");
		exit(1);
	}
	printf("creator:  Removed semafor with id %d.\n", semid);
	return 0;
}

