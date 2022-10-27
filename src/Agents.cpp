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

#include <windows.h>
#include "Environment.h"
#include "Agents.h"
#include "MathUtilities.h"
#include "HH_NeuralNetwork.h"
#include "Simulation.h"

Agents::Agents()
{
    Agents::count = 0;

    agents = new Agent_ptr[maxSize];

    for(uint32_t i=0; i<maxSize;i++)
    {
        agents[i] = NULL;
    }
}

Agents::Agents(uint32_t maxSize)
{
    this->maxSize = maxSize;

    Agents();
}

double Agents::GetTotalFitness()
{
    double totalFitness = 0;

    for(uint32_t i=0; i<count;i++)
    {
        totalFitness += agents[i]->fitness;
    }

    return totalFitness;
}

double Agents::GetAvgFitness()
{
    return GetTotalFitness() / count;
}

void Agents::Reset()
{
    for(uint32_t i=0; i<count;i++)
    {
        agents[i]->Reset();
    }
}

void Agents::RegenerateNNConnections()
{
    for(uint32_t i=0; i<count;i++)
    {
        agents[i]->RegenerateNNConnections();

        if (!Simulation::training && i>0) //if not training then copy the previous NN instead of randomizing weights
        {
            ((HH_NeuralNetwork *) agents[i]->neuralNetwork)->Copy((HH_NeuralNetwork *) agents[0]->neuralNetwork);
            ((HH_NeuralNetwork *) agents[i]->neuralNetwork)->neurons->CopyResultsBuffers(((HH_NeuralNetwork *) agents[0]->neuralNetwork)->neurons->resultsBuffers);
        }
    }
}

void Agents::ResetTraces()
{
    for(uint32_t i=0; i<count;i++)
    {
        agents[i]->ResetTrace();
    }
}

void Agents::Add(Agent *agent)
{
    if (agent == NULL)
        agents[count] = new Agent();
    else
        agents[count] = agent;

    agents[count]->index = count;
    count++;
}

void Agents::SetTargetTemperature(long double temperature, int32_t agentIndex)
{
    if (agentIndex == -1)
    {
        for(uint32_t i=0; i<count;i++)
            ((HH_NeuralNetwork *) agents[i]->neuralNetwork)->SetTargetTemperature(temperature);
    }
    else
        ((HH_NeuralNetwork *) agents[agentIndex]->neuralNetwork)->SetTargetTemperature(temperature);
}

void Agents::ActivateNeuronsFromFoodAngle()
{
    Environment::foodForAgents_ptr->SetFoodColor(Food::FoodDefaultColor); //set to green, closest food to agent in ActivateNeuronsFromFoodAngle() is set to gold

    for(uint32_t i=0; i<count;i++)
    {
        agents[i]->ActivateNeuronsFromFoodAngle();
    }
}

void Agents::ProcessIfEating()
{
    int32_t closestFoodIndex;
    for(uint32_t i=0; i<count;i++)
    {
        closestFoodIndex = agents[i]->FoodAvailable();

        if (closestFoodIndex > -1)
            agents[i]->EatFood(closestFoodIndex);
    }
}

uint32_t Agents::Process(uint32_t agentIndex)
{
    uint32_t status = 0;

    if (agentIndex != maxSize)
        agents[agentIndex]->Process(true);
    else
        for(uint32_t i=0; i<count;i++)
        {
            status += agents[i]->Process(true);
        }

    return status;
}

void Agents::Draw()
{
    for(uint32_t i=0; i<count;i++)
    {
        agents[i]->Draw();
    }
}

void Agents::saveMembraneVoltagesTraceGraph()
{
    char textBuffer[255];
    uint32_t time = GetTickCount();

    FILE* agentNNMembraneVoltageTraceFile;
    for(uint32_t i=0; i<count;i++)
    {
        sprintf(textBuffer, "%s/%s/agent%i_NNMembraneVoltageTrace%i.txt", Simulation::saveRootFolder, Simulation::saveMembraneVoltagesTraceGraphFolder, i, time);

        agentNNMembraneVoltageTraceFile = fopen(textBuffer, "w");

        ((HH_NeuralNetwork *) agents[i]->neuralNetwork)->saveMembraneVoltagesTraceGraph(agentNNMembraneVoltageTraceFile);
        fclose(agentNNMembraneVoltageTraceFile);
    }
}

Agents::~Agents()
{
    for(uint32_t i=0; i<count;i++)
    {
        delete agents[i];
    }

    delete [] agents;
}

uint32_t Agents::maxSize = 1000;
uint32_t Agents::count = 0;
