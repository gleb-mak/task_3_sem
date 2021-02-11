#pragma once

#include "mystring.h"

struct Io
{
    void* opaque;

    struct String* (*getword) (struct Io* this);
};

struct Io* NewIo(const char* path);
void DeleteIo(struct Io* this);
