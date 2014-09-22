#ifndef _List_H_
#define _List_H_

#include <stdlib.h>
#include <string.h>

typedef struct 
{
    struct tPrivate *private;
}tList;

extern tList *newList(void);
extern void   deleteList(tList *this);

extern void   List_append(tList *this, void *data);
extern void  *List_appendAlloc(tList *this, void *data, int size);
extern void   List_insert(tList *this, int index, void *data);
extern void  *List_insertAlloc(tList *this, int index, void *data, int size);
extern void   List_insertBack(tList *this, int index, void *data);
extern void  *List_insertBackAlloc(tList *this, int index, void *data, int size);
extern void   List_remove(tList *this, int index);
extern void   List_removeBack(tList *this, int index);
extern void   List_unlink(tList *this, int index);
extern void   List_unlinkBack(tList *this, int index);
extern void   List_clear(tList *this);
extern void  *List_at(tList *this, int index);
extern void  *List_atBack(tList *this, int index);
extern int    List_count(tList *this);

extern void  *List_begin(tList *this);
extern void  *List_end(tList *this);
extern void  *List_iteratorNext(tList *this);
extern void  *List_iteratorPrev(tList *this);

#endif