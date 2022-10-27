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
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

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
