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

#include "ResultsAndGraphs.h"
#include "Simulation.h"

ResultsAndGraphs::ResultsAndGraphs()
{

}

void ResultsAndGraphs::InitializeGraphs()
{
    Vector graphPos(10000, -30000, 0);

    graphPos.y -= 12000;
    errorLayerPercentageGraph = new Graph(graphPos, 100);
    errorLayerPercentageGraph->drawAverage = true;

    graphPos.y -= 12000;
    increasedVoltageErrorGraph = new Graph(graphPos, 100);
    increasedVoltageErrorGraph->drawAverage = true;

    increasedVoltageErrorGraph->yLabelInc = 1;
    increasedVoltageErrorGraph->yScale = 100;

    graphPos.y -= 12000;
    decreasedVoltageErrorGraph = new Graph(graphPos, 100);
    decreasedVoltageErrorGraph->drawAverage = true;

    decreasedVoltageErrorGraph->yLabelInc = 1;
    decreasedVoltageErrorGraph->yScale = 100;

    double yScale = 100;
    double xScale = (double) Simulation::graphDataPointIntervalDeltas / 10;

    graphPos.y -= 12000;
    accruedErrorPercentageGraph = new Graph(graphPos);
    accruedErrorPercentageGraph->drawAverage = true;
    accruedErrorPercentageGraph->drawValues = false;
    accruedErrorPercentageGraph->drawxLabels = false;
    accruedErrorPercentageGraph->drawRollingAverage = true;
    accruedErrorPercentageGraph->SetMaxSeriesColor(1.0, 0, 0);


    accruedErrorPercentageGraph->yScale = yScale/10;
    accruedErrorPercentageGraph->xScale = xScale;

    graphPos.y -= 12000;
    double rfTargetTemperatureGraphPosY = graphPos.y;

    rfTemperatureLessNormalTemperatureCurrentSpikeCountGraph = new Graph(graphPos);
    rfTemperatureLessNormalTemperatureCurrentSpikeCountGraph->drawValues = false;
    rfTemperatureLessNormalTemperatureCurrentSpikeCountGraph->drawAverage = true;
    rfTemperatureLessNormalTemperatureCurrentSpikeCountGraph->drawRollingAverage = true;
    rfTemperatureLessNormalTemperatureCurrentSpikeCountGraph->drawxLabels = false;
    rfTemperatureLessNormalTemperatureCurrentSpikeCountGraph->SetMaxSeriesColor(1.0, 0, 0);
    rfTemperatureLessNormalTemperatureCurrentSpikeCountGraph->SetAvgSeriesColor(1.0, 0, 0);
    rfTemperatureLessNormalTemperatureCurrentSpikeCountGraph->SetRollingAvgSeriesColor(1.0, 0, 0);

    rfTemperatureLessNormalTemperatureCurrentSpikeCountGraph->yScale = yScale;

    rfTemperatureLessNormalTemperatureCurrentSpikeCountGraph->xScale = xScale;

    graphPos.y -= 28000;
    rfTemperatureCurrentSpikeCountGraph = new Graph(graphPos);
    rfTemperatureCurrentSpikeCountGraph->drawValues = false;
    rfTemperatureCurrentSpikeCountGraph->drawAverage = true;
    rfTemperatureCurrentSpikeCountGraph->drawRollingAverage = true;
    rfTemperatureCurrentSpikeCountGraph->drawxLabels = false;
    rfTemperatureCurrentSpikeCountGraph->SetMaxSeriesColor(1.0, 0, 0);
    rfTemperatureCurrentSpikeCountGraph->SetAvgSeriesColor(1.0, 0, 0);
    rfTemperatureCurrentSpikeCountGraph->SetRollingAvgSeriesColor(1.0, 0, 0);

    rfTemperatureCurrentSpikeCountGraph->yScale = yScale;

    rfTemperatureCurrentSpikeCountGraph->xScale = xScale;

    normalTemperatureCurrentSpikeCountGraph = new Graph(graphPos);
    normalTemperatureCurrentSpikeCountGraph->drawValues = false;
    normalTemperatureCurrentSpikeCountGraph->drawAverage = true;
    normalTemperatureCurrentSpikeCountGraph->drawRollingAverage = true;
    normalTemperatureCurrentSpikeCountGraph->drawxLabels = false;
    normalTemperatureCurrentSpikeCountGraph->SetMaxSeriesColor(1.0, 0, 0);

    normalTemperatureCurrentSpikeCountGraph->yScale = yScale;
    normalTemperatureCurrentSpikeCountGraph->xScale = xScale;

    rfTargetTemperatureGraph = new Graph(graphPos);
    rfTargetTemperatureGraph->drawxLabels = false;
    rfTargetTemperatureGraph->SetMaxSeriesColor(1.0, 0, 0);
    rfTargetTemperatureGraph->SetSeriesColors(0, 1, 0, 0);
    rfTargetTemperatureGraph->yScale = yScale * Graph::yTemperatureScale;
    rfTargetTemperatureGraph->xScale = xScale;

    rfCurrentTemperatureGraph = new Graph(graphPos);
    rfCurrentTemperatureGraph->drawxLabels = false;
    rfCurrentTemperatureGraph->SetMaxSeriesColor(1.0, 1.0, 0);
    rfCurrentTemperatureGraph->SetSeriesColors(0, 1, 1, 0);
    rfCurrentTemperatureGraph->yScale = yScale * Graph::yTemperatureScale;
    rfCurrentTemperatureGraph->xScale = xScale;


    sprintf(errorLayerPercentageGraph->graphTitle, "Error (difference) Percentages for each layer, %i trials, RF Temperature Start %.16Lf\n", Simulation::trainingGeneration + 1, Simulation::rfInducedTemperaturesFromPulsesForAgents[0]);
    sprintf(increasedVoltageErrorGraph->graphTitle, "Avg Excitory Voltage Change Per Neuron, %i trials, RF Temperature Start %.16Lf\n", Simulation::trainingGeneration + 1, Simulation::rfInducedTemperaturesFromPulsesForAgents[0]);
    sprintf(decreasedVoltageErrorGraph->graphTitle, "Avg Inhibitory Voltage Change Per Neuron, %i trials, RF Temperature Start %.16Lf\n", Simulation::trainingGeneration + 1, Simulation::rfInducedTemperaturesFromPulsesForAgents[0]);
    sprintf(accruedErrorPercentageGraph->graphTitle, "Error (difference) percentage over time for the final layer of the NNs, %i trials, RF Temperature Start %.16Lf\n", Simulation::rfTemperaturePulseSequenceCount, Simulation::rfInducedTemperaturesFromPulsesForAgents[0]);
    sprintf(rfTemperatureLessNormalTemperatureCurrentSpikeCountGraph->graphTitle, "RF Temperature affected NN - Reference Normal Temperature NN Current Spike Counts over time, %i trials, RF Temperature Start %.16Lf\n", Simulation::rfTemperaturePulseSequenceCount, Simulation::rfInducedTemperaturesFromPulsesForAgents[0]);
    sprintf(rfTemperatureCurrentSpikeCountGraph->graphTitle, "Current Spike Counts over time for the RF temperature affected NN, %i trials, RF Temperature Start %.16Lf\n", Simulation::rfTemperaturePulseSequenceCount, Simulation::rfInducedTemperaturesFromPulsesForAgents[0]);
    sprintf(normalTemperatureCurrentSpikeCountGraph->graphTitle, "Current Spike Counts over time for the normal temperature NN, %i trials, RF Temperature Start %.16Lf\n", Simulation::rfTemperaturePulseSequenceCount, Simulation::rfInducedTemperaturesFromPulsesForAgents[0]);
    sprintf(rfTargetTemperatureGraph->graphTitle, "Target Temperature over time for the RF temperature affected NN, %i trials, RF Temperature Start %.16Lf\n", Simulation::rfTemperaturePulseSequenceCount, Simulation::rfInducedTemperaturesFromPulsesForAgents[0]);
    sprintf(rfCurrentTemperatureGraph->graphTitle, "Current Temperature over time for the RF temperature affected NN, %i trials, RF Temperature Start %.16Lf\n", Simulation::rfTemperaturePulseSequenceCount, Simulation::rfInducedTemperaturesFromPulsesForAgents[0]);


    errorLayerPercentageGraph->SetGraphFileName(Simulation::saveRootFolder, Simulation::saveResultGraphsFolder, "errorLayerPercentageGraph", Simulation::simStartRealTime);
    accruedErrorPercentageGraph->SetGraphFileName(Simulation::saveRootFolder, Simulation::saveResultGraphsFolder, "accruedErrorPercentageGraph", Simulation::simStartRealTime);
    increasedVoltageErrorGraph->SetGraphFileName(Simulation::saveRootFolder, Simulation::saveResultGraphsFolder, "increasedVoltageErrorGraph", Simulation::simStartRealTime);
    decreasedVoltageErrorGraph->SetGraphFileName(Simulation::saveRootFolder, Simulation::saveResultGraphsFolder, "decreasedVoltageErrorGraph", Simulation::simStartRealTime);
    rfTemperatureLessNormalTemperatureCurrentSpikeCountGraph->SetGraphFileName(Simulation::saveRootFolder, Simulation::saveResultGraphsFolder, "rfTemperatureLessNormalTemperatureCurrentSpikeCountGraph", Simulation::simStartRealTime);
    rfTemperatureCurrentSpikeCountGraph->SetGraphFileName(Simulation::saveRootFolder, Simulation::saveResultGraphsFolder, "rfTemperatureCurrentSpikeCountGraph", Simulation::simStartRealTime);
    normalTemperatureCurrentSpikeCountGraph->SetGraphFileName(Simulation::saveRootFolder, Simulation::saveResultGraphsFolder, "normalTemperatureCurrentSpikeCountGraph", Simulation::simStartRealTime);
    rfTargetTemperatureGraph->SetGraphFileName(Simulation::saveRootFolder, Simulation::saveResultGraphsFolder, "rfTargetTemperatureGraph", Simulation::simStartRealTime);
    rfCurrentTemperatureGraph->SetGraphFileName(Simulation::saveRootFolder, Simulation::saveResultGraphsFolder, "rfCurrentTemperatureGraph", Simulation::simStartRealTime);


    electromagneticErrorsForLayerTotal = new double_ptr[Simulation::agents->count];
    electromagneticErrorPercentageForLayerTotal = new double_ptr[Simulation::agents->count];
    currentErrorForLayerPercentage = new double_ptr[Simulation::agents->count];

    totalIncreasedVoltageDifferencesForLayer = new double_ptr[Simulation::agents->count];
    totalDecreasedVoltageDifferencesForLayer = new double_ptr[Simulation::agents->count];

    currentIncreasedVoltageDifference= new double[Simulation::agents->count];
    totalIncreasedVoltageDifference= new double[Simulation::agents->count];
    totalDecreasedVoltageDifference = new double[Simulation::agents->count];
    totalVoltageDifference = new double[Simulation::agents->count];


    for (int32_t agentIndex = 0; agentIndex < Simulation::agents->count; agentIndex++)
    {
        electromagneticErrorsForLayerTotal[agentIndex] = new double[HH_NeuralNetwork::LAYERS];
        electromagneticErrorPercentageForLayerTotal[agentIndex] = new double[HH_NeuralNetwork::LAYERS];
        currentErrorForLayerPercentage[agentIndex] = new double[HH_NeuralNetwork::LAYERS];

        totalIncreasedVoltageDifferencesForLayer[agentIndex] = new double[HH_NeuralNetwork::LAYERS];
        totalDecreasedVoltageDifferencesForLayer[agentIndex] = new double[HH_NeuralNetwork::LAYERS];

        for (int32_t i = 0; i < HH_NeuralNetwork::LAYERS; i++)
        {
            electromagneticErrorsForLayerTotal[agentIndex][i] = 0;
            electromagneticErrorPercentageForLayerTotal[agentIndex][i] = 0;
            currentErrorForLayerPercentage[agentIndex][i] = 0;
        }
    }
}

