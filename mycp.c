#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>

int copy(int src, int dst)
{
        int size_r = 0;
        int size_tmp = 0;
        char buf[10000];
        if ((size_r = read(src, buf, 10000)) < 0)
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
	int fs = 0;
	int opt = 0;
        int option_index = 0;
	char *file_names[2];
	int j = 0;
	char ans = 0;
	int is_f_opt = 0;
	int is_i_opt = 0;
	int is_v_opt = 0;
        static struct option long_options[] = {
        	{"verbose", no_argument, 0, 'v'},
                {"force", no_argument, 0, 'f'},
                {"interactive", no_argument, 0, 'i'}
	};
	for (int i = 1; i < argc; i++)
	{
		if (argv[i][0] == '-')
		{
			continue;
		}
		file_names[j] = argv[i];
		j++;
	}
	if ((fs = open(file_names[0], O_RDWR)) < 0)
        {
                perror("open source file failed");
                exit(1);
        }
	while ((opt = getopt_long(argc, argv, "fvi", long_options, &option_index)) != -1) 
	{
		switch(opt) 
		{
			case 'f':
				if ((!is_f_opt) && ((fd = open(file_names[1], O_RDWR | O_CREAT | O_EXCL, S_IWRITE | S_IREAD)) < 0))
        		        {
                        		if (remove(file_names[1]) != 0)
					{
						perror("delete destination file failed");
						exit(1);
					}
				}
				is_f_opt = 1;
				break;
			case 'i':
				if ((!is_i_opt) && ((fd = open(file_names[1], O_RDWR | O_CREAT | O_EXCL, S_IWRITE | S_IREAD)) < 0))
                                {
					printf("cp: overwrite '%s'? ", file_names[1]);
					scanf("%c", &ans);
					if ((ans == 'n') || (ans == 'N'))
					{
						return 0;
					}
				}
				is_i_opt = 1;
				break;
			case 'v':
				is_v_opt = 1;
				break;
			default:
                                break;
		}
	}
        if (((!is_f_opt) && (fd = open(file_names[1], O_RDWR, S_IWRITE | S_IREAD)) < 0))
        {
                perror("open destination file failed");
                exit(1);
        }
        copy(fs, fd);
	if (is_v_opt)
	{
		printf("'%s' -> '%s'", file_names[0], file_names[1]);
	}
	close(fs);
        close(fd);
        return 0;
}

