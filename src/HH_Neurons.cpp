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
#include <sysinfoapi.h>
#include <random>
#include <chrono>

#include "GL/glew.h"
#include "GraphicsAndUI.h"
#include "HH_Neurons.h"
#include "HH_NeuralNetwork.h"
#include "HH_NeuralNetworks.h"
#include "Simulation.h"
#include "MathUtilities.h"

HH_Neurons::HH_Neurons(uint32_t layers, uint32_t* rowCountsForColumns, void* neuralNetwork)
{
    HH_NeuralNetwork* networkPtr = (HH_NeuralNetwork*) neuralNetwork;

    this->layers = layers;
    this->rowCountsForColumns = new uint32_t[HH_NeuralNetwork::MAX_LAYERS];

    for (uint32_t i=0; i<layers; i++)
    {
        this->rowCountsForColumns[i] = rowCountsForColumns[i];
    }

    this->neuralNetwork = neuralNetwork;

    neurons = new HH_Neuron_ptr[HH_NeuralNetwork::MAX_LAYERS];

    resultsBuffers = new ResultsBuffer_ptr[HH_NeuralNetwork::MAX_LAYERS];

    count = 0;
    for (uint32_t i=0; i<layers; i++)
    {
        neurons[i] = new HH_Neuron[rowCountsForColumns[i]];

        resultsBuffers[i] = new ResultsBuffer(rowCountsForColumns[i], Simulation::stimulusArrayLength);

        count += rowCountsForColumns[i];
    }

    for (uint32_t i=layers; i<HH_NeuralNetwork::MAX_LAYERS; i++)
    {
        resultsBuffers[i] = NULL;
    }

    for (uint32_t i=0; i<layers; i++)
    {
        for (uint32_t j=0; j<rowCountsForColumns[i]; j++)
            neurons[i][j].neuralNetwork = neuralNetwork;
    }

    long double weightValue;

    std::uniform_real_distribution<long double> unif(-Simulation::weightsRange, Simulation::weightsRange);

    for (uint32_t i=1; i<layers; i++)
    {
        for (uint32_t j=0; j<rowCountsForColumns[i]; j++)
        {
            for (uint32_t k=0; k<rowCountsForColumns[i-1]; k++)
            {
                if (!Simulation::setWeightValue)
                {
                    weightValue = unif(Simulation::randomGenerator);
                }
                else
                    weightValue = Simulation::weightValue;

                neurons[i][j].connections.Add(i-1, k, weightValue);
            }
        }
    }

    pos.x = 0;

    Vector pos;
    for (uint32_t i=0; i<layers; i++)
    {
        for (uint32_t j=0; j<rowCountsForColumns[i]; j++)
        {
            pos = GetNewMembraneGraphPos(i, j);
            neurons[i][j].SetPos(pos.x, pos.y, 0.0);
        }
    }

    graphColor = new Color(0, 1, 0);

    mostRecentStimulus[0] = 0;

    totalCurrentSpikeCount = 0;
    spikeCountSamples = 0;
}

void HH_Neurons::Load(FILE* file)
{
    for (uint32_t i=0; i<layers; i++)
    {
        for (uint32_t j=0; j<rowCountsForColumns[i]; j++)
        {
            neurons[i][j].Load(file);
        }
    }

    char textBuffer[255];
    fgets(textBuffer, 255, file);
    uint32_t resultsBuffersCount = atoi(textBuffer);

    resultsBuffers = new ResultsBuffer_ptr[resultsBuffersCount];

    uint32_t resultsBufferLength;
    for (uint32_t i=0; i<layers; i++)
    {
        fgets(textBuffer, 255, file);
        resultsBufferLength = atoi(textBuffer);

        resultsBuffers[i] = new ResultsBuffer(rowCountsForColumns[i], rowCountsForColumns[i] * 10);
        resultsBuffers[i]->currentIndex = resultsBufferLength;

        resultsBuffers[i]->Load(file);
    }
}