void ResultsAndGraphs::SaveGraphs()
{
    sprintf(errorLayerPercentageGraph->graphTitle, "Error (difference) Percentages for each layer, %i trials, RF Temperature Start %.16Lf\n", Simulation::trainingGeneration + 1, Simulation::rfInducedTemperaturesFromPulsesForAgents[0]);
    sprintf(increasedVoltageErrorGraph->graphTitle, "Avg Excitory Voltage Change Per Neuron, %i trials, RF Temperature Start %.16Lf\n", Simulation::trainingGeneration + 1, Simulation::rfInducedTemperaturesFromPulsesForAgents[0]);
    sprintf(decreasedVoltageErrorGraph->graphTitle, "Avg Inhibitory Voltage Change Per Neuron, %i trials, RF Temperature Start %.16Lf\n", Simulation::trainingGeneration + 1, Simulation::rfInducedTemperaturesFromPulsesForAgents[0]);
    sprintf(accruedErrorPercentageGraph->graphTitle, "Error (difference) percentage over time for the final layer of the NNs, %i trials, RF Temperature Start %.16Lf\n", Simulation::rfTemperaturePulseSequenceCount, Simulation::rfInducedTemperaturesFromPulsesForAgents[0]);
    sprintf(rfTemperatureLessNormalTemperatureCurrentSpikeCountGraph->graphTitle, "RF Temperature affected NN - Reference Normal Temperature NN Current Spike Counts over time, %i trials, RF Temperature Start %.16Lf\n", Simulation::rfTemperaturePulseSequenceCount, Simulation::rfInducedTemperaturesFromPulsesForAgents[0]);
    sprintf(rfTemperatureCurrentSpikeCountGraph->graphTitle, "Current Spike Counts over time for the RF temperature affected NN, %i trials, RF Temperature Start %.16Lf\n", Simulation::rfTemperaturePulseSequenceCount, Simulation::rfInducedTemperaturesFromPulsesForAgents[0]);
    sprintf(normalTemperatureCurrentSpikeCountGraph->graphTitle, "Current Spike Counts over time for the normal temperature NN, %i trials, RF Temperature Start %.16Lf\n", Simulation::rfTemperaturePulseSequenceCount, Simulation::rfInducedTemperaturesFromPulsesForAgents[0]);
    sprintf(rfTargetTemperatureGraph->graphTitle, "Target Temperature over time for the RF temperature affected NN, %i trials, RF Temperature Start %.16Lf\n", Simulation::rfTemperaturePulseSequenceCount, Simulation::rfInducedTemperaturesFromPulsesForAgents[0]);
    sprintf(rfCurrentTemperatureGraph->graphTitle, "Current Temperature over time for the RF temperature affected NN, %i trials, RF Temperature Start %.16Lf\n", Simulation::rfTemperaturePulseSequenceCount, Simulation::rfInducedTemperaturesFromPulsesForAgents[0]);

    errorLayerPercentageGraph->Save("");
    increasedVoltageErrorGraph->Save("");
    decreasedVoltageErrorGraph->Save("");
    accruedErrorPercentageGraph->Save("", 1);
    rfTemperatureLessNormalTemperatureCurrentSpikeCountGraph->Save("", 1);
    rfTemperatureCurrentSpikeCountGraph->Save("", 1);
    normalTemperatureCurrentSpikeCountGraph->Save("", 1);
    rfTargetTemperatureGraph->Save("", 1);
    rfCurrentTemperatureGraph->Save("", 1);
}

