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

#include <string.h>
#include <stdlib.h>
#include "NN_Connection.h"

NN_Connection::NN_Connection(uint32_t column, uint32_t row, long double synapseConductance)
{
    this->column = column;
    this->row = row;

    this->synapseConductance = synapseConductance;

    neuralConnectionActivityTime = 0;
}

void NN_Connection::Load(FILE* file)
{
    char textBuffer[255];
    memset(textBuffer, 0, 255);

    fgets(textBuffer, 255, file);

    char *ptr = strtok(textBuffer, ",");

    column = atoi(ptr);
    ptr = strtok(NULL, ",");

    row = atoi(ptr);
    ptr = strtok(NULL, ",");

    synapseConductance = atof(ptr);
}

void NN_Connection::Save(FILE *file)
{
    char textBuffer[255];

    sprintf(textBuffer, "%d,%d,%.23Lf\n", column, row, synapseConductance);

    uint32_t length = strlen(textBuffer);
    fwrite(textBuffer, sizeof(char), length, file);
}
