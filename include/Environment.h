#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "pstdint.h"
#include "FoodForAgents.h"

class Environment
{
    private:

    public:
        static uint32_t rangeX;
        static uint32_t rangeY;
        static FoodForAgents* foodForAgents_ptr;
        static uint32_t ROWS;
        static uint32_t COLUMNS;
        static uint32_t GRID_WIDTH;
        static uint32_t GRID_HEIGHT;

        Environment();
        virtual ~Environment();
};

#endif // ENVIRONMENT_H