void ResultsAndGraphs::Draw()
{
    errorLayerPercentageGraph->Draw();

    increasedVoltageErrorGraph->Draw();
    decreasedVoltageErrorGraph->Draw();

    accruedErrorPercentageGraph->Draw();

    rfTemperatureLessNormalTemperatureCurrentSpikeCountGraph->Draw();
    rfTemperatureCurrentSpikeCountGraph->Draw();
    normalTemperatureCurrentSpikeCountGraph->Draw();
    rfTargetTemperatureGraph->Draw();
    rfCurrentTemperatureGraph->Draw();
}

void ResultsAndGraphs::ResetTotalVoltageDifferencesForLayerAndTotalErrorPercentageForLayerResultingFromRFTemperatureIncreases()
{
    for (uint32_t agentIndex=1; agentIndex<Simulation::agents->count; agentIndex++)
    {
        for (uint32_t j=0; j<HH_NeuralNetwork::LAYERS;j++)
        {
            totalIncreasedVoltageDifferencesForLayer[agentIndex][j] = 0;
            totalDecreasedVoltageDifferencesForLayer[agentIndex][j] = 0;

            electromagneticErrorsForLayerTotal[agentIndex][j] = 0;
            electromagneticErrorPercentageForLayerTotal[agentIndex][j] = 0;
            currentErrorForLayerPercentage[agentIndex][j] = 0;
        }

        currentIncreasedVoltageDifference[agentIndex] = 0;
        totalIncreasedVoltageDifference[agentIndex] = 0;
        totalDecreasedVoltageDifference[agentIndex] = 0;
        totalVoltageDifference[agentIndex] = 0;
    }

    voltageDifferencesSamples = 0;
    electromagneticErrorForLayerAdditionCount = 0;
}

