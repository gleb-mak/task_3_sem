#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>
#include <getopt.h>
#include <string.h>

int isLetter(char x)
{
	    return ((x != '\t') && (x != '\0') && (x != ' ') && (x != '\n'));
}

int main(int argc, char *argv[])
{
		int is_end = 1;
		int is_w = 0;
		int j = 0;
		int option_index = 0;
		int opt = 0;
		int num_s = 0;
		int num_w = 0;
		int num_b = 0;
		int size_r = 1;
		struct timespec t1;
		struct timespec t2;
		long long int dt = 0;
		int stat = 0;
		char *argv_new[argc];
		int pipefd[2];
		char buf[100];
		pid_t pid;
	  	static struct option long_options[] = {
                {"wc", no_argument, 0, 'w'}
        };
        while ((opt = getopt_long(argc, argv, "w", long_options, &option_index)) != -1)
        {
                switch(opt)
                {
                        case 'w':
								is_w = 1;
                                break;
                        default:
                                break;
                }
        }	
        for (int i = 1; i < argc; i++)
        {
			if ((strcmp(argv[i], "-w") != 0) && (strcmp(argv[i], "--wc") != 0) && (strcmp(argv[i], "--") != 0))
			{
				argv_new[i - 1 - j] = argv[i];
				continue;
			}
			j++;
        }
		argv_new[argc - 1 - j] = NULL;
        clock_gettime(CLOCK_REALTIME, &t1);
		if (!is_w)
		{
        	if ((pid = fork()) == -1)
			{
				perror("fork failed");
				exit(1);
			}
        	if (pid == 0)
        	{
                execvp(argv_new[0], argv_new);
                perror("exec feiled");
				exit(1);
        	}
		}
		if (is_w)
		{
			if (pipe(pipefd) == -1)
			{
				perror("pipe failed");
				exit(1);
			}
			switch (pid = fork())
			{
				case -1: //error
					perror("fork failed");
					exit(1);
				case 0: //child
					close(1);
					dup(pipefd[1]);
					close(pipefd[1]);
					close(pipefd[0]);
					execvp(argv_new[0], argv_new);
					perror("exec");
					break;
				default: //parent
	                close(pipefd[1]);
        	        while ((size_r = read(pipefd[0], buf, 100)) != 0)
                	{
						if (size_r == -1)
						{
                            perror("read failed");
                            exit(1);
						}
						num_b += size_r;
						if (isLetter(buf[0]) && is_end)
						{
							num_w++;
						}
						for (int i = 0; i < size_r - 1; i++)
						{
							if (!isLetter(buf[i]) && isLetter(buf[i + 1]))
							{
								num_w++;
							}
							if (buf[i] == '\n')
							{
								num_s++;
							}
						}
						is_end = !isLetter(buf[size_r - 1]);
                    }
                    printf("\t%d\t%d\t%d\n", num_s + 1, num_w, num_b);
                    close(pipefd[0]);
					break;
			}
		}
		wait(&stat);
        clock_gettime(CLOCK_REALTIME, &t2);
        dt = (t2.tv_sec - t1.tv_sec) * 1000000000 + (t2.tv_nsec - t1.tv_nsec);
        printf("%lld sec  %lld microsec\n", dt / 1000000000, (dt % 1000000000) / 1000);
        return 0;
}

