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

#include "RecurrentList.h"
#include <cstddef>

RecurrentList::RecurrentList(uint32_t size)
{
    this->size = size;
    itemsList = new void_ptr[size];

    for(uint32_t i=0; i<size;i++)
        itemsList[i] = NULL;

    start = end = 0;
}

void RecurrentList::Add(void* item)
{
    if (itemsList[end])
        delete itemsList[end];

    itemsList[end] = item;

    end++;

    if (end >= size)
        end = 0;

    if (end == start)
    {
        start++;

        if (start >= size)
        start = 0;
    }
}

void RecurrentList::Begin()
{
    currentIndex = start;
}

void* RecurrentList::GetCurrent()
{
    if (currentIndex != end)
        return itemsList[currentIndex];
    else
        return NULL;
}

void RecurrentList::Next()
{
    currentIndex++;

    if (currentIndex>=size)
        currentIndex=0;
}

void RecurrentList::Reset()
{
    start = end = currentIndex = 0;
}

RecurrentList::~RecurrentList()
{
    for(uint32_t i=0; i<size;i++)
        if (itemsList[i] != NULL)
            delete itemsList[i];

        delete [] itemsList;
}
