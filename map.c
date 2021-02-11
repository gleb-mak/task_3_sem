#include "map.h"
#include "mystring.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

struct Item
{
    struct String* key;
    int value;
};

struct MapPrivate
{
    struct Item* data;
    int length;
};

int MapGet(struct Map* this, struct String* key)
{
    struct MapPrivate* private = (struct MapPrivate*)this->opaque;
    for (int i = 0; i < private->length; ++i)
    {
        if (key->compare(key, private->data[i].key) == 0)
        {
            return private->data[i].value;
        }
    }
    return 0; //if this key doesn't exist
}

void MapSet(struct Map* this, struct String* key, int val)
{
    struct MapPrivate* private = (struct MapPrivate*)this->opaque;
    for (int i = 0; i < private->length; ++i)
    {
        if (key->compare(key, private->data[i].key) == 0)
        {
            private->data[i].value = val;
            return;
        }
    }
    //if this key doesn't exist yet
    private->length++;
    private->data = realloc(private->data, sizeof(struct Item) *  private->length);
    private->data[private->length - 1].key = key->copy(key);
    private->data[private->length - 1].value = val;
    this->opaque = private;
}

void MapPrint(struct Map* this)
{
    struct MapPrivate* private = (struct MapPrivate*)this->opaque;

    for (int i = 0; i < private->length; ++i)
    {
        private->data[i].key->print(private->data[i].key);
        printf(": %d\n", private->data[i].value);
    }
}

struct Map* NewMap()
{
    struct Map* res = (struct Map*)malloc(sizeof(struct Map));
    struct MapPrivate* private = (struct MapPrivate*)malloc(sizeof(struct MapPrivate));
    private->length = 0;
    private->data = malloc(0);
    res->opaque = private;
    res->get = MapGet;
    res->set = MapSet;
    res->print = MapPrint;
    return res;
}

void DeleteMap(struct Map* this)
{
    struct MapPrivate* private = this->opaque;
    for (int i = 0; i < private->length; ++i)
    {
        DeleteString(private->data[i].key);
    }
    free(private->data);
    free(private);
    free(this);
}
