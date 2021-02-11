#pragma once

struct String
{
    void* opaque;

    void (*print) (struct String* this);
    int (*compare) (struct String* this, struct String* other);
    struct String* (*copy) (struct String* src);
};

struct String* NewString(char* string);
void DeleteString(struct String* this);