void HH_Neurons::Save(FILE* file)
{
    char textBuffer[255];
    sprintf(textBuffer, "%d\n", layers);

    uint32_t length = strlen(textBuffer);
    fwrite(textBuffer, sizeof(char), length, file);

    for (uint32_t i=0; i<layers; i++)
    {
        sprintf(textBuffer, "%d\n", rowCountsForColumns[i]);
        length = strlen(textBuffer);
        fwrite(textBuffer, sizeof(char), length, file);
    }

    for (uint32_t i=0; i<layers; i++)
    {
        for (uint32_t j=0; j<rowCountsForColumns[i]; j++)
        {
            neurons[i][j].Save(file);
        }
    }

    sprintf(textBuffer, "%d\n", layers);
    fwrite(textBuffer, sizeof(char), strlen(textBuffer), file);

    for (uint32_t i=0; i<layers; i++)
    {
        resultsBuffers[i]->Save(file);
    }
}

void HH_Neurons::Copy(HH_Neurons* srcNeurons)
{
    ClearResultsBuffers();

    for (uint32_t i=0; i<layers; i++)
    {
        for (uint32_t j=0; j<rowCountsForColumns[i]; j++)
        {
            neurons[i][j].CopyWeights(&srcNeurons->neurons[i][j]);
        }
    }
}

void HH_Neurons::Mutate(double rate)
{
    for (uint32_t i=0; i<layers; i++)
    {
        for (uint32_t j=0; j<rowCountsForColumns[i]; j++)
        {
            neurons[i][j].Mutate(rate);
        }
    }
}

void HH_Neurons::SetTargetTemperature(long double temperature, int32_t column, int32_t row)
{
    if (column == -1)
    {
        for (uint32_t i=0; i<layers; i++)
        {
            for (uint32_t j=0; j<rowCountsForColumns[i]; j++)
            {
                neurons[i][j].SetTargetTemperature(temperature);
            }
        }
    }
    else
    {
        if (row == -1)
        {
            for (uint32_t j=0; j<rowCountsForColumns[column]; j++)
                neurons[column][j].SetTargetTemperature(temperature);
        }
        else
            neurons[column][row].SetTargetTemperature(temperature);
    }
}

void HH_Neurons::SetNoise(double noise, int32_t column)
{
    if (column == -1)
    {
        for (uint32_t i=0; i<layers; i++)
        {
            for (uint32_t j=0; j<rowCountsForColumns[i]; j++)
            {
                neurons[i][j].SetNoise(noise);
            }
        }
    }
    else
    {
        for (uint32_t j=0; j<rowCountsForColumns[column]; j++)
            neurons[column][j].SetNoise(noise);
    }
}

void HH_Neurons::SetSynStimulus(double stimulus)
{
    for (uint32_t i=0; i<layers; i++)
    {
        for (uint32_t j=0; j<rowCountsForColumns[i]; j++)
        {
            neurons[i][j].setStimulus = stimulus;
        }
    }
}

void HH_Neurons::SetSynStimulus(double stimulus, int32_t column, int32_t row)
{
    neurons[column][row].setStimulus = stimulus;
}

void HH_Neurons::SetQ(char* q)
{
    uint32_t qLength = strlen(q);
    uint32_t n=0;

    bool stimulus = false;
    for (uint32_t i=0; i<qLength;i++)
    {
        if (q[i] == '1')
        {
            stimulus = true;

            neurons[0][n].setStimulus = Simulation::stimulusValue;
        }
        else
            if (q[i] == '0')
            {
                neurons[0][n].setStimulus = 0;
            }

        n++;

        if (n>=rowCountsForColumns[0])
            break;
    }

    if (stimulus)
        strcpy(mostRecentStimulus, q);
}

void HH_Neurons::ClearQ()
{
    for (uint32_t i=0; i<rowCountsForColumns[0];i++)
    {
        neurons[0][i].setStimulus = 0;
    }
}

void HH_Neurons::Process()
{
    HH_NeuralNetwork* networkPtr = (HH_NeuralNetwork*) neuralNetwork;

    uint32_t k =0;
    for (uint32_t i=0; i<layers; i++)
    {
        for (uint32_t j=0; j<rowCountsForColumns[i]; j++)
        {
            if (!Simulation::training && networkPtr->agent->noiseActivated)
                neurons[i][j].Process(HH_NeuralNetworks::NEURAL_NOISE[k++]);
            else
                neurons[i][j].Process(0);
        }
    }
}

