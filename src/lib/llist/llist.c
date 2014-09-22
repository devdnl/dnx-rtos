#include "llist.h"

#include <stdio.h>

typedef struct
{
    void *prev;
    void *next;
    void *data;
}tListItem;

struct tPrivate
{
    int count;
    int count2;
    tListItem *first;
    tListItem *last;
    tListItem *iterator;
};

static void insertItem(tList *this, int index, void *data, int direction);
static void *atItem(tList *this, int index, int direction);
static void removeItem(tList *this, int index, int direction, int unlink);
static void appendPrepend(tList *this, void *data, int direction);

/*! \brief List constructor.
 *  \return tList - new list pointer
*/
tList* newList(void)
{
    /* allocate memory for private data */
    struct tPrivate *priv = calloc(1, sizeof(struct tPrivate));
    if( priv == NULL )
        return NULL;

    /* allocate memory for public data */
    tList* this = calloc(1, sizeof(tList));
    if( this == NULL )
        return NULL;

    this->private = priv;

    /* set default values */
    this->private->count = 0;
    this->private->count2 = 0;
    this->private->first = NULL;
    this->private->last = NULL;

    return this;
}

/*! \brief List destructor.
 *  \param [in] this - list pointer
*/
void deleteList(tList *this)
{
    List_clear(this);

    /* free class memory */
    free(this->private);
    free(this);
}

/*! \brief Append data to list.
 *  \param [in] this - list pointer
 *  \param [in] data - data pointer
*/
void List_append(tList *this, void *data)
{
    appendPrepend(this, data, 1);
}

/*! \brief Append data to list and alloc memory.
 *  \param [in] this - list pointer
 *  \param [in] data - data pointer
 *  \param [in] size - size of data
 *  \return void* - address to memory block
*/
void *List_appendAlloc(tList *this, void *data, int size)
{
    void* mem = malloc(size);
    if (mem == NULL)
        return NULL;

    memcpy(mem, data, size);
    appendPrepend(this, mem, 1);
    return mem;
}

/*! \brief Clear list.
 *  \param [in] this - list pointer
*/
void List_clear(tList *this)
{
    if( this->private->count == 0 )
        return;

    tListItem *item = this->private->first;
    tListItem *cItem;

    while( item != NULL )
    {
        cItem = item;
        item = item->next;

        if( cItem->data != NULL )
            free(cItem->data);
        free(cItem);
    }

    this->private->count = 0;
    this->private->count2 = 0;
    this->private->first = NULL;
    this->private->last = NULL;
}

/*! \brief Remove list element.
 *  \param [in] this - list pointer
 *  \param [in] index - element index to remove
*/
void List_remove(tList *this, int index)
{
    if( index <= (this->private->count2) )
        return removeItem(this, index, 1, 0);
    else
        return removeItem(this, this->private->count - index - 1, -1, 0);
}

/*! \brief Remove list element from back.
 *  \param [in] this - list pointer
 *  \param [in] index - element index to remove
*/
void List_removeBack(tList *this, int index)
{
    if( index <= (this->private->count2) )
        return removeItem(this, index, -1, 0);
    else
        return removeItem(this, this->private->count - index - 1, 1, 0);
}

/*! \brief Remove list item but not data.
 *  \param [in] this - list pointer
 *  \param [in] index - element index to remove
*/
void List_unlink(tList *this, int index)
{
    if( index <= (this->private->count2) )
        return removeItem(this, index, 1, 1);
    else
        return removeItem(this, this->private->count - index - 1, -1, 1);
}

/*! \brief Remove list item but not data from back.
 *  \param [in] this - list pointer
 *  \param [in] index - element index to remove
*/
void List_unlinkBack(tList *this, int index)
{
    if( index <= (this->private->count2) )
        return removeItem(this, index, -1, 1);
    else
        return removeItem(this, this->private->count - index - 1, 1, 1);
}

/*! \brief Insert element to list.
 *  \param [in] this - list pointer
 *  \param [in] index - element index to remove
 *  \param [in] data - pointer to data
*/
void List_insert(tList *this, int index, void *data)
{
    if( index <= (this->private->count2) )
        insertItem(this, index, data, 1);
    else
        insertItem(this, this->private->count - index, data, -1);
}

