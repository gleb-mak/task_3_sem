#include <stdio.h>
#include <string.h>

int main(int argc, char* argv[])
{
    char* str = "-n";
    int newLine = 1;
    int i = 0;
    if (argc == 1)
    {
        printf("\n");
        return 0;
    }
    if (strcmp(argv[1], str) == 0)
    {
        newLine++;
    }
    for (int i = newLine; i < argc; i++)
    {
        printf("%s", argv[i]);
        if (i != argc - 1)
        {
            printf(" ");
        }
    }
    if (newLine == 1)
    {
        printf("\n");
    }
    return 0;
}