void HH_Neurons::LoadVoltagesForlayer(FILE* file)
{
    uint32_t layerIndex, neuronsCountForLayer;
    double voltage;

    char textBuffer[999999];
    memset(textBuffer, 0, 999999);

    char *ptr;

    fgets(textBuffer, 999999, file);

    ptr = strtok(textBuffer, ",");
    layerIndex = atoi(ptr);

    ptr = strtok(NULL, ",");
    neuronsCountForLayer = atoi(ptr);

    uint32_t j = 0;
    while (ptr != NULL)
    {
        ptr = strtok (NULL, ",");

        voltage = atof(ptr);

        neurons[layerIndex][j].membraneVoltage = voltage;

        j++;
    }
}

void HH_Neurons::LoadVoltageData(FILE* file)
{
    for (uint32_t i=0; i<layers; i++)
    {
        LoadVoltagesForlayer(file);
    }
}

uint32_t HH_Neurons::LoadVoltagesForLayerBinary(char* buffer, uint32_t offset)
{
    uint32_t layerIndex = *reinterpret_cast<uint32_t*>(buffer + offset);
    offset += sizeof(uint32_t);

    uint32_t neuronsCountForLayer = *reinterpret_cast<uint32_t*>(buffer + offset);
    offset += sizeof(uint32_t);

    double voltage;

    uint32_t j = 0;
    for (uint32_t i=0; i<neuronsCountForLayer; i++)
    {
        voltage = *reinterpret_cast<double*>(buffer + offset);
        offset += sizeof(double);

        neurons[layerIndex][j].membraneVoltage = voltage;

        j++;
    }

    return offset;
}

uint32_t HH_Neurons::LoadVoltageDataBinary(char* buffer, uint32_t offset)
{
    for (uint32_t i=0; i<layers; i++)
    {
        offset = LoadVoltagesForLayerBinary(buffer, offset);
    }

    return offset;
}

void HH_Neurons::SaveVoltagesForLayer(FILE* file, uint32_t layerIndex)
{
    char textBuffer[255];
    size_t length;

    sprintf(textBuffer, "%i, ", layerIndex);
    length = strlen(textBuffer);
    fwrite(textBuffer, sizeof(char), length, file);

    sprintf(textBuffer, "%i", rowCountsForColumns[layerIndex]);
    length = strlen(textBuffer);
    fwrite(textBuffer, sizeof(char), length, file);

    for (uint32_t j=0; j<rowCountsForColumns[layerIndex]; j++)
    {
        sprintf(textBuffer, ",%f", neurons[layerIndex][j].membraneVoltage);
        length = strlen(textBuffer);
        fwrite(textBuffer, sizeof(char), length, file);
    }

    fwrite("\n", sizeof(char), 1, file);
}

void HH_Neurons::SaveVoltages(FILE* file)
{
    for (uint32_t i=0; i<layers;i++)
    {
        SaveVoltagesForLayer(file, i);
    }
}

void HH_Neurons::SaveVoltagesForLayerBinary(std::ofstream* file, uint32_t layerIndex)
{
    file->write(reinterpret_cast<const char*> (&layerIndex), sizeof(uint32_t));
    file->write(reinterpret_cast<const char*> (&rowCountsForColumns[layerIndex]), sizeof(uint32_t));

    for (uint32_t j=0; j<rowCountsForColumns[layerIndex]; j++)
    {
        file->write(reinterpret_cast<const char*> (&neurons[layerIndex][j].membraneVoltage), sizeof(double));
    }
}

void HH_Neurons::SaveVoltagesBinary(std::ofstream* file)
{
    uint32_t layerIndex=0;

    for (uint32_t i=0; i<layers;i++)
    {
        SaveVoltagesForLayerBinary(file, i);
    }
}

