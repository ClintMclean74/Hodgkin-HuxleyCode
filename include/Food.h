/*
 * Code System for the book "Solving Havana Syndrome and Biological Effects of RF
 * Using the Hodgkin-Huxley Neuron Model"
 * Copyright (C) 2022 by Clint Mclean <clint@mcleanresearchinstitute.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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
