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
    int count = 0;
    mydir = opendir("/proc");
    while ((myfile = readdir(mydir)) != NULL)
    {
        stat(myfile->d_name, &mystat);
        if ((myfile->d_name[0] >= '0') && (myfile->d_name[0] <= '9') && (mystat.st_mode == 16877))
        {
            printf(" %s\n", myfile->d_name);
            count++;
        }
    }
    printf("%d\n", count);
    closedir(mydir);
}