void HH_Neurons::AddStimulusResultActionMappingsToBuffers()
{
    if (strcmp(mostRecentStimulus, "")!=0 && strcmp(mostRecentStimulus, "000000000")!=0)
    {
        uint32_t* spikeCountsArray = new uint32_t[HH_NeuralNetwork::MAX_ROW_COUNT];

        HH_NeuralNetwork* networkPtr = (HH_NeuralNetwork*) neuralNetwork;
        Result *result;

        for (uint32_t i=0; i<layers;i++) //for each layer check if the stimulus and the result is in the buffer
        {
            result = resultsBuffers[i]->Existing(mostRecentStimulus, GetSpikeCountsArrayForLayer(i, spikeCountsArray));

            if (result) //if already in the buffer then get the specified action
            {
                networkPtr->agent->action = result->action;
            }
            else //if not then get the action from the stimulus and add to the buffer the stimulus, result and action
            {
                if (strcmp(mostRecentStimulus, Simulation::foodRightStimulus)==0)
                    networkPtr->agent->action = Action::RotateRight;
                else
                    if (strcmp(mostRecentStimulus, Simulation::foodForwardStimulus)==0)
                        networkPtr->agent->action = Action::Forward;
                    else
                        if (strcmp(mostRecentStimulus, Simulation::foodLeftStimulus)==0)
                            networkPtr->agent->action = Action::RotateLeft;

                resultsBuffers[i]->Add(mostRecentStimulus, neurons[i], networkPtr->agent->action);
            }
        }

        delete spikeCountsArray;
    }
}

char* HH_Neurons::GetStringFromResult(uint32_t index)
{
    for (uint32_t i=0; i<rowCountsForColumns[index]; i++)
    {
        itoa(neurons[index][i].spikeCount, &resultStr[i], 10);
    }

    return resultStr;
}

Action HH_Neurons::GetResultAction()
{
    uint32_t* spikeCountsArray = new uint32_t[HH_NeuralNetwork::MAX_ROW_COUNT];

    Action action = resultsBuffers[layers-1]->GetActionForResult(GetSpikeCountsArrayForLayer(layers - 1, spikeCountsArray));

    delete spikeCountsArray;

    return  action;
}

uint32_t HH_Neurons::GetExistingACount(HH_Neuron* result, uint32_t layerIndex)
{
    return resultsBuffers[layerIndex]->GetExistingACount(result);
}

Vector HH_Neurons::GetNewMembraneGraphPos(uint32_t column, uint32_t row)
{
    Vector newGraphXY;

	newGraphXY.x = ((column % HH_NeuralNetwork::MAX_LAYER_COLUMN_WIDTH) * (MembraneVoltageGraph::WIDTH + MembraneVoltageGraph::X_GAP));

	if (column >= HH_NeuralNetwork::MAX_LAYER_COLUMN_WIDTH)
        newGraphXY.x += MembraneVoltageGraph::WIDTH + MembraneVoltageGraph::X_GAP;

	newGraphXY.y = row * (MembraneVoltageGraph::HEIGHT + MembraneVoltageGraph::Y_GAP);

	uint32_t maxNeuronRows = 0;
	for (uint32_t i=0; i<layers; i++)
    {
        if (rowCountsForColumns[i] > maxNeuronRows)
            maxNeuronRows= rowCountsForColumns[i];
    }

	newGraphXY.y -= (column / HH_NeuralNetwork::MAX_LAYER_COLUMN_WIDTH) * (maxNeuronRows * (MembraneVoltageGraph::HEIGHT + MembraneVoltageGraph::Y_GAP) + MembraneVoltageGraph::Y_GAP);

	if (column == 0)
	{
	    newGraphXY.y += (maxNeuronRows * (MembraneVoltageGraph::HEIGHT + MembraneVoltageGraph::Y_GAP))/2;
        newGraphXY.y -= (rowCountsForColumns[column] * (MembraneVoltageGraph::HEIGHT + MembraneVoltageGraph::Y_GAP))/2;
	}

	return newGraphXY;
}

double HH_Neurons::GetStrongestABSWeight()
{
    double maxABSWeight, weight;

    for (uint32_t j=0; j<layers;j++)
        {
            for (uint32_t i=0; i<rowCountsForColumns[j];i++)
            {
                weight = neurons[j][i].GetStrongestABSWeight();

                if (weight > maxABSWeight)
                    maxABSWeight = weight;
            }
        }

    return maxABSWeight;
}

void HH_Neurons::Draw()
{
    glColor3f(1, 1, 1);

    sprintf(graphicsText, "Temperature %.12Lf C", neurons[0][0].Tk - ChemistryValues::KELVIN);
    GraphicsAndUI::DrawTextStr(graphicsText, 0, - 550, 0.0, 0.7);

    sprintf(graphicsText, "Most Recent Stimulus: %s", mostRecentStimulus);
    GraphicsAndUI::DrawTextStr(graphicsText, 0, -650, 0.0, 0.7);

    sprintf(graphicsText, "Fitness: %f", ((HH_NeuralNetwork *) neuralNetwork)->agent->fitness);
    GraphicsAndUI::DrawTextStr(graphicsText, 0, -850, 0.0, 0.7);

    double maxABSWeight = GetStrongestABSWeight();

    for (uint32_t j=0; j<layers;j++)
    {
        for (uint32_t i=0; i<rowCountsForColumns[j];i++)
        {
            neurons[j][i].Draw(maxABSWeight);
        }
    }
}

