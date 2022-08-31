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
#include "NN_Connections.h"
#include "HH_Neurons.h"
#include "GL/glew.h"
#include "MathUtilities.h"
#include "Simulation.h"
#include "HH_NeuralNetwork.h"

NN_Connections::NN_Connections()
{
    count = 0;

    scale = 1;

    connections = new NN_Connection_Ptr[Max_Connections];

    for (int i=0; i<Max_Connections;i++)
        connections[i] = NULL;
}

void NN_Connections::Load(FILE* file)
{
    char textBuffer[255];

    fgets(textBuffer, 255, file);
    count  = atoi(textBuffer);

    for (int i=0; i<count;i++)
        connections[i]->Load(file);
}

void NN_Connections::Save(FILE* file)
{
    char textBuffer[255];
    sprintf(textBuffer, "%d\n", count);
    uint32_t length = strlen(textBuffer);
    fwrite(textBuffer, sizeof(char), length, file);

    for (int i=0; i<count;i++)
        connections[i]->Save(file);
}

void NN_Connections::CopyWeights(NN_Connections* srcConnections, int32_t replaceColumn, int32_t newColumn)
{
    for (int i=0; i<srcConnections->count;i++)
    {
        if (connections[i])
        {
            connections[i]->column = srcConnections->connections[i]->column;
            connections[i]->row = srcConnections->connections[i]->row;
            connections[i]->synapseConductance = srcConnections->connections[i]->synapseConductance;
        }
        else
            if (i < Max_Connections)
                connections[i] = new NN_Connection(srcConnections->connections[i]->column == replaceColumn && replaceColumn != -1 ? newColumn : srcConnections->connections[i]->column, srcConnections->connections[i]->row, srcConnections->connections[i]->synapseConductance);
    }

    count = srcConnections->count;
}

void NN_Connections::Mutate(double rate)
{
    double value;
    uint32_t index;
    double rate2;

    rate2 = rate * 2;
    uint32_t mutationCount = NN_Connections::MUTATION_CONNECTION_PERCENTAGE * count;
    for (int i=0; i<mutationCount;i++)
    {
        index = (double) rand() / RAND_MAX * count;

        if (index>=3)
            index = 2;

        index = i;

        value = (double) rand()/ RAND_MAX * rate - rate/2;

        connections[index]->synapseConductance += value;

        connections[index]->synapseConductance = MathUtilities::Limit(connections[index]->synapseConductance, 1);
    }
}

void NN_Connections::Add(uint32_t column, uint32_t row, long double synapseConductance)
{
    if (count < Max_Connections)
        connections[count++] = new NN_Connection(column, row, synapseConductance);
}

void NN_Connections::SetSynapseConductance(long double value)
{
    for (int i=0; i<count;i++)
    {
        connections[i]->synapseConductance = value;
    }
}

long double NN_Connections::GetExcitorySynapseConductance(void* neurons)
{
    HH_Neuron_ptr* hh_Neurons = (HH_Neuron_ptr*) neurons;

    long double totalExcitorySynapseConductance = 0;

    for (int i=0; i<count;i++)
    {
        if (hh_Neurons[connections[i]->column][connections[i]->row].membraneVoltage>Simulation::preSynapticaActivityVoltageThreshold)
        {
            if (connections[i]->synapseConductance > 0)
            {
                totalExcitorySynapseConductance += connections[i]->synapseConductance;
            }
        }
    }

    return totalExcitorySynapseConductance;
}

long double NN_Connections::GetInhibitorySynapseConductance(void* neurons)
{
    HH_Neuron_ptr* hh_Neurons = (HH_Neuron_ptr*) neurons;

    long double totalInhibitorySynapseConductance = 0;

    for (int i=0; i<count;i++)
    {
        if (hh_Neurons[connections[i]->column][connections[i]->row].membraneVoltage>Simulation::preSynapticaActivityVoltageThreshold)
        {
            if (connections[i]->synapseConductance < 0)
                totalInhibitorySynapseConductance += connections[i]->synapseConductance;
        }
    }

    return totalInhibitorySynapseConductance;
}

long double NN_Connections::GetCurrentSignalingConductance(void* neurons, double maxSynapseConductanceForArea)
{
    HH_Neuron_ptr* hh_Neurons = (HH_Neuron_ptr*) neurons;

    long double totalSynapseConductance = 0;

    uint32_t activeConnectionsCount = 0;
    for (int i=0; i<count;i++)
    {
        if (hh_Neurons[connections[i]->column][connections[i]->row].membraneVoltage>20)
        {
            totalSynapseConductance += connections[i]->synapseConductance;

            activeConnectionsCount++;
        }
    }

    if (activeConnectionsCount==0)
        return 0;
    else
        return totalSynapseConductance;
}

double NN_Connections::GetStrongestABSWeight()
{
    double maxWeight = 0;

    double weight;

    for (uint32_t i=0; i<count;i++)
    {
        weight = connections[i]->synapseConductance;

        if (weight > maxWeight)
            maxWeight = weight;
    }

    return maxWeight;
}

Vector NN_Connections::GetMinMaxWeight()
{
    Vector minMaxWeight;

    double weight;

    for (uint32_t i=0; i<count;i++)
    {
        weight = connections[i]->synapseConductance;

        if (weight < minMaxWeight.x)
            minMaxWeight.x = weight;

        if (weight > minMaxWeight.y)
            minMaxWeight.y = weight;
    }

    return minMaxWeight;
}

void NN_Connections::Draw(void* parent, void* neurons, double maxABSWeight)
{
    HH_Neuron_ptr* hh_Neurons = (HH_Neuron_ptr*) neurons;
    HH_Neuron* thisNeuron = (HH_Neuron*) parent;

    HH_Neuron* preSynapticNeuron;

    double lineWidth;
    for (int i=0; i<count;i++)
    {
        preSynapticNeuron = &hh_Neurons[connections[i]->column][connections[i]->row];

        lineWidth = fabs(connections[i]->synapseConductance)/maxABSWeight * NN_Connections::MAX_GRAPHICS_CONNECTION_THICKNESS;

        if (connections[i]->synapseConductance>0)
            glColor3f(0, 1, 0);
        else
            glColor3f(1, 0, 0);

        if (preSynapticNeuron->membraneVoltage>0)
        {
            lineWidth *= 10;
            glColor3f(1, 1, 0);
        }

        if (lineWidth>0)
        {
            glLineWidth(lineWidth);

            glBegin(GL_LINES);
            glVertex2f(preSynapticNeuron->pos.x + MembraneVoltageGraph::WIDTH, preSynapticNeuron->pos.y);
            glVertex2f(thisNeuron->pos.x, thisNeuron->pos.y);
            glEnd();
        }
    }
}

NN_Connections::~NN_Connections()
{
    for (int i=0; i<count;i++)
    {
        delete connections[i];
    }

    delete [] connections;
}

double NN_Connections::MutationRate = 0.00025;

double NN_Connections::MUTATION_CONNECTION_PERCENTAGE = 1.0;

double NN_Connections::MAX_GRAPHICS_CONNECTION_THICKNESS_NEAR = 3.0;
double NN_Connections::MAX_GRAPHICS_CONNECTION_THICKNESS_FAR = 1.0;
double NN_Connections::MAX_GRAPHICS_CONNECTION_THICKNESS = NN_Connections::MAX_GRAPHICS_CONNECTION_THICKNESS_FAR;
