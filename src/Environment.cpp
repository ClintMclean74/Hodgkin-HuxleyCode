#include "Environment.h"

Environment::Environment()
{
}

Environment::~Environment()
{
}

uint32_t Environment::rangeX = 10000;
uint32_t Environment::rangeY = 10000;
uint32_t Environment::ROWS = 3;
uint32_t Environment::COLUMNS = 3;
uint32_t Environment::GRID_WIDTH = Environment::rangeX / Environment::COLUMNS;
uint32_t Environment::GRID_HEIGHT = Environment::rangeY / Environment::ROWS;

FoodForAgents* Environment::foodForAgents_ptr = NULL;
