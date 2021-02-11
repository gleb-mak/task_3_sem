#pragma once

#include "mystring.h"

struct Map
{
    void* opaque;

    int (*get) (struct Map* this, struct String* key);
    void (*set) (struct Map* this, struct String* key, int val);
    void (*print) (struct Map* this);

};

struct Map* NewMap();
void DeleteMap(struct Map* this);
