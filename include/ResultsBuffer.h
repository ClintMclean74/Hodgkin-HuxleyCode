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

#ifndef ResultsBuffer_H
#define ResultsBuffer_H TRUE

#include "pstdint.h"
#include "HH_Neuron.h"

enum Action {None, RotateRight, Forward, RotateLeft};

class Result
{
    public:
        char* stimulusBuffer;
        uint32_t* resultBuffer;
        Action action;
        uint32_t length;

        Result(uint32_t length);
        void Load(FILE* file);
        void Save(FILE* file);
        bool Equals(char* q);
        bool Equals(HH_Neuron* result);
        bool Equals(uint32_t* result);
        bool Equals(char* q, uint32_t* result);
        bool Equals(char* q, HH_Neuron* result);

        void operator=(Result result);
        ~Result();
};

typedef Result* Result_ptr;


class ResultsBuffer
{
    public:
        uint32_t maxSize = 100;
        uint32_t currentIndex = 0;
        uint32_t error = 0;

        Result_ptr* buffer;

        ResultsBuffer(uint32_t resultLength, uint32_t bufferSize);
        void Load(FILE* file);
        void Save(FILE* file);
        void operator=(ResultsBuffer srcBuffer);

        void Add(char* q, HH_Neuron* result);
        void Add(char* q, HH_Neuron* result, Action action);
        Result* Existing(char* q, uint32_t* result);
        Result* Existing(char* q, HH_Neuron* result);
        uint32_t GetExistingQCount(char* q);
        uint32_t GetExistingACount(HH_Neuron* result);
        Action GetActionForResult(uint32_t* result);
        void Copy(ResultsBuffer* srcBuffer);
        static bool Zeros(char* str);
        void Clear();

        ~ResultsBuffer();
};

typedef ResultsBuffer* ResultsBuffer_ptr;

#endif // ResultsBuffer_H
