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
    //scale = srcConnections->scale;

    //sigmoidGradient = srcConnections->sigmoidGradient;

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

//double NN_Connections::MutationRate = 0.00025;
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

        ////value = (double) rand()/ RAND_MAX * rate2 - rate2/2;
        connections[index]->synapseConductance += value;

        connections[index]->synapseConductance = MathUtilities::Limit(connections[index]->synapseConductance, 1);
    }

    ////double value = (double) rand() / RAND_MAX * 10;

    /*////for (int i=0; i<count;i++)
    {
        value = (double) rand() / RAND_MAX * 10;

        /*if (value < 1)
            connections[i]->synapseConductance = 0;
        else
            */

/*////


        {
            value = (double) rand()/ RAND_MAX * rate - rate/2;
            connections[i]->synapseConductance += value;
        }

        connections[i]->synapseConductance = MathUtilities::Limit(connections[i]->synapseConductance, 1);

        //connections[i]->synapseConductance = MathUtilities::Round(connections[i]->synapseConductance, 6);
    }*/
}

/*void NN_Connections::MutateScale(double rate)
{
    double value = (double) rand()/ RAND_MAX * rate - rate/2;

    scale += value;
}*/

/*void NN_Connections::Mutate()
{
    double value;

    for (int i=0; i<count;i++)
    {
        value = (double) rand()/ RAND_MAX * MutationRate - MutationRate/2;
        connections[i]->synapseConductance += value;

        connections[i]->synapseConductance = MathUtilities::Round(connections[i]->synapseConductance, 6);
    }
}*/

void NN_Connections::Add(uint32_t column, uint32_t row, double synapseConductance)
{
    if (count < Max_Connections)
        connections[count++] = new NN_Connection(column, row, synapseConductance);
}

void NN_Connections::AdjustAllSynapseConductance(double value)
{
    for (int i=0; i<count;i++)
    {
        connections[i]->synapseConductance += value;
    }
}

double NN_Connections::GetCurrentSignallingConductance(void* neurons, double maxSynapseConductanceForArea)
{
    HH_Neuron_ptr* hh_Neurons = (HH_Neuron_ptr*) neurons;

    double totalSynapseConductance = 0;
    for (int i=0; i<count;i++)
    {
        if (hh_Neurons[connections[i]->column][connections[i]->row].membraneVoltage>0)
        {
            ////connections[i]->synapseConductance -= HH_NeuralNetwork::weightActivityWeakenRate;

            totalSynapseConductance += (connections[i]->synapseConductance * scale * maxSynapseConductanceForArea);
        }
    }

    return totalSynapseConductance;
}


void NN_Connections::SetNeuralConnectionActivityTime(void* neurons)
{
    HH_Neuron_ptr* hh_Neurons = (HH_Neuron_ptr*) neurons;

    for (int i=0; i<count;i++)
    {
        if (hh_Neurons[connections[i]->column][connections[i]->row].membraneVoltage>0)
        {
            connections[i]->neuralConnectionActivityTime = Simulation::currentSimTime;
        }
    }
}

void NN_Connections::Reinforce(double value, bool usedConnections)
{
    double delta, elapsedTime;
    for (uint32_t i=0; i<count;i++)
    {
        elapsedTime = Simulation::currentSimTime - connections[i]->neuralConnectionActivityTime;

        if (usedConnections)
        {
            if (connections[i]->neuralConnectionActivityTime > 0)
            {
                if (elapsedTime < 1)
                    elapsedTime = 1;

                delta = value/((elapsedTime) * HH_NeuralNetwork::rewardDecayRate);
            }
        }
        else
        {
            delta = value;////((elapsedTime) * HH_NeuralNetwork::reinforcementRegenerationRate);
        }

        connections[i]->synapseConductance = connections[i]->synapseConductance + delta;
            connections[i]->synapseConductance = MathUtilities::Limit(connections[i]->synapseConductance, 1);
    }
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

    //double weightRange = minMaxWeight.y - minMaxWeight.x;

    double lineWidth;
    for (int i=0; i<count;i++)
    {
        preSynapticNeuron = &hh_Neurons[connections[i]->column][connections[i]->row];

        //lineWidth = (connections[i]->synapseConductance-minMaxWeight.x)/weightRange * 10;

        //lineWidth = abs(connections[i]->synapseConductance-minMaxWeight.x)/weightRange * 10;

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
        /*else
        {
            glColor3f(1, 1, 1);
        }*/

        if (lineWidth>0)
        {
            glLineWidth(lineWidth);

            glBegin(GL_LINES);
            glVertex2f(preSynapticNeuron->pos.x + Graph::WIDTH, preSynapticNeuron->pos.y);
            glVertex2f(thisNeuron->pos.x, thisNeuron->pos.y);
            glEnd();
        }
    }
}

double NN_Connections::MutationRate = 0.00025;
//double NN_Connections::MutationRate = 0.001;

double NN_Connections::MUTATION_CONNECTION_PERCENTAGE = 1.0;


double NN_Connections::MAX_GRAPHICS_CONNECTION_THICKNESS_NEAR = 3.0;
double NN_Connections::MAX_GRAPHICS_CONNECTION_THICKNESS_FAR = 1.0;
double NN_Connections::MAX_GRAPHICS_CONNECTION_THICKNESS = NN_Connections::MAX_GRAPHICS_CONNECTION_THICKNESS_FAR;
