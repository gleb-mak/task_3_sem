#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include "mystring.h"
#include "map.h"
#include "iostream.h"

int main(int argc, char** argv)
{

    struct Io* io = NewIo(argv[1]);
    struct Map* word_counter = NewMap();
    struct String* str;

    while ((str = io->getword(io)) != NULL)
    {
        word_counter->set(word_counter, str, word_counter->get(word_counter, str) + 1);
        DeleteString(str);
    }
    word_counter->print(word_counter);
    DeleteMap(word_counter);
    DeleteIo(io);
    return 0;
}
