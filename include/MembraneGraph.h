#ifndef GRAPH_H
#define GRAPH_H

#include "pstdint.h"

class MembraneVoltageGraph
{
    public:
        static const uint32_t MAX_GRAPH_INDEX = 700;
        static double WIDTH;
        static double HEIGHT;
        static double X_GAP;
        static double Y_GAP;
        static bool SCROLLING;
};

typedef MembraneVoltageGraph* MembraneGraphPtr;

#endif
