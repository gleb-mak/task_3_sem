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

void mywait(int semid, int semnum)
{
    struct sembuf semoparr[1];

    semoparr[0].sem_num = semnum;
    semoparr[0].sem_op = 0;
    semoparr[0].sem_flg = 0;
    if (semop(semid, semoparr, 1) == -1)
    {
        perror("wait");
        exit(1);
    }
}

void minus(int semid, int semnum)
{
    struct sembuf semoparr[1];

    semoparr[0].sem_num = semnum;
    semoparr[0].sem_op = -1;
    semoparr[0].sem_flg = 0;
    if (semop(semid, semoparr, 1) == -1)
	{
		perror("minus");
		exit(1);
	}
}

void plus(int semid, int semnum)
{
    struct sembuf semoparr[1];

    semoparr[0].sem_num = semnum;
    semoparr[0].sem_op = 1;
    semoparr[0].sem_flg = 0;
    if (semop(semid, semoparr, 1) == -1)
    {
        perror("plus");
        exit(1);
    }
}

int copy(FILE *src, char *ptr, int semid)
{
	minus(semid, 0);
	if (NULL == fgets(ptr, 1000, src))
	{
		plus(semid, 0);
		return 1;
	}
	minus(semid, 1);
	return 0;
}

int main(int argc, char* argv[])
{
	int shmid;
	int semid;
	char *shmptr;
	key_t shmkey = ftok("./mycat.c", 0);
	key_t semkey = ftok("./mycat.c", 1);

	if ((shmid = shmget(shmkey, 100000, 0600)) < 0)
	{
		perror("shmget");
		exit(1);
	}
	if ((semid = semget(semkey, 2, 0600)) < 0)
	{
		perror("semget");
		exit(1);
	}	
	if ((shmptr = shmat(shmid, NULL, 0)) < 0)
	{
		perror("shmat");
		exit(1);
	}
    /* if (semctl(semid, 1, SETVAL, 1) == -1) */
	/* { */
    /*     perror("semctl1"); */
    /*     exit(1); */
    /* } */
    /*  */
	if (argc == 1)
    {
        while(1)
        {
			FILE *file = fdopen(0, "r");
            copy(file, shmptr, semid);
        }
    }
    else
        {
			for (int i = 1; i < argc; ++i)
			{
				int fd = open(argv[i], O_RDONLY);
				if (fd < 0)
				{
					perror("open failed");
					exit(1);
				}
				FILE *file = fdopen(fd, "r");
				int is_end = 0;
				while (!is_end)
				{
					is_end = copy(file, shmptr, semid);
				}
				close(fd);
        }
    }
	if (shmdt(shmptr) == -1)
	{
		perror("shmdt");
		exit(1);
	}
    return 0; 
}

