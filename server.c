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

union semun {
      unsigned short  array[2];
};

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

void minus(int semid, int sem_num)
{
    struct sembuf semoparr[1];

    semoparr[0].sem_num = sem_num;
    semoparr[0].sem_op = -1;
    semoparr[0].sem_flg = 0;
    if (semop(semid, semoparr, 1) == -1)
	{
		perror("minus");
		exit(1);
	}
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

int main(int argc, char *argv[])
{
    int semid;
	int shmid;
	char *shmptr;
	key_t shmkey = ftok("./mycat.c", 0);
    key_t semkey = ftok("./mycat.c", 1);
    union semun semset = {{0, 1}};
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
    if ((shmptr = shmat(shmid, NULL, 0)) < 0)
    {
        perror("shmat");
        exit(1);
    }
	FILE *file = fdopen(1, "w");
	while (1)
	{
		plus(semid, 0);
		mywait(semid, 1);
		usleep(2000000);
		//fputs(shmptr, file);
		printf("%s", shmptr);
		sync();
		plus(semid, 1);
	}
    if (semctl(semid, IPC_RMID, 0) == -1)
    {
        perror("delete sem");
        exit(1);
    }
    if (shmctl(shmid, IPC_RMID, 0) == -1)
    {
        perror("delete shm");
        exit(1);
    }
	return 0;
}

