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
        static double reinforcementValue;
        static Color* foodColor;
        static Color* closestFoodColor;
        std::ifstream loadingFoodDataFileBinary;
        std::ofstream savingFoodDataFileBinary;
        char* loadingFoodDataBuffer;
        size_t loadingFoodDataBufferOffset = 0;

        Food_ptr *foodArray;

        FoodForAgents();
        FoodForAgents(uint32_t maxSize);
        void ReGenerate();
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