void ResultsAndGraphs::DetermineElectromagneticInducedTotalVoltageDifferences()
{
    long double agent0NeuronVoltage, agentRFNeuronVoltage, voltageDifference;

    for (uint32_t agentIndex=1; agentIndex<Simulation::agents->count; agentIndex++)
    {
        currentIncreasedVoltageDifference[agentIndex] = 0;

        for (uint32_t j=0; j<HH_NeuralNetwork::LAYERS;j++)
        {
            for (uint32_t i=0; i<((HH_NeuralNetwork*) Simulation::agents->agents[0]->neuralNetwork)->neurons->rowCountsForColumns[j];i++)
            {
                agent0NeuronVoltage = ((HH_NeuralNetwork*) Simulation::agents->agents[0]->neuralNetwork)->neurons->neurons[j][i].membraneVoltage;
                agentRFNeuronVoltage = ((HH_NeuralNetwork*) Simulation::agents->agents[agentIndex]->neuralNetwork)->neurons->neurons[j][i].membraneVoltage;

                voltageDifference =  agentRFNeuronVoltage - agent0NeuronVoltage;

                totalVoltageDifference[agentIndex] += voltageDifference;

                if (voltageDifference>0)
                {
                    totalIncreasedVoltageDifferencesForLayer[agentIndex][j] += voltageDifference;

                    totalIncreasedVoltageDifference[agentIndex] += voltageDifference;

                    currentIncreasedVoltageDifference[agentIndex] += voltageDifference;
                }
                else
                {
                    totalDecreasedVoltageDifferencesForLayer[agentIndex][j] += -voltageDifference;

                    totalDecreasedVoltageDifference[agentIndex] += -voltageDifference;
                }
            }
        }
    }

    voltageDifferencesSamples++;
}

