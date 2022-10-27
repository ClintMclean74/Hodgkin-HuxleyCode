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

#include <stdio.h>
#include <stdlib.h>

#include "Environment.h"
#include "FoodForAgents.h"
#include "MathUtilities.h"
#include "Agents.h"
#include "Simulation.h"

FoodForAgents::FoodForAgents()
{
    if (Simulation::loadingData)
    {
        char textBuffer[255];
        sprintf(textBuffer, "agent_food/agentsFood1738528546.txt");
        loadingFoodDataFileBinary.open(textBuffer, std::ifstream::binary);

        // get length of file
        loadingFoodDataFileBinary.seekg(0, loadingFoodDataFileBinary.end);
        size_t length = static_cast<size_t>(loadingFoodDataFileBinary.tellg());

        loadingFoodDataFileBinary.seekg(0, loadingFoodDataFileBinary.beg);

        // read whole contents of the file to a buffer at once
        loadingFoodDataBuffer = new char[length];
        loadingFoodDataFileBinary.read(loadingFoodDataBuffer, length);
        loadingFoodDataFileBinary.close();
    }

    if (Simulation::saveAgentAllMembraneVoltagesTrace)
    {
        char textBuffer[255];
        sprintf(textBuffer, "agent_food/agentsFood%lu.txt", Simulation::simStartRealTime);

        savingFoodDataFileBinary.open(textBuffer, std::ofstream::binary);
    }

    foodArray = new Food_ptr[maxSize];

    for(uint32_t i=0; i<maxSize;i++)
    {
        foodArray[i] = NULL;
    }
}

FoodForAgents::FoodForAgents(uint32_t maxSize)
{
    this->maxSize = maxSize;

    FoodForAgents();
}

void FoodForAgents::GenerateAndSaveFoodPosForAgents()
{
    Vector foodPos;
    double gridStartX;
    double gridStartY;

    if (Simulation::loadingData)
    {
        FoodForAgents::maxFoodPerAgent = *reinterpret_cast<uint32_t*>(loadingFoodDataBuffer + loadingFoodDataBufferOffset);
        loadingFoodDataBufferOffset += sizeof(uint32_t);
    }

    if (Simulation::saveAgentAllMembraneVoltagesTrace)
    {
        savingFoodDataFileBinary.write(reinterpret_cast<const char*> (&FoodForAgents::maxFoodPerAgent), sizeof(uint32_t));
        savingFoodDataFileBinary.flush();
    }

    for (uint32_t i=0; i<FoodForAgents::maxFoodPerAgent; i++)
    {
        if (Simulation::loadingData)
        {
            foodPos.x = *reinterpret_cast<double*>(loadingFoodDataBuffer + loadingFoodDataBufferOffset);
            loadingFoodDataBufferOffset += sizeof(double);

            foodPos.y = *reinterpret_cast<double*>(loadingFoodDataBuffer + loadingFoodDataBufferOffset);
            loadingFoodDataBufferOffset += sizeof(double);

            foodPos.z = *reinterpret_cast<double*>(loadingFoodDataBuffer + loadingFoodDataBufferOffset);
            loadingFoodDataBufferOffset += sizeof(double);
        }
        else
        {
            do
            {
                foodPos.Set((double) rand() / RAND_MAX * Environment::GRID_WIDTH, (double) rand() / RAND_MAX * Environment::GRID_HEIGHT, 0);
            }while(foodPos.Length()<Food::radius + Agent::radius);
        }

        if (Simulation::saveAgentAllMembraneVoltagesTrace)
        {
            savingFoodDataFileBinary.write(reinterpret_cast<const char*> (&foodPos.x), sizeof(double));
            savingFoodDataFileBinary.write(reinterpret_cast<const char*> (&foodPos.y), sizeof(double));
            savingFoodDataFileBinary.write(reinterpret_cast<const char*> (&foodPos.z), sizeof(double));

            savingFoodDataFileBinary.flush();
        }

        for (uint32_t j=0; j<Agents::count; j++)
        {
            uint32_t foodIndex = i * Agents::count + j;

            if (foodArray[foodIndex] == NULL)
                foodArray[foodIndex] = new Food();
            else
                 if (foodArray[foodIndex]->eaten)
                    foodArray[foodIndex]->eaten = false;

            foodArray[foodIndex]->agentIndex = j;

            gridStartX = (uint32_t)(((double) j)/Environment::ROWS) * Environment::GRID_WIDTH;
            gridStartY = (j % Environment::ROWS) * Environment::GRID_HEIGHT;

            foodArray[foodIndex]->pos.Set(gridStartX + foodPos.x, gridStartY + foodPos.y, 0);;
        }
    }

    for (uint32_t i=FoodForAgents::maxFoodPerAgent*Agents::count; i<FoodForAgents::maxSize; i++)
    {
        RemoveFood(i);
    }
}

