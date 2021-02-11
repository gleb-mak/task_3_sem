#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include "mystring.h"

struct StringPrivate
{
    int length;
    char* data;
};

void StringPrint(struct String* this)
{
    struct StringPrivate* private = (struct StringPrivate*)this->opaque;

    printf("%s", private->data);
}

int StringCompare(struct String* this, struct String* other)
{
    struct StringPrivate* private_this = (struct StringPrivate*)this->opaque;
    struct StringPrivate* private_other = (struct StringPrivate*)other->opaque;

    return strcmp(private_this->data, private_other->data);
}

struct String* NewStringCopy(struct String* src)
{
    struct String* this = (struct String*)malloc(sizeof(struct String));
    struct StringPrivate* private_src = (struct StringPrivate*)src->opaque;
    struct StringPrivate* private = (struct StringPrivate*)malloc(sizeof(struct StringPrivate));

    private->length = private_src->length;
    private->data = (char*)malloc(private->length);
    strcpy(private->data, (char*)private_src->data);
    this->opaque = private;

    this->print = StringPrint;
    this->compare = StringCompare;
    this->copy = NewStringCopy;

    return this;
}

struct String* NewString(char* string)
{

    struct String* res = (struct String*)malloc(sizeof(struct String));

    //initialize private data
    struct StringPrivate* private = res->opaque;
    private = (struct StringPrivate*)malloc(sizeof(struct StringPrivate));
    private->length = strlen(string);
    private->data = (char*)malloc(private->length);
    strcpy(private->data, (char*)string);
    res->opaque = private;

    //initialize methods
    res->print = StringPrint;
    res->compare = StringCompare;
    res->copy = NewStringCopy;

    return res;
}

void DeleteString(struct String* this)
{
    struct StringPrivate* private = this->opaque;
    free(private->data);
    free(private);
    free(this);
}