void ResultsAndGraphs::GraphCurrentExcitoryRFSpikeCount()
{
    int32_t index = -1;

    if (!Simulation::averagingPulseSequencesResults) //not averaging data for each pulse so scroll
            index = Graph::MAX_INDEX - 1;

    int32_t value;

    //get and add the current spike count for the normal temperature graph
    int32_t referenceTemperatureCurrentSpikeCount = ((HH_NeuralNetwork*) Simulation::agents->agents[0]->neuralNetwork)->neurons->GetCurrentSpikeCount();
    normalTemperatureCurrentSpikeCountGraph->AddPoint(referenceTemperatureCurrentSpikeCount, Simulation::rfTemperaturePulseSequenceCount, index);

    for (uint32_t agentIndex=1; agentIndex<Simulation::agents->count; agentIndex++)
    {
        //get and add the current spike count for the RF temperature graph
        value = ((HH_NeuralNetwork*) Simulation::agents->agents[agentIndex]->neuralNetwork)->neurons->GetCurrentSpikeCount();
        rfTemperatureCurrentSpikeCountGraph->AddPoint(value, Simulation::rfTemperaturePulseSequenceCount, index);

        //get the difference of the RF temperature - normal temperature current spike count and add to the difference graph
        rfTemperatureLessNormalTemperatureCurrentSpikeCountGraph->AddPoint(value - referenceTemperatureCurrentSpikeCount, Simulation::rfTemperaturePulseSequenceCount, index);
    }

    if (!Simulation::averagingPulseSequencesResults) //not averaging data for each pulse so scroll
    {
        normalTemperatureCurrentSpikeCountGraph->Scroll();
        rfTemperatureCurrentSpikeCountGraph->Scroll();
        rfTemperatureLessNormalTemperatureCurrentSpikeCountGraph->Scroll();
    }
}

void ResultsAndGraphs::GraphCurrentRFTemperature()
{
    int32_t index = -1;

    if (!Simulation::averagingPulseSequencesResults) //not averaging data for each pulse so scroll
            index = Graph::MAX_INDEX - 1;

    for (uint32_t agentIndex=1; agentIndex<Simulation::agents->count; agentIndex++)
    {
        rfTargetTemperatureGraph->AddPoint(((HH_NeuralNetwork*) Simulation::agents->agents[agentIndex]->neuralNetwork)->neurons->neurons[0][0].targetTk - ChemistryValues::KELVIN - Simulation::referenceTemperature, agentIndex - 1, index);
        rfCurrentTemperatureGraph->AddPoint(((HH_NeuralNetwork*) Simulation::agents->agents[agentIndex]->neuralNetwork)->neurons->neurons[0][0].Tk - ChemistryValues::KELVIN - Simulation::referenceTemperature, agentIndex - 1, index);
    }

    if (!Simulation::averagingPulseSequencesResults) //not averaging data for each pulse so scroll
    {
        rfTargetTemperatureGraph->Scroll();
        rfCurrentTemperatureGraph->Scroll();
    }
}

