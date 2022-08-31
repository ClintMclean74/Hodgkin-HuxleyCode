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

#ifndef HH_NeuralNetworks_H
#define HH_NeuralNetworks_H TRUE

#include "HH_NeuralNetwork.h"
#include "Agents.h"

class HH_NeuralNetworks
{
    private:
        char graphicsText[255];
        uint32_t fittestIndexes[2];

    public:
        static const uint32_t MAX_NETWORKS = 1000;
        static uint32_t NETWORKS_ROWS;
        static uint32_t NETWORKS_COLUMNS;
        static double MAX_NEURAL_NOISE;
        static double NEURAL_NOISE_DELTA;
        static double* NEURAL_NOISE;

        double averageError = 0;

        uint32_t neuralNetworksCount = 0;

        bool visible;

        HH_NeuralNetwork_ptr neuralNetworks[MAX_NETWORKS];

        Vector pos;

        Agents *agents;

        HH_NeuralNetworks(Agents *agents);
        ~HH_NeuralNetworks();
        void InitializeNNs(char* fileName = "");
        void Add(HH_NeuralNetwork *neuralNetwork);
        uint32_t* GetFittestNNIndexes();
        void SetResultsBuffers(ResultsBuffer_ptr* srcBuffers);
        void CopyResultsBuffers(uint32_t index);
        void CopyWeights(uint32_t index);
        void CopyWeights(uint32_t* indexes);
        void Mutate();
        void ResetResultsBuffer();
        void ResetTotalErrorAndResultsBuffer();
        void SetMembraneVoltage(long double voltage, bool clamp);
        void SetTargetTemperature(long double temperature, int32_t index = -1, int32_t layerIndex = -1, int32_t neuronIndex = -1);
        void SetNoise(double noise, uint32_t index, int32_t layerIndex = -1);
        void SetSynStimulus(double stimulus);
        void SetSynStimulus(double stimulus, uint32_t column, uint32_t row);
        void AdjustSynapseConductance(double value);
        void Process();
        void SaveVoltages();
        double GetAverageError();
        void Draw();
        void ResetHH();
        void ResetSpikeCounts();
};

#endif // HH_NeuralNetworks_H
