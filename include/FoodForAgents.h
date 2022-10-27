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

#ifndef FOODFORAGENTS_H
#define FOODFORAGENTS_H

#include <fstream>

#include "pstdint.h"
#include "Color.h"
#include "Food.h"

class FoodForAgents
{
    public:
        static uint32_t maxSize;
        static uint32_t maxFoodPerAgent;
        static uint32_t foodAmount;
        static Color* foodColor;
        static Color* closestFoodColor;
        std::ifstream loadingFoodDataFileBinary;
        std::ofstream savingFoodDataFileBinary;
        char* loadingFoodDataBuffer;
        size_t loadingFoodDataBufferOffset = 0;

        Food_ptr *foodArray;

        FoodForAgents();
        FoodForAgents(uint32_t maxSize);
        void GenerateAndSaveFoodPosForAgents();
        void Add(Food *food, int32_t agentIndex = -1, Vector* pos = NULL);
        void SetFoodColor(Color color);
        double GetDistanceToFood(Vector pos, uint32_t closestFoodIndex);
        int32_t GetClosestFood(Vector pos, int32_t agentIndex = -1);
        int32_t WithinRadius(Vector pos, double radius, int32_t agentIndex = -1);
        void Draw();
        void FoodEaten(uint32_t foodIndex);
        void RemoveFood(uint32_t foodIndex);
        virtual ~FoodForAgents();
};

#endif // FOODFORAGENTS_H
