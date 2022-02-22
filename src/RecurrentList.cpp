#include "RecurrentList.h"

RecurrentList::RecurrentList(uint32_t size)
{
    this->size = size;
    itemsList = new void_ptr[size];

    for(uint32_t i=0; i<size;i++)
        itemsList[i] = NULL;

    start = end = 0;
}

void RecurrentList::Add(void* item)
{
    if (itemsList[end])
        delete itemsList[end];

    itemsList[end] = item;

    end++;

    if (end >= size)
        end = 0;

    if (end == start)
    {
        start++;

        if (start >= size)
        start = 0;
    }
}

void RecurrentList::Begin()
{
    currentIndex = start;
}

void* RecurrentList::GetCurrent()
{
    if (currentIndex != end)
        return itemsList[currentIndex];
    else
        return NULL;
}

void RecurrentList::Next()
{
    currentIndex++;

    if (currentIndex>=size)
        currentIndex=0;
}

void RecurrentList::Reset()
{
    start = end = currentIndex = 0;
}

RecurrentList::~RecurrentList()
{

}