void FoodForAgents::SetFoodColor(Color color)
{
    for(uint32_t i=0; i<maxSize;i++)
    {
        if (foodArray[i] != NULL)
            foodArray[i]->color.Set(color);
    }
}

void FoodForAgents::Add(Food *food, int32_t agentIndex, Vector* pos)
{
    if (food == NULL)
        foodArray[foodAmount] = new Food();
    else
        foodArray[foodAmount] = food;

    if (agentIndex != -1)
    {
        foodArray[foodAmount]->agentIndex = agentIndex;

        double gridStartX = (uint32_t)(((double) agentIndex)/Environment::ROWS) * Environment::GRID_WIDTH;
        double gridStartY = (agentIndex % Environment::ROWS) * Environment::GRID_HEIGHT;

        if (!pos)
            foodArray[foodAmount++]->pos.Set(gridStartX + (double) rand() / RAND_MAX * Environment::GRID_WIDTH, gridStartY + (double) rand() / RAND_MAX * Environment::GRID_HEIGHT, 0);
        else
            foodArray[foodAmount++]->pos.Set(gridStartX + pos->x, gridStartY + pos->y, 0);
    }
    else
        foodArray[foodAmount++]->pos.Set((double) rand() / RAND_MAX * Environment::rangeX, (double) rand() / RAND_MAX * Environment::rangeX, 0);
}

double FoodForAgents::GetDistanceToFood(Vector pos, uint32_t closestFoodIndex)
{
    return MathUtilities::Distance(pos, foodArray[closestFoodIndex]->pos);
}

int32_t FoodForAgents::GetClosestFood(Vector pos, int32_t agentIndex)
{
    int32_t index = -1;

    double minDistance = -1, distance;
    for(uint32_t i=0; i<maxSize;i++)
    {
        if (foodArray[i] != NULL && foodArray[i]->eaten == false)
            if (foodArray[i]->agentIndex == agentIndex || agentIndex == -1)
            {
                distance = GetDistanceToFood(pos, i);

                if (distance < minDistance || minDistance == -1)
                {
                    minDistance = distance;

                    index = i;
                }
            }
    }

    return index;
}

int32_t FoodForAgents::WithinRadius(Vector pos, double radius, int32_t agentIndex)
{
    for(int32_t i=0; i<maxSize;i++)
    {
        if (foodArray[i] != NULL && foodArray[i]->eaten == false)
            if (foodArray[i]->agentIndex == agentIndex || agentIndex == -1)
                if (foodArray[i]->WithinRadius(pos, radius))
                    return i;
    }

    return -1;
}

void FoodForAgents::Draw()
{
    for(uint32_t i=0; i<maxSize;i++)
    {
        if (foodArray[i] != NULL)
            foodArray[i]->Draw();
    }
}

void FoodForAgents::FoodEaten(uint32_t foodIndex)
{
    foodArray[foodIndex]->eaten = true;

    foodAmount--;
}

void FoodForAgents::RemoveFood(uint32_t foodIndex)
{
    delete foodArray[foodIndex];

    foodArray[foodIndex] = NULL;

    foodAmount--;
}

FoodForAgents::~FoodForAgents()
{
    for(uint32_t i=0; i<maxSize;i++)
    {
        if (foodArray[i] != NULL)
            delete foodArray[i];
    }

    delete [] foodArray;
}

uint32_t FoodForAgents::maxFoodPerAgent = 10;
uint32_t FoodForAgents::foodAmount = 0;
uint32_t FoodForAgents::maxSize = 1000;
Color* FoodForAgents::foodColor = new Color(0, 1, 0);
Color* FoodForAgents::closestFoodColor = new Color(1.0, 1.0, 0.0);
