#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>

int main(int argc, char* argv[])
{
    DIR *mydir;
    struct dirent *myfile;
    struct stat mystat;
    int count1 = 0;
    int count2 = 0;
    int count3 = 0;
    int isdir = 0;
    mydir = opendir("/proc");
    while ((myfile = readdir(mydir)) != NULL)
    {
        char name[100] = { 0 };
        strcat(name, "/proc/");
        strcat(name, myfile->d_name);
        stat(name, &mystat);
        isdir = S_ISDIR(mystat.st_mode);
        if (isdir)
        {
            count1++;
        }
        if ((myfile->d_name[0] >= '0') && (myfile->d_name[0] <= '9'))
        {
            //printf(" %s\n", myfile->d_name);
            count2++;
        }
        count3++;
        isdir = 0;
    }
    printf("isdir: %d\t started with numbrer: %d\t all: %d\n", count1, count2, count3);
    closedir(mydir);
}

