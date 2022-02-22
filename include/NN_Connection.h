#ifndef NN_Connection_H
#define NN_Connection_H TRUE

#include <stdio.h>
#include "pstdint.h"

class NN_Connection
{
    public:
        uint32_t column;
        uint32_t row;
        double synapseConductance;
        uint32_t neuralConnectionActivityTime;

        NN_Connection(uint32_t column, uint32_t row, double synapseConductance);

        void Load(FILE* file);
        void Save(FILE* file);
};

typedef NN_Connection* NN_Connection_Ptr;

#endif //NN_Connection_H
