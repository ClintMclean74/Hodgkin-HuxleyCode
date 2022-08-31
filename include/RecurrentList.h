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

#ifndef RECURRENTLIST_H
#define RECURRENTLIST_H

#include "pstdint.h"

typedef void* void_ptr;

class RecurrentList
{
    protected:

    private:
        uint32_t size;
        void_ptr* itemsList;
        uint32_t start;
        uint32_t end;
        uint32_t currentIndex;

    public:
        RecurrentList(uint32_t size);
        void Add(void* item);
        void Begin();
        void* GetCurrent();
        void Next();
        void Reset();

        virtual ~RecurrentList();
};

#endif // RECURRENTLIST_H
