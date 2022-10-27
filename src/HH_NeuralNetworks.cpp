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
#include "HH_NeuralNetworks.h"
#include "HH_NeuralNetwork.h"
#include "GL/glew.h"
#include "Simulation.h"

typedef double* double_ptr;

HH_NeuralNetworks::HH_NeuralNetworks(Agents *agents)
{
    visible = true;
    HH_NeuralNetwork::ROWS = new uint32_t[HH_NeuralNetwork::LAYERS];

    HH_NeuralNetwork::ROWS[0] = HH_NeuralNetwork::layer1Length;
    uint8_t i = 1;

    for (; i<HH_NeuralNetwork::LAYERS-1; i++)
        HH_NeuralNetwork::ROWS[i] = HH_NeuralNetwork::hiddenLayerLength;

    HH_NeuralNetwork::ROWS[i] = HH_NeuralNetwork::resultLayerLength;

    this->agents = agents;
}

void HH_NeuralNetworks::InitializeNNs(char* fileName)
{
    pos.x = -MembraneVoltageGraph::WIDTH * (HH_NeuralNetworks::NETWORKS_COLUMNS/2);

    for (uint32_t i = 0; i < HH_NeuralNetworks::NETWORKS_COLUMNS; i++)
    {
        pos.y = -MembraneVoltageGraph::HEIGHT * (HH_NeuralNetworks::NETWORKS_ROWS/2);

        for (uint32_t j = 0; j < HH_NeuralNetworks::NETWORKS_ROWS; j++)
        {
            if (strlen(fileName)==0)
            {
                neuralNetworks[neuralNetworksCount] = new HH_NeuralNetwork(HH_NeuralNetwork::LAYERS, HH_NeuralNetwork::ROWS, &pos);

                if (!Simulation::training && neuralNetworksCount>0) //if not training then copy the previous NN instead of randomizing weights
                {
                    neuralNetworks[neuralNetworksCount]->Copy(neuralNetworks[0]);
                    neuralNetworks[neuralNetworksCount]->neurons->CopyResultsBuffers(neuralNetworks[0]->neurons->resultsBuffers);
                }
            }
            else
            {
                neuralNetworks[neuralNetworksCount] = HH_NeuralNetwork::Load(fileName);

                if (neuralNetworksCount == 0)
                    HH_NeuralNetwork::LAYERS = neuralNetworks[neuralNetworksCount]->layers;
            }

            neuralNetworks[neuralNetworksCount]->agent = new Agent();
            neuralNetworks[neuralNetworksCount]->agent->SetNN(neuralNetworks[neuralNetworksCount]);
            Simulation::agents->Add(neuralNetworks[neuralNetworksCount]->agent);

            neuralNetworks[neuralNetworksCount++]->pos = pos;

            if (neuralNetworksCount >= Simulation::rfInducedTemperaturesFromPulsesForAgentsCount + 1)
            {
                i = HH_NeuralNetworks::NETWORKS_COLUMNS;
                break;
            }

            pos.y += (HH_NeuralNetwork::HEIGHT + HH_NeuralNetwork::Y_GAP);
        }

        pos.x += (HH_NeuralNetwork::WIDTH + HH_NeuralNetwork::X_GAP);
    }

    for (uint32_t i = 0; i < Simulation::agentsWithoutNoiseCount; i++)
    {
        if (Simulation::agentsWithoutNoise[i] < Simulation::agents->count)
            Simulation::agents->agents[Simulation::agentsWithoutNoise[i]]->noiseActivated = false;
    }
}

void HH_NeuralNetworks::Add(HH_NeuralNetwork *neuralNetwork)
{
    neuralNetworks[neuralNetworksCount] = neuralNetwork;

    Vector pos;

    pos.x = -MembraneVoltageGraph::WIDTH * (HH_NeuralNetworks::NETWORKS_COLUMNS/2);

    pos.y = -MembraneVoltageGraph::HEIGHT * (HH_NeuralNetworks::NETWORKS_ROWS/2);

    neuralNetworks[neuralNetworksCount]->pos = pos;

    neuralNetworksCount++;
}

uint32_t* HH_NeuralNetworks::GetFittestNNIndexes()
{
    double maxFitness = -1;

    for (uint32_t i = 0; i < neuralNetworksCount; i++)
    {
        if (neuralNetworks[i]->agent->fitness > maxFitness || maxFitness == -1)
        {
            maxFitness = neuralNetworks[i]->agent->fitness;
            fittestIndexes[0] = i;
        }
    }

    maxFitness = -1;

    for (uint32_t i = 0; i < neuralNetworksCount; i++)
    {
        if (i != fittestIndexes[0])
        {
            if (neuralNetworks[i]->agent->fitness > maxFitness || maxFitness == -1)
            {
                maxFitness = neuralNetworks[i]->agent->fitness;
                fittestIndexes[1] = i;
            }
        }
    }

    if (maxFitness == -1)
        fittestIndexes[1] = fittestIndexes[0];


    return &fittestIndexes[0];
}

