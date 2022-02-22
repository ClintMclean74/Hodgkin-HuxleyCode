#ifndef FOOD_H
#define FOOD_H

#include "pstdint.h"
#include "Color.h"
#include "Vector.h"

class Food
{
    public:
        static double radius;
        Vector pos;
        Color color;

        int32_t agentIndex;

        static Color FoodDefaultColor;

        bool eaten = false;

        Food();
        bool WithinRadius(Vector pos, double radius);
        void Draw();
        virtual ~Food();
};

typedef Food* Food_ptr;

#endif // FOOD_H
