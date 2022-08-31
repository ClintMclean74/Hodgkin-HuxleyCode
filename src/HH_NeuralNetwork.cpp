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

#include <string.h>
#include <random>
#include <chrono>
#include "GL/glew.h"
#include "HH_NeuralNetwork.h"
#include "HH_NeuralNetworks.h"
#include "MathUtilities.h"
#include "Simulation.h"

HH_NeuralNetwork::HH_NeuralNetwork(uint32_t layers, uint32_t* rowCountsForColumns, Vector* pos)
{
    this->layers = layers;
    this->rowCountsForColumns = rowCountsForColumns;

    if (pos != NULL)
    {
        this->pos = *pos;
    }

    agent = NULL;

    weightsMutationRate = 0.01;

    neurons = new HH_Neurons(layers, rowCountsForColumns, this);

    if (!HH_NeuralNetworks::NEURAL_NOISE)
        HH_NeuralNetworks::NEURAL_NOISE = new double[neurons->count];
}

HH_NeuralNetwork* HH_NeuralNetwork::Load(char* fileName)
{
    FILE* file = fopen(fileName, "r+");

    char textBuffer[255];

    fgets(textBuffer, 255, file);
    uint32_t layers = atoi(textBuffer);

    uint32_t* rowCountsForColumns = new uint32_t[layers];

    for(uint32_t i = 0; i<layers; i++)
    {
        fgets(textBuffer, 255, file);

        rowCountsForColumns[i] = atoi(textBuffer);
    }

    HH_NeuralNetwork* neuralNetwork = new HH_NeuralNetwork(layers, rowCountsForColumns);

    neuralNetwork->neurons->Load(file);

    fclose(file);

    return neuralNetwork;
}

void HH_NeuralNetwork::Save(char* fileName)
{
    FILE* file = fopen(fileName, "w+");

    neurons->Save(file);

    fclose(file);
}

void HH_NeuralNetwork::saveMembraneVoltagesTraceGraph(FILE* membraneVoltageTraceFile)
{
    char textBuffer[255];

    for(uint32_t i = 0; i<layers; i++)
    {
        for(uint32_t j = 0; j<rowCountsForColumns[i]; j++)
        {
            sprintf(textBuffer, "NeuronL%iR%i", i, j);

            uint32_t length = strlen(textBuffer);
            fwrite(textBuffer, sizeof(char), length, membraneVoltageTraceFile);

            neurons->neurons[i][j].saveMembraneVoltagesTraceGraph(membraneVoltageTraceFile);

            fwrite("\n\n", sizeof(char), 2, membraneVoltageTraceFile);
        }
    }
}

void HH_NeuralNetwork::saveMembraneVoltagesTraceGraph(FILE* graphVoltagesFile, uint32_t layer, uint32_t row)
{
    neurons->neurons[layer][row].saveMembraneVoltagesTraceGraph(graphVoltagesFile);
}

void HH_NeuralNetwork::Copy(HH_NeuralNetwork* neuralNetwork)
{
    if (agent)
    {
        agent->rotationDelta = neuralNetwork->agent->rotationDelta;
        agent->velocity = neuralNetwork->agent->velocity;
    }

    weightsMutationRate = neuralNetwork->weightsMutationRate;

    neurons->Copy(neuralNetwork->neurons);
}

void HH_NeuralNetwork::Mutate()
{
    neurons->Mutate(weightsMutationRate);
}

void HH_NeuralNetwork::SetTargetTemperature(long double temperature, int32_t column, int32_t row)
{
    this->temperature = temperature;

    neurons->SetTargetTemperature(temperature, column, row);
}

void HH_NeuralNetwork::SetNoise(double noise, int32_t column)
{
    neurons->SetNoise(noise, column);
}

void HH_NeuralNetwork::SetSynStimulus(double stimulus)
{
    neurons->SetSynStimulus(stimulus);
}

void HH_NeuralNetwork::SetSynStimulus(double stimulus, uint32_t column, uint32_t row)
{
    neurons->SetSynStimulus(stimulus, column, row);
}

void HH_NeuralNetwork::RegenerateConnections()
{
    long double weightValue;

    // initialize a uniform distribution
    std::uniform_real_distribution<long double> unif(-Simulation::weightsRange, Simulation::weightsRange);

    for (uint32_t i=1; i<layers; i++)
    {
        for (uint32_t j=0; j<rowCountsForColumns[i]; j++)
        {
            for (uint32_t k=0; k<rowCountsForColumns[i-1]; k++)
            {
                weightValue = unif(Simulation::randomGenerator);

                neurons->neurons[i][j].connections.connections[k]->synapseConductance = weightValue;
            }
        }
    }
}

void HH_NeuralNetwork::SetQ(char* q)
{

    neurons->SetQ(q);
}

void HH_NeuralNetwork::ClearQ()
{
    neurons->ClearQ();

}

void HH_NeuralNetwork::Process()
{
    neurons->Process();
}

void HH_NeuralNetwork::Draw()
{
    glPushMatrix();
    glTranslatef(pos.x, pos.y, pos.z+100);

    neurons->Draw();

    glPopMatrix();
}

void HH_NeuralNetwork::ResetHH()
{
    neurons->ResetHH();
}

void HH_NeuralNetwork::ResetSpikeCounts()
{
    neurons->ResetSpikeCounts();
}

HH_NeuralNetwork::~HH_NeuralNetwork()
{
    delete neurons;
}

double HH_NeuralNetwork::WIDTH = 400;
double HH_NeuralNetwork::HEIGHT = 400;
double HH_NeuralNetwork::X_GAP = 120000;
double HH_NeuralNetwork::Y_GAP = 20000;

uint32_t* HH_NeuralNetwork::ROWS = NULL;

uint32_t HH_NeuralNetwork::LAYERS = 2;

uint32_t HH_NeuralNetwork::layer1Length = 1;
uint32_t HH_NeuralNetwork::hiddenLayerLength = 1;
uint32_t HH_NeuralNetwork::resultLayerLength = 1;

uint32_t HH_NeuralNetwork::MAX_LAYER_COLUMN_WIDTH = 15;
