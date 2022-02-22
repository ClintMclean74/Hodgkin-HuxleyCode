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
        ////sprintf(textBuffer, "agent_food/agentsFood%i.txt", Simulation::simStartRealTime);
        ////sprintf(textBuffer, "agents_3nn_30layers_9_30_37_37_00002_37_00004/agent_food/agentsFood641009531.txt");
        ////sprintf(textBuffer, "agents_2nn_9_30_37_37_000001/agent_food/agentsFood816196296.txt");
        sprintf(textBuffer, "agent_food/agentsFood910055203.txt", Simulation::simStartRealTime);
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

    if (Simulation::savingData)
    {
        char textBuffer[255];
        sprintf(textBuffer, "agent_food/agentsFood%i.txt", Simulation::simStartRealTime);
        ////sprintf(textBuffer, "agent_food/agentsFood641009531.txt");

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

void FoodForAgents::ReGenerate()
{
    /*////for(uint32_t i=0; i<maxSize;i++)
    {
        if (foodArray[i] == NULL)
            foodArray[i] = new Food();

        foodArray[i]->pos.Set((double) rand() / RAND_MAX * Environment::rangeX, (double) rand() / RAND_MAX * Environment::rangeY, 0);
    }*/


    Vector foodPos;
    double gridStartX;
    double gridStartY;

    if (Simulation::loadingData)
    {
        FoodForAgents::maxFoodPerAgent = *reinterpret_cast<uint32_t*>(loadingFoodDataBuffer + loadingFoodDataBufferOffset);
        loadingFoodDataBufferOffset += sizeof(uint32_t);
    }

    if (Simulation::savingData)
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

        if (Simulation::savingData)
        {
            savingFoodDataFileBinary.write(reinterpret_cast<const char*> (&foodPos.x), sizeof(double));
            savingFoodDataFileBinary.write(reinterpret_cast<const char*> (&foodPos.y), sizeof(double));
            savingFoodDataFileBinary.write(reinterpret_cast<const char*> (&foodPos.z), sizeof(double));

            savingFoodDataFileBinary.flush();
        }


        for (uint32_t j=0; j<Agents::count; j++)
        {
            uint32_t foodIndex = i * Agents::count + j;

            /*////if (foodArray[foodIndex] == NULL)
                foodArray[foodIndex] = new Food();
            else*/
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
            //distance = (pos - foodArray[i]->pos).Length();
            ////distance = MathUtilities::Distance(pos, foodArray[i]->pos);
            distance = GetDistanceToFood(pos, i);

            if (distance < minDistance || minDistance == -1)
            {
                minDistance = distance;

                index = i;
            }
        }
    }

    if (index != -1 && foodArray[index]->agentIndex != agentIndex)
        int grc = 1;

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

////uint32_t FoodForAgents::maxFoodPerAgent = 300;
uint32_t FoodForAgents::maxFoodPerAgent = 10;
uint32_t FoodForAgents::foodAmount = 0;
uint32_t FoodForAgents::maxSize = 1000;
double FoodForAgents::reinforcementValue = 10;
Color* FoodForAgents::foodColor = new Color(0, 1, 0);
Color* FoodForAgents::closestFoodColor = new Color(1.0, 1.0, 0.0);
