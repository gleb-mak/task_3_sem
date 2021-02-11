#include "iostream.h"
#include "mystring.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

struct IoPrivate
{
    FILE* file;
};

struct String* GetWord(struct Io* this)
{
    struct IoPrivate* private = (struct IoPrivate*)this->opaque;

    char str[100];
    int letter;
    int i = 0;
    letter = fgetc(private->file);
    while ((letter == ' ') || (letter == '\n'))
    {
        letter = fgetc(private->file);
    }
    if (letter == EOF)
    {
        return NULL;
    }
    ungetc(letter, private->file);
    letter = fgetc(private->file);
    while ((letter != ' ') && (letter != EOF) && (letter != '\n') && (letter != '\0'))
    {
        str[i] = letter;
        ++i;
        letter = fgetc(private->file);
    }
    str[i] = '\0';
    return NewString(str);
}

struct Io* NewIo(const char* path_in)
{
    struct Io* res = (struct Io*)malloc(sizeof(struct Io));
    struct IoPrivate* private = (struct IoPrivate*)malloc(sizeof(struct IoPrivate));

    private->file = fopen(path_in, "r");
    res->opaque = private;
    res->getword = GetWord;
    return res;
}

void DeleteIo(struct Io* this)
{
    struct IoPrivate* private = (struct IoPrivate*)this->opaque;

    fclose(private->file);
    free(private);
    free(this);
}