void HH_Neurons::ResetHH()
{
    for (uint32_t j=0; j<layers;j++)
    {
        for (uint32_t i=0; i<rowCountsForColumns[j];i++)
        {
            neurons[j][i].ResetHH();
        }
    }
}

uint32_t HH_Neurons::GetCurrentSpikeCount()
{
    uint32_t newCurrentSpikeCount = 0;

    for (uint32_t j=0; j<layers;j++)
    {
        for (uint32_t i=0; i<rowCountsForColumns[j];i++)
        {
            newCurrentSpikeCount += neurons[j][i].spike;
        }
    }

    return newCurrentSpikeCount;
}

void HH_Neurons::ResetCurrentSpikeCountAvg()
{
    totalCurrentSpikeCount = 0;
    spikeCountSamples = 0;
}

uint32_t HH_Neurons::GetCurrentSpikeCountForLayer(uint32_t index)
{
    uint32_t spikeCount = 0;

    for (uint32_t i=0; i<rowCountsForColumns[index];i++)
    {
        spikeCount += neurons[index][i].spike;
    }

    return spikeCount;
}

int32_t HH_Neurons::GetCurrentLeadingSpikeCount(void *nn)
{
    int32_t leadingSpikeCount = 0;

    HH_NeuralNetwork* nnPtr = (HH_NeuralNetwork *) nn;

    for (uint32_t j=0; j<layers;j++)
    {
        for (uint32_t i=0; i<rowCountsForColumns[j];i++)
        {
            leadingSpikeCount += (int32_t) neurons[j][i].spikeCount - nnPtr->neurons->neurons[j][i].spikeCount;
        }
    }

    return leadingSpikeCount;
}

uint32_t HH_Neurons::GetTotalSpikeCount()
{
    uint32_t totalSpikeCount = 0;

    for (uint32_t j=0; j<HH_NeuralNetwork::LAYERS;j++)
    {
        for (uint32_t i=0; i<rowCountsForColumns[j];i++)
        {
            totalSpikeCount += neurons[j][i].spikeCount;
        }
    }

    return totalSpikeCount;
}

uint32_t* HH_Neurons::GetSpikeCountsArrayForLayer(uint32_t layerIndex, uint32_t* spikeCountsArray)
{
    for (uint32_t i=0; i<rowCountsForColumns[layerIndex];i++)
    {
        spikeCountsArray[i] = neurons[layerIndex][i].spikeCount;
    }

    return spikeCountsArray;
}

void HH_Neurons::GetSpikeCountArraysForLayers(uint32_t **spikeCountsArrays)
{
    for (uint32_t j=0; j<HH_NeuralNetwork::LAYERS;j++)
    {
        for (uint32_t i=0; i<rowCountsForColumns[j];i++)
        {
            spikeCountsArrays[j][i] = neurons[j][i].spikeCount;
        }
    }
}

void HH_Neurons::ResetSpikeCounts()
{
    for (uint32_t j=0; j<layers;j++)
        {
            for (uint32_t i=0; i<rowCountsForColumns[j];i++)
            {
                neurons[j][i].ResetSpikeCounts();
            }
        }
}

void HH_Neurons::CopyResultsBuffers(ResultsBuffer_ptr* srcBuffers)
{
    for (uint32_t i=0; i<layers;i++)
    {
        resultsBuffers[i]->Copy(srcBuffers[i]);
    }
}

void HH_Neurons::ClearResultsBuffers()
{
    for (uint32_t j=0; j<layers;j++)
    {
        resultsBuffers[j]->Clear();
    }
}

HH_Neurons::~HH_Neurons()
{
     for (uint32_t i=0; i<layers; i++)
    {
        delete neurons[i];
        delete resultsBuffers[i];
    }

    delete [] neurons;
    delete [] resultsBuffers;
}
