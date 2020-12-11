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

int dir_count(char* path_name)
{
    DIR *cur_dir;
    struct dirent* cur_file;
    struct stat cur_stat;
    if ((cur_dir = opendir(path_name)) == NULL)
    {
        return 0;
    }
    while ((cur_file = readdir(cur_dir)) != NULL)
    {
        count3++;
        int i = 0;
        char* name = (char*)malloc(strlen(path_name) + strlen(cur_file->d_name) + 2);
        *name = 0;
        strcat(name, path_name);
        strcat(name, "/");
        strcat(name, cur_file->d_name);
        lstat(name, &cur_stat);
        if ((cur_file->d_name[0] >= '0') && (cur_file->d_name[0] <= '9') && (strcmp(path_name, "/proc") == 0))
        {
            count2++;
        }
        if (S_ISDIR(cur_stat.st_mode))
        {
            if ((strcmp(cur_file->d_name, ".") != 0) && (strcmp(cur_file->d_name, "..")))
            {
                count1++;
                dir_count(name);
            }
        }
        free(name);
    }
    closedir(cur_dir);
    return 0;
}

int main(int argc, char* argv[])
{
    DIR *mydir;
    struct dirent *myfile;
    struct stat mystat;
    int isdir = 0;
    dir_count("/proc");
    printf("isdir: %d\t started with numbrer: %d\t all: %d\n", count1, count2, count3);
    return 0;
}
