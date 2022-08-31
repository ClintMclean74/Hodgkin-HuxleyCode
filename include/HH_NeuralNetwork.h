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

#ifndef HH_NeuralNetwork_H
#define HH_NeuralNetwork_H TRUE

#include "HH_Neurons.h"
#include "Agent.h"

class HH_NeuralNetwork
{
    public:
        static uint32_t* ROWS;
        static const uint32_t MAX_LAYERS = 1000;
        static const uint32_t MAX_ROW_COUNT = 100;
        static uint32_t LAYERS;

        uint32_t* rowCountsForColumns;
        uint32_t layers = LAYERS;

        static double WIDTH;
        static double HEIGHT;
        static double X_GAP;
        static double Y_GAP;

        static uint32_t layer1Length;
        static uint32_t hiddenLayerLength;
        static uint32_t resultLayerLength;

        static uint32_t MAX_LAYER_COLUMN_WIDTH;

        Vector pos;

        HH_Neurons* neurons;

        double weightsMutationRate;

        long double temperature;
        long double rfTemperature;

        uint16_t electromagneticErrorForTemperatureCount = 0;

        Agent* agent;

        double signal;

        HH_NeuralNetwork(uint32_t layers, uint32_t* rowCountsForColumns, Vector* pos = NULL);
        ~HH_NeuralNetwork();
        static HH_NeuralNetwork* Load(char* fileName);
        void AddLayersFromNN(HH_NeuralNetwork* network);
        void Save(char* fileName);
        void saveMembraneVoltagesTraceGraph(FILE* graphVoltagesFile, uint32_t layer, uint32_t row);
        void Copy(HH_NeuralNetwork* neuralNetwork);
        void Mutate();
        void SetTargetTemperature(long double temperature, int32_t column = -1, int32_t row = -1);
        void SetNoise(double noise, int32_t column = -1);
        void SetSynStimulus(double stimulus);
        void SetSynStimulus(double stimulus, uint32_t column, uint32_t row);
        void AdjustSynapseConductance(double value);
        void RegenerateConnections();
        void SetQ(char* q);
        void ClearQ();
        void Process();
        long double* GetVoltageValuesForNeuron(uint32_t layer, uint32_t row);
        void Draw();
        void ResetHH();
        void ResetSpikeCounts();
        void saveMembraneVoltagesTraceGraph(FILE* graphVoltagesFile);
};

typedef HH_NeuralNetwork* HH_NeuralNetwork_ptr;

#endif // HH_NeuralNetwork_H