void ResultsAndGraphs::GraphAvgRFMembraneVoltageDifferences()
{
    Graph* graph1 = increasedVoltageErrorGraph;
    Graph* graph2 = decreasedVoltageErrorGraph;

    for (uint32_t agentIndex=1; agentIndex<Simulation::agents->count; agentIndex++)
    {
        double voltageErrorForLayer;

        for (uint32_t j=0; j<HH_NeuralNetwork::LAYERS;j++)
        {
            graph1->AddPoint(totalIncreasedVoltageDifferencesForLayer[agentIndex][j]/voltageDifferencesSamples/((HH_NeuralNetwork*) Simulation::agents->agents[0]->neuralNetwork)->neurons->rowCountsForColumns[j], Simulation::trainingGeneration, j);
            graph2->AddPoint(totalDecreasedVoltageDifferencesForLayer[agentIndex][j]/voltageDifferencesSamples/((HH_NeuralNetwork*) Simulation::agents->agents[0]->neuralNetwork)->neurons->rowCountsForColumns[j], Simulation::trainingGeneration, j);
        }
    }
}

double ResultsAndGraphs::DetermineError(char* result1, char* result2, uint32_t length)
{
    uint32_t error;
    uint32_t layerErrorCount=0;

    int32_t a, b;

    char tempA, tempB;

    for (uint32_t i=0; i<length; i++)
    {
        tempA = result1[i+1];
        tempB = result2[i+1];
        result1[i+1] = 0;
        result2[i+1] = 0;

        a = atoi(&result1[i]);
        b = atoi(&result2[i]);

        result1[i+1] = tempA;
        result2[i+1] = tempB;

        error = abs(a - b);

        if (error != 0)
            layerErrorCount++;
    }

    return (double) layerErrorCount/length * 100;
}

void ResultsAndGraphs::DetermineLayerPercentageDifferences()
{
    int32_t electromagneticErrorsForLayer[Simulation::agents->count][HH_NeuralNetwork::LAYERS];
    uint32_t totalSpikeCountForLayerAgent1;
    uint32_t totalSpikeCountForLayerAgent2;
    double error;

    electromagneticErrorForLayerAdditionCount++;

    char result1[255];
    char result2[255];

    memset(result1, 0, 255);
    memset(result2, 0, 255);

    for (uint32_t agentIndex=1; agentIndex<Simulation::agents->count; agentIndex++)
    {
        for (uint32_t j=0; j<HH_NeuralNetwork::LAYERS;j++)
        {
            electromagneticErrorsForLayer[agentIndex][j] = 0;

            uint32_t layerLength = ((HH_NeuralNetwork*) Simulation::agents->agents[0]->neuralNetwork)->neurons->rowCountsForColumns[j];
            for (uint32_t i=0; i<layerLength;i++)
            {
                sprintf(&result1[i], "%i", Simulation::agents->agents[0]->spikeCountsArrays[j][i]);
                sprintf(&result2[i], "%i", Simulation::agents->agents[agentIndex]->spikeCountsArrays[j][i]);
            }

            currentErrorForLayerPercentage[agentIndex][j] = DetermineError(result1, result2, layerLength);

            if (currentErrorForLayerPercentage[agentIndex][j]>0)
                electromagneticErrorPercentageForLayerTotal[agentIndex][j] += currentErrorForLayerPercentage[agentIndex][j];
        }
    }
}

void ResultsAndGraphs::GraphAvgLayerPercentageDifferences()
{
    int32_t index = -1;

    if (!Simulation::averagingPulseSequencesResults) //not averaging data for each pulse so scroll
            index = Graph::MAX_INDEX - 1;

    for (uint32_t agentIndex=1; agentIndex<Simulation::agents->count; agentIndex++)
    {
        for (uint32_t j=0; j<HH_NeuralNetwork::LAYERS;j++)
        {
            errorLayerPercentageGraph->AddPoint(electromagneticErrorPercentageForLayerTotal[agentIndex][j]/electromagneticErrorForLayerAdditionCount, Simulation::trainingGeneration, j);

            if (j == HH_NeuralNetwork::LAYERS - 1)
                accruedErrorPercentageGraph->AddPoint(currentErrorForLayerPercentage[agentIndex][j], Simulation::rfTemperaturePulseSequenceCount, index);
        }
    }
}