/*! \brief Insert element to list and allocate memory.
 *  \param [in] this - list pointer
 *  \param [in] index - element index to remove
 *  \param [in] data - pointer to data
 *  \param [in] size - data size
 *  \return void* - pointer do allocated memory
*/
void *List_insertAlloc(tList *this, int index, void *data, int size)
{
    void* mem = malloc(size);
    if (mem == NULL)
        return NULL;

    memcpy(mem, data, size);
    List_insert(this, index, mem);
    return mem;
}

/*! \brief Insert element to list from back.
 *  \param [in] this - list pointer
 *  \param [in] index - element index to remove
 *  \param [in] data - pointer to data
*/
void List_insertBack(tList *this, int index, void *data)
{
    if( index <= (this->private->count2) )
        insertItem(this, index, data, -1);
    else
        insertItem(this, this->private->count - index, data, 1);
}

/*! \brief Insert element to list from back and allocate memory.
 *  \param [in] this - list pointer
 *  \param [in] index - element index to remove
 *  \param [in] data - pointer to data
 *  \param [in] size - data size
 *  \return void* - pointer do allocated memory
*/
void *List_insertBackAlloc(tList *this, int index, void *data, int size)
{
    void* mem = malloc(size);
    if (mem == NULL)
        return NULL;

    memcpy(mem, data, size);
    List_insertBack(this, index, mem);
    return mem;
}

/*! \brief Get element from list.
 *  \param [in] this - list pointer
 *  \param [in] index - element index to get
 *  \return void* - pointer to data
*/
void *List_at(tList *this, int index)
{
    if( index <= (this->private->count2) )
        return atItem(this, index, 1);
    else
        return atItem(this, this->private->count - index - 1, -1);
}

/*! \brief Get element from list from back.
 *  \param [in] this - list pointer
 *  \param [in] index - element index to get
 *  \return void* - pointer to data
*/
void *List_atBack(tList *this, int index)
{
    if( index <= (this->private->count2) )
        return atItem(this, index, -1);
    else
        return atItem(this, this->private->count - index - 1, 1);
}

/*! \brief Get list elements count.
 *  \param [in] this - list pointer
 *  \return int - count of list elements
*/
int List_count(tList *this)
{
    return this->private->count;
}

/*! \brief Set iterator at first item.
 *  \param [in] this - list pointer
 *  \return void* - pointer to data (first element)
*/
void *List_begin(tList *this)
{
    this->private->iterator = this->private->first;

    if( this->private->iterator == NULL )
        return NULL;

    return this->private->iterator->data;
}

/*! \brief Set iterator at last item.
 *  \param [in] this - list pointer
 *  \return void* - pointer to data (last element)
*/
void *List_end(tList *this)
{
    this->private->iterator = this->private->last;

    if( this->private->iterator == NULL )
        return NULL;

    return this->private->iterator->data;
}

/*! \brief Set iterator at last item.
 *  \param [in] this - list pointer
 *  \return void* - pointer to data
*/
void *List_iteratorNext(tList *this)
{
    this->private->iterator = this->private->iterator->next;

    if( this->private->iterator == NULL )
        return NULL;

    return this->private->iterator->data;
}

/*! \brief Set iterator at last item.
 *  \param [in] this - list pointer
 *  \return void* - pointer to data
*/
void *List_iteratorPrev(tList *this)
{
    this->private->iterator = this->private->iterator->prev;

    if( this->private->iterator == NULL )
        return NULL;

    return this->private->iterator->data;
}

