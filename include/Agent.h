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

#ifndef AGENT_H
#define AGENT_H

#include <fstream>

#include "Vector.h"
#include "FoodForAgents.h"
#include "ResultsBuffer.h"
#include "RecurrentList.h"

typedef uint32_t* uint32_t_ptr;
typedef double* double_ptr;

class PosAngle
{
    public:
        Vector pos;
        Vector angle;
        char description[255];
};

enum ProcessingStatus{ResettingHH, Stimulating, ProcessignStimulus, ProcessedResult};

class Agent
{
    private:
        double width;
        double length;

        double neuronRadius;

        Vector gridStart;
        Vector gridEnd;

        uint32_t nextStimulusIndex = 0;

        FoodForAgents *foodForAgents_ptr;
        char textBuffer[255];

        Color agentColor;
        Color neuronActiveColor;

        FILE* agentLog;
        FILE* loadingVoltageDataFile;
        FILE* voltagesFile;
        std::ifstream loadingVoltageDataFileBinary;
        std::ofstream savingVoltageDataFileBinary;
        char* loadingVoltageDataBuffer;
        bool resetLoadingData = false;

        RecurrentList *traceList;

        void AddTrace(char* description);

    public:
        static double radius;
        uint32_t index;
        uint32_t sleepIndex;
        double rotationDelta;
        void* neuralNetwork;

        uint32_t closestFoodIndex;

        Vector pos;
        Vector pointing;
        Vector refPointing;
        double velocity;
        double distanceToFood;
        double angleToFood;
        char* currentStimulus;
        char* resultStr;
        Action action;

        double fitness;

        double generationTime;
        double prevGenerationTime;

        double resetHHTime;
        double startStimTime;

        uint32_t **spikeCountsArrays;

        size_t loadingVoltageDataBufferOffset = 0;

        ProcessingStatus processingStatus;

        bool noiseActivated = true;

        Agent();
        Agent(Vector pos);
        void SetNN(void* neuralNetwork);
        void GetSpikeCountArraysForLayers(uint32_t **spikeCountsArrays);
        double GetTemperature();
        void Reset();
        void RegenerateNNConnections();
        void ResetTrace();
        void Forward();
        void Rotate(double angle);
        void ActivateNeuronsFromFoodAngle();
        uint32_t ProcessResult();
        uint32_t Process(bool startInputStimulus);
        int32_t FoodAvailable();
        void EatFood(uint32_t closestFoodIndex);
        void Draw();
        void Draw(Vector agentPos, double angle, double scale = 1, bool justTriangle = false);

        virtual ~Agent();
};

typedef Agent* Agent_ptr;

#endif // AGENT_H
