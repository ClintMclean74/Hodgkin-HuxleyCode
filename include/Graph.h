#ifndef GRAPH_H
#define GRAPH_H

#include "pstdint.h"

class Graph
{
    public:
        static const uint32_t MAX_GRAPH_INDEX = 700;
        ////static const uint32_t MAX_GRAPH_INDEX = 1000;
        //static const uint32_t ROWS = 5;
        static double WIDTH;
        static double HEIGHT;
        static double X_GAP;
        static double Y_GAP;
        static bool SCROLLING;

};

typedef Graph* GraphPtr;

#endif
