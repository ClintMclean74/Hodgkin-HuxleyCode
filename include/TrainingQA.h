#ifndef TrainingQA_H
#define TrainingQA_H TRUE

#include "pstdint.h"

class TrainingQA
{
    //uint32_t size;
    uint32_t layer1Size;
    uint32_t resultLayerSize;

    public:
        char* q;
        char* a;

        TrainingQA(uint32_t layer1Size, uint32_t resultLayerSize);
        void Initialize();
        void Set(const char q[], const char a[]);
        double CheckQ(uint32_t* resultLayerSignalsBuffer);
        double CheckA(uint32_t* qLayerSignalsBuffer, uint32_t resultLength, bool print);
        ~TrainingQA();
};

typedef TrainingQA* TrainingQA_ptr;

#endif // TrainingQA_H
