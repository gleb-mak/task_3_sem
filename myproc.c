#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

int count1;
int count2;
int count3;

int dir_count(DIR * cur_dir)
{
    struct dirent* cur_file;
    struct stat cur_stat;
    char** subdirs;
    subdirs = (char**)malloc(10000 * sizeof(char*));
    while ((cur_file = readdir(cur_dir)) != NULL)
    {
        count3++;
        int i = 0;
        char name[100] = { 0 };
        strcat(name, "/proc/");
        strcat(name, cur_file->d_name);
        stat(name, &cur_stat);
        if (S_ISDIR(cur_stat.st_mode))
        {
            subdirs[i] = (char*)malloc((strlen(name) + 1) * sizeof(char));
            strcpy(subdirs[i], name);
            i++;
            count1++;
        }
        if ((cur_file->d_name[0] >= '0') && (cur_file->d_name[0] <= '9'))
        {
            count2++;
        }
    }
    closedir(cur_dir);
    while (*subdirs != NULL)
    {
        if ((*subdirs[0] != '.'))
        {
            dir_count(opendir(*subdirs));
        }
        subdirs++;
    }
    free(subdirs);
}

int main(int argc, char* argv[])
{
    DIR *mydir;
    struct dirent *myfile;
    struct stat mystat;
    int isdir = 0;
    mydir = opendir("/proc");
    dir_count(mydir);
    printf("isdir: %d\t started with numbrer: %d\t all: %d\n", count1, count2, count3);
    closedir(mydir);
}
