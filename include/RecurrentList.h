#ifndef RECURRENTLIST_H
#define RECURRENTLIST_H

#include "pstdint.h"

typedef void* void_ptr;

class RecurrentList
{
    protected:

    private:
        uint32_t size;
        void_ptr* itemsList;
        uint32_t start;
        uint32_t end;
        uint32_t currentIndex;

    public:
        RecurrentList(uint32_t size);
        void Add(void* item);
        void Begin();
        void* GetCurrent();
        void Next();
        void Reset();

        virtual ~RecurrentList();
};

#endif // RECURRENTLIST_H