void HH_NeuralNetworks::CopyResultsBuffers(uint32_t index)
{
    for (uint32_t i = 0; i < neuralNetworksCount; i++)
    {
        if (i!= index)
            neuralNetworks[i]->neurons->CopyResultsBuffers(neuralNetworks[index]->neurons->resultsBuffers);
    }
}

void HH_NeuralNetworks::CopyWeights(uint32_t index)
{
    for (uint32_t i = 0; i < neuralNetworksCount; i++)
    {
        if (i != index)
            neuralNetworks[i]->Copy(neuralNetworks[index]);
    }
}

void HH_NeuralNetworks::CopyWeights(uint32_t* indexes)
{
    uint32_t startIndex = 0;
    uint32_t endIndex = neuralNetworksCount/2;

    for (uint32_t i = startIndex; i < endIndex; i++)
    {
        if (i != indexes[0])
            neuralNetworks[i]->Copy(neuralNetworks[indexes[0]]);
    }

    for (uint32_t i = endIndex; i < neuralNetworksCount; i++)
    {
        if (i != indexes[1])
            neuralNetworks[i]->Copy(neuralNetworks[indexes[1]]);
    }
}

void HH_NeuralNetworks::Mutate()
{
    for (uint32_t i = 1; i < neuralNetworksCount; i++)
    {
        neuralNetworks[i]->Mutate();

        neuralNetworks[i]->neurons->ClearResultsBuffers(); //Clear the results buffers because there could now be different stimulus->results
    }
}

void HH_NeuralNetworks::ResetResultsBuffer()
{
    for (uint32_t i = 0; i < neuralNetworksCount; i++)
    {
        neuralNetworks[i]->neurons->ClearResultsBuffers();
    }
}

void HH_NeuralNetworks::SetTargetTemperature(long double temperature, int32_t index, int32_t layerIndex, int32_t neuronIndex)
{
        if (index > -1 && index < neuralNetworksCount)
            neuralNetworks[index]->SetTargetTemperature(temperature, layerIndex, neuronIndex);
        else
        {
            for (uint32_t i = 0; i < neuralNetworksCount; i++)
            {
                neuralNetworks[i]->SetTargetTemperature(temperature);
            }
        }
}

void HH_NeuralNetworks::SetNoise(double noise, uint32_t index, int32_t layerIndex)
{
    if (layerIndex == -1)
        neuralNetworks[index]->SetNoise(noise);
    else
        neuralNetworks[index]->SetNoise(noise, layerIndex);
}

void HH_NeuralNetworks::SetSynStimulus(double stimulus)
{
    for (uint32_t i = 0; i < neuralNetworksCount; i++)
        neuralNetworks[i]->SetSynStimulus(stimulus);
}

void HH_NeuralNetworks::SetSynStimulus(double stimulus, uint32_t column, uint32_t row)
{
    for (uint32_t i = 0; i < neuralNetworksCount; i++)
        neuralNetworks[i]->SetSynStimulus(stimulus, column, row);
}

void HH_NeuralNetworks::Process()
{
    uint32_t neuronsCount = neuralNetworks[0]->neurons->count;

    for (uint32_t i = 0; i < neuronsCount; i++)
    {
        HH_NeuralNetworks::NEURAL_NOISE[i] = (double) rand()/RAND_MAX * Simulation::MAX_NEURAL_NOISE - Simulation::MAX_NEURAL_NOISE/2;
    }

    for (uint32_t i = 0; i < neuralNetworksCount; i++)
    {
        neuralNetworks[i]->Process();
    }
}

void HH_NeuralNetworks::Draw()
{
    if (visible)
    {
        for (uint32_t i = 1; i < neuralNetworksCount; i++)
        {
            neuralNetworks[i]->Draw();
        }
    }

    neuralNetworks[0]->Draw();
}

void HH_NeuralNetworks::ResetHH()
{
    for (uint32_t i = 0; i < neuralNetworksCount; i++)
    {
        neuralNetworks[i]->ResetHH();
    }
}

void HH_NeuralNetworks::ResetSpikeCounts()
{
    for (uint32_t i = 0; i < neuralNetworksCount; i++)
    {
        neuralNetworks[i]->ResetSpikeCounts();
    }
}

HH_NeuralNetworks::~HH_NeuralNetworks()
{
    for (uint32_t i = 0; i < neuralNetworksCount; i++)
    {
        delete neuralNetworks[i];
    }

    delete neuralNetworks;
}

double* HH_NeuralNetworks::NEURAL_NOISE = NULL;

double HH_NeuralNetworks::NEURAL_NOISE_DELTA = 1;
