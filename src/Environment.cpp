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
