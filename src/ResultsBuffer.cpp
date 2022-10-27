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
#include "ResultsBuffer.h"
#include "HH_NeuralNetwork.h"

Result::Result(uint32_t length)
{
    stimulusBuffer = new char[255];
    resultBuffer = new uint32_t[length];

    this->length = length;
}

void Result::Load(FILE* file)
{
    char textBuffer[255];
    memset(textBuffer, 0, 255);
    memset(stimulusBuffer, 0, 255);

    fgets(stimulusBuffer, 255, file);

    fgets(textBuffer, 255, file);
    char *ptr = strtok(textBuffer, ",");

    for(uint32_t i=0; i<length; i++)
    {
        resultBuffer[i] = atoi(ptr);
        ptr = strtok(NULL, ",");
    }

    fgets(textBuffer, 255, file);
    action = (Action) atoi(textBuffer);
}

void Result::Save(FILE* file)
{
    char textBuffer[255];

    fwrite(stimulusBuffer, sizeof(char), strlen(stimulusBuffer), file);

    fwrite("\n", sizeof(char), 1, file);

    for(uint32_t i=0; i<length; i++)
    {
        if (i>0)
        {
            fwrite(", ", sizeof(char), 2, file);
        }

        sprintf(textBuffer, "%i", resultBuffer[i]);
        fwrite(textBuffer, sizeof(char), strlen(textBuffer), file);
    }

    fwrite("\n", sizeof(char), 1, file);

    sprintf(textBuffer, "%i", action);
    fwrite(textBuffer, sizeof(char), strlen(textBuffer), file);

    fwrite("\n", sizeof(char), 1, file);
}

bool Result::Equals(char* q)
{
    if (strcmp(stimulusBuffer, q) != 0)
        return false;

    return true;
}

bool Result::Equals(HH_Neuron* result)
{
    for(uint32_t i=0; i<length; i++)
    {
        if (resultBuffer[i] != result[i].spikeCount)
        {
            return false;
        }
    }

    return true;
}

bool Result::Equals(uint32_t* result)
{
    for(uint32_t i=0; i<length; i++)
    {
        if (resultBuffer[i] != result[i])
        {
            return false;
        }
    }

    return true;
}

bool Result::Equals(char* q, uint32_t* result)
{
    if (!Equals(q))
        return false;

    if (!Equals(result))
        return false;

    return true;
}

bool Result::Equals(char* q, HH_Neuron* result)
{
    if (!Equals(q))
        return false;

    if (!Equals(result))
        return false;

    return true;
}

void Result::operator=(Result result)
{
    strcpy(stimulusBuffer, result.stimulusBuffer);

    for(uint32_t i=0; i<length; i++)
    {
        resultBuffer[i] = result.resultBuffer[i];
    }

    action = result.action;
}

Result::~Result()
{
    delete [] stimulusBuffer;
    delete [] resultBuffer;
}

ResultsBuffer::ResultsBuffer(uint32_t resultLength, uint32_t bufferSize)
{
    maxSize = bufferSize;

    buffer = new Result_ptr[maxSize];

    for(uint32_t i=0; i<maxSize; i++)
    {
        buffer[i] = new Result(resultLength);
    }

    error = 0;
}

void ResultsBuffer::operator=(ResultsBuffer srcBuffer)
{
    for(uint32_t i=0; i<currentIndex; i++)
    {
        *buffer[i] = *srcBuffer.buffer[i];
    }
}

void ResultsBuffer::Load(FILE* file)
{
    for(uint32_t i=0; i<currentIndex; i++)
    {
        buffer[i]->Load(file);
    }
}

void ResultsBuffer::Save(FILE* file)
{
    char textBuffer[255];
    sprintf(textBuffer, "%i\n", currentIndex);
    fwrite(textBuffer, sizeof(char), strlen(textBuffer), file);

    for(uint32_t i=0; i<currentIndex; i++)
    {
        buffer[i]->Save(file);
    }
}

void ResultsBuffer::Add(char* q, HH_Neuron* result)
{
    if (currentIndex<maxSize)
    {
        strcpy(buffer[currentIndex]->stimulusBuffer, q);

        for(uint32_t i=0; i<buffer[currentIndex]->length; i++)
        {
            buffer[currentIndex]->resultBuffer[i] = result[i].spikeCount;
        }

        currentIndex++;
    }
    else
        currentIndex = 0;
}

void ResultsBuffer::Add(char* q, HH_Neuron* result, Action action)
{
    if (currentIndex<maxSize)
    {
        strcpy(buffer[currentIndex]->stimulusBuffer, q);

        for(uint32_t i=0; i<buffer[currentIndex]->length; i++)
        {
            buffer[currentIndex]->resultBuffer[i] = result[i].spikeCount;
        }

        buffer[currentIndex]->action = action;

        currentIndex++;
    }
    else
        currentIndex = 0;
}

Result* ResultsBuffer::Existing(char* q, uint32_t* result)
{
    for(uint32_t i=0; i<currentIndex; i++)
    {
        if (buffer[i]->Equals(q, result))
        {
            return buffer[i];
        }
    }

    return NULL;
}

Result* ResultsBuffer::Existing(char* q, HH_Neuron* result)
{
    for(uint32_t i=0; i<currentIndex; i++)
    {
        if (buffer[i]->Equals(q, result))
        {
            return buffer[i];
        }
    }

    return NULL;
}

uint32_t ResultsBuffer::GetExistingQCount(char* q)
{
    uint32_t count = 0;

    for(uint32_t i=0; i<currentIndex; i++)
    {
        if (buffer[i]->Equals(q))
        {
            count++;
        }
    }

    return count;
}

uint32_t ResultsBuffer::GetExistingACount(HH_Neuron* result)
{
    uint32_t count = 0;

    for(uint32_t i=0; i<currentIndex; i++)
    {
        if (buffer[i]->Equals(result))
        {
            count++;
        }
    }

    return count;
}

Action ResultsBuffer::GetActionForResult(uint32_t* result)
{
    for(uint32_t i=0; i<currentIndex; i++)
    {
        if (buffer[i]->Equals(result))
        {
            return buffer[i]->action;
        }
    }

    return Action::None;
}

void ResultsBuffer::Copy(ResultsBuffer* srcBuffer)
{
    currentIndex = srcBuffer->currentIndex;

    for(uint32_t i=0; i<currentIndex; i++)
    {
        strcpy(buffer[i]->stimulusBuffer, srcBuffer->buffer[i]->stimulusBuffer);

        for(uint32_t j=0; j<buffer[i]->length; j++)
        {
            buffer[i]->resultBuffer[j] = srcBuffer->buffer[i]->resultBuffer[j];
        }

        buffer[i]->action = srcBuffer->buffer[i]->action;
    }
}

bool ResultsBuffer::Zeros(char* str)
{
    uint32_t strLength = strlen(str);

    for(uint32_t i=0; i<strLength; i++)
    {
        if (str[i] != '0')
            return false;
    }

    return true;
}

void ResultsBuffer::Clear()
{
    currentIndex = 0;

    error = 0;
}

ResultsBuffer::~ResultsBuffer()
{
    for(uint32_t i=0; i<maxSize; i++)
        delete buffer[i];

    delete [] buffer;
};
