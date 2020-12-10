#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#define SIZE 20000

int copy(int src, int dst)
{
	int size_r = 0;
	int size_tmp = 0;
	char buf[SIZE];
	if ((size_r = read(src, buf, SIZE)) < 0)
    {
		perror("read failed");
		exit(1);
    }
	while (size_r > 0)
	{
        if ((size_tmp = write(dst, buf, size_r)) < 0)
        {
            perror("write failed");
			exit(1);
		}
		size_r -= size_tmp;
	}
	return 0;
}

int main(int argc, char* argv[])
{
	int fd = 0;
	int flag = O_RDWR;
	if (argc == 1)
	{
		while(1)
		{
			copy(0, 1);
		}
	}
	else
    {
		for (int i = 1; i < argc; ++i)
		{
      		if ((fd = open(argv[i], flag)) < 0)
			{
				perror("open failed");
				exit(1);
			}
            copy(fd, 1);
			close(fd);
		}
	}
    return 0;
}