/*! \brief Remove list element.
 *  \param [in] this - list pointer
 *  \param [in] index - element index to remove
 *  \param [in] unlink - unlink data from list and remove item
 *  \param [in] direction - >=0 from front; <0 from back
*/
static void removeItem(tList *this, int index, int direction, int unlink)
{
    if( this->private->count == 0 )
        return;

    if( index < 0 )
        index = 0;

    if( index >= this->private->count )
        index = this->private->count-1;

    tListItem *item;
    if( direction >= 0 )
        item = this->private->first;
    else
        item = this->private->last;

    tListItem *prevItem = NULL;
    tListItem *nextItem = NULL;

    while( item != NULL )
    {
        if( index-- == 0 )
        {
            prevItem = item->prev;
            nextItem = item->next;

            if( prevItem == NULL )
                this->private->first = nextItem;
            else
                prevItem->next = nextItem;

            if( nextItem == NULL )
                this->private->last = prevItem;
            else
                nextItem->prev = prevItem;

            if( (item->data != NULL) && (unlink == 0) )
                free(item->data);
            free(item);
            this->private->count--;
            this->private->count2 = this->private->count/2;
            break;
        }

        if( direction >= 0 )
            item = item->next;
        else
            item = item->prev;
    }
}

/*! \brief Insert element to list.
 *  \param [in] this - list pointer
 *  \param [in] index - element index to remove
 *  \param [in] data - pointer to data
 *  \param [in] direction - >=0 from front; <0 from back
*/
static void insertItem(tList *this, int index, void *data, int direction)
{
    if( index <= 0 )
    {
        if( direction >= 0 )
            appendPrepend(this, data, -1);
        else
            appendPrepend(this, data, 1);
        return;
    }else
    if( index >= this->private->count )
    {
        if( direction >= 0 )
            appendPrepend(this, data, 1);
        else
            appendPrepend(this, data, -1);
        return;
    }

    tListItem* newItem = calloc(1, sizeof(tListItem));
    if( newItem == NULL )
        return;

    newItem->data = data;

    tListItem *item;
    if( direction >= 0 )
        item = this->private->first;
    else
    {
        item = this->private->last;
        index -= 1;
    }

    if( item == NULL )
    {
        newItem->prev = NULL;
        newItem->next = NULL;
        this->private->first = newItem;
        this->private->last = newItem;
        this->private->count++;
        this->private->count2 = this->private->count/2;
        return;
    }

    while( item != NULL )
    {
        if( index-- == 0 )
        {
            tListItem *itemPrev = item->prev;

            if( itemPrev == NULL )
                this->private->first = newItem;
            else
                itemPrev->next = newItem;

            newItem->prev = itemPrev;
            newItem->next = item;
            itemPrev = newItem;

            this->private->count++;
            this->private->count2 = this->private->count/2;
            break;
        }

        if( direction >=0 )
            item = item->next;
        else
            item = item->prev;
    }
}

/*! \brief Get element from list.
 *  \param [in] this - list pointer
 *  \param [in] index - element index to get
 *  \param [in] direction - >=0 from front; <0 from back
 *  \return void* - pointer to data
*/
static void *atItem(tList *this, int index, int direction)
{
    if( index < 0 )
        return NULL;

    if( index >= this->private->count )
        return NULL;

    tListItem *item;
    if( direction >= 0 )
        item = this->private->first;
    else
        item = this->private->last;

    while( item != NULL )
    {
        if( index-- == 0 )
            return item->data;

        if( direction >= 0 )
            item = item->next;
        else
            item = item->prev;
    }

    return NULL;
}

/*! \brief Append data to list.
 *  \param [in] this - list pointer
 *  \param [in] data - data pointer
 *  \param [in] direction - >=0 append; <0 prepend
*/
static void appendPrepend(tList *this, void *data, int direction)
{
    tListItem* newItem = calloc(1, sizeof(tListItem));
    if( newItem == NULL )
        return;

    newItem->data = data;

    if(this->private->first == NULL)
    {
        newItem->prev = NULL;
        newItem->next = NULL;
        this->private->first = newItem;
        this->private->last = newItem;
    }else
    {
        if( direction >= 0 )
        {
            tListItem* prevItem = this->private->last;
            prevItem->next = newItem;
            newItem->prev = prevItem;
            newItem->next = NULL;
            this->private->last = newItem;
        }else
        {
            tListItem* nextItem = this->private->first;
            nextItem->prev = newItem;
            newItem->prev = NULL;
            newItem->next = nextItem;
            this->private->first = newItem;
        }
    }

    this->private->count++;
    this->private->count2 = this->private->count/2;
}

