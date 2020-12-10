#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <mqueue.h>
#include <sys/wait.h>
#define SIZE 20

struct mq_data
{
	int size;
	char buf[10];
};

int main(int argc, char *argv[])
{
	mqd_t mqd1, mqd2;
	int shm_fd, fd;
	pid_t pid;
    if ((mqd1 = mq_open("/queue1", O_RDWR | O_CREAT, 0600, NULL)) == -1)
    {
        perror("mq_open1");
        exit(1);
    }
    if ((mqd2 = mq_open("/queue2", O_RDWR | O_CREAT)) == -1)
    {
        perror("mq_open2");
        exit(1);
    }
	if ((shm_fd = shm_open("/memory", O_RDWR | O_CREAT, 0600)) == -1)
	{
		perror("shm_open");
		exit(1);
	}
	if (ftruncate(shm_fd, 100) == -1)
	{
		perror("ftruncate");
		exit(1);
	}
	if (argc == 1)
	{
		fd = 0;
	}
	else 
	{
		if ((fd = open(argv[1], O_RDWR)) < 0)
		{
			perror("open failed");
			exit(1);
		}
	}
	pid = fork();
	if (pid == 0) //child - reader
	{
		struct mq_data *ptr;
		int size_r;
		char buf[10];
		void *msg;
		struct mq_attr attr;
		int prior = 1;
		mq_getattr(mqd2, &attr);
		msg = malloc(attr.mq_msgsize);
		ptr = mmap(NULL, 100, PROT_WRITE, MAP_SHARED, shm_fd, 0); // connect shared memory
		while (prior == 1)
		{
			mq_receive(mqd2, msg, attr.mq_msgsize, NULL); // get msg that we can write
			size_r = read(fd, buf, 10);
			ptr->size = size_r;
			if (size_r == 0)
			{
				prior = 2;
			}
			for (int i = 0; i < size_r; i++)
			{
				ptr->buf[i] = buf[i];
			}
			mq_send(mqd1, msg, 0, prior); // send that parent can read
		}
		if (fd != 0)
		{
			close(fd);
		}
		if (mq_close(mqd1) == -1)
		{
			perror("mq_close");
			exit(1);
		}
        if (mq_close(mqd2) == -1)
        {
            perror("mq_close");
            exit(1);
        }
		close(shm_fd);
		free(msg);
		return 0;
	}
	else //parent - writer
	{
		struct mq_data *ptr;
		unsigned int prior;
		void *msg;
		struct mq_attr attr;
		mq_getattr(mqd1, &attr);
		msg = malloc(attr.mq_msgsize);
		ptr = mmap(NULL, 100, PROT_READ, MAP_SHARED, shm_fd, 0);
		mq_send(mqd2, msg, 0, 1);
		while(1)
		{
			mq_receive(mqd1, msg, attr.mq_msgsize, &prior); //get msg that we can read from memory
			if (prior == 2)
			{
				int n;
				wait(&n);
				if (mq_close(mqd1) == -1)
				{
					perror("mq_close");
					exit(1);
				}
				if (mq_unlink("/queue1") == -1)
				{
					perror("mq_unlink");
					exit(1);
				}
				if (mq_close(mqd2) == -1)
                {
                    perror("mq_close");
                    exit(1);
                }
                if (mq_unlink("/queue2") == -1)
                {
                    perror("mq_unlink");
                    exit(1);
                }
				close(shm_fd);
				if (shm_unlink("/memory") == 1)
				{
					perror("shm_unliink");
					exit(1);
				}
				free(msg);
				return 0;
			}
			write(1, ptr->buf, ptr->size);
			mq_send(mqd2, msg, 0, 1); //send msg that child can write in memory
		}
	}
}
