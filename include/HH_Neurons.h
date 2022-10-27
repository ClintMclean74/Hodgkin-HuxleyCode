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

#ifndef HH_Neurons_H
#define HH_Neurons_H TRUE

#include "HH_Neuron.h"
#include "ResultsBuffer.h"
#include "MembraneVoltageGraph.h"
#include "Color.h"

typedef uint8_t* uint8_t_ptr;
typedef uint32_t* uint32_t_ptr;

class HH_Neurons
{
    void* neuralNetwork;
    char graphicsText[255];
    char resultStr[255];

    double totalCurrentSpikeCount;
    uint32_t spikeCountSamples;

    FILE* voltagesFile;

    public:
        uint32_t layers;
        uint32_t* rowCountsForColumns;
        uint32_t count;

        HH_Neuron_ptr* neurons;
        Color* graphColor;

        Vector pos;
        uint32_t columns;

        uint32_t signalingCounts[6];
        uint32_t_ptr* layerSignals;

        ResultsBuffer_ptr* resultsBuffers;

        char mostRecentStimulus[255];

        HH_Neurons(uint32_t layers, uint32_t* rowCountsForColumns, void* neuralNetwork);
        ~HH_Neurons();
        void Load(FILE* file);
        void Save(FILE* file);
        void Copy(HH_Neurons* srcNeurons);
        void Mutate(double rate);
        void SetTargetTemperature(long double temperature);
        void SetTargetTemperature(long double temperature, int32_t column);
        void SetTargetTemperature(long double temperature, int32_t column, int32_t row);
        void SetNoise(double noise, int32_t column);
        void SetSynStimulus(double stimulus);
        void SetSynStimulus(double stimulus, int32_t column, int32_t row);
        void SetQ(char* q);
        void ClearQ();
        uint32_t GetCurrentSpikeCount();
        void ResetCurrentSpikeCountAvg();
        uint32_t GetCurrentSpikeCountForLayer(uint32_t index);
        int32_t GetCurrentLeadingSpikeCount(void *nn);
        uint32_t GetTotalSpikeCount();
        uint32_t* GetSpikeCountsArrayForLayer(uint32_t layerIndex, uint32_t* spikeCountsArray);
        void GetSpikeCountArraysForLayers(uint32_t **spikeCountsArrays);
        void DetermineError(bool print);
        void Process();
        void LoadVoltagesForlayer(FILE* file);
        void LoadVoltageData(FILE* file);
        uint32_t LoadVoltagesForLayerBinary(char* buffer, uint32_t offset);
        uint32_t LoadVoltageDataBinary(char* buffer, uint32_t offset);
        void SaveVoltagesForLayer(FILE* file, uint32_t layerIndex);
        void SaveVoltagesForLayerBinary(std::ofstream* file, uint32_t layerIndex);
        void SaveVoltages(FILE* file);
        void SaveVoltagesBinary(std::ofstream* file);
        void AddStimulusResultActionMappingsToBuffers();
        char* GetStringFromResult(uint32_t index);
        Action GetResultAction();
        uint32_t GetExistingACount(HH_Neuron* result, uint32_t layerIndex);
        Vector GetNewMembraneGraphPos(uint32_t column, uint32_t row);
        double GetStrongestABSWeight();
        void Draw();
        void ResetHH();
        void ResetSpikeCounts();
        void CopyResultsBuffers(ResultsBuffer_ptr* srcBuffers);
        void ClearResultsBuffers();
};

#endif // HH_Neurons_H
