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

#include <iostream>
//#include <cmath>
#include <tgmath.h>
#include <string.h>

#include <random>
#include <limits>
#include <thread>

#include "GL/glew.h"
#include "GL/glut.h"
#include "Agents.h"
#include "HH_NeuralNetworks.h"
#include "MembraneVoltageGraph.h"
#include "Graph.h"
#include "FoodForAgents.h"
#include "Simulation.h"
#include "Environment.h"
#include "GraphicsAndUI.h"
#include "MathUtilities.h"
#include "Compatibility.h"

char textBuffer[255];

void FreeMemory(void)
{
    for (uint32_t i = 0; i < Simulation::stimulusArrayLength; i++)
        delete Simulation::stimulusArray[i];
}

void GenerateNextGeneration()
{
    uint32_t *fittestNNIndexes = Simulation::neuralNetworks->GetFittestNNIndexes();

    // check if trained, evolved
    if (Simulation::neuralNetworks->neuralNetworks[fittestNNIndexes[0]]->agent->fitness >= 0) // if there aren't errors (negative values, fitness >= 0) then copy the nn's and test
    {
        Simulation::neuralNetworks->CopyWeights(fittestNNIndexes[0]); // generate new generation from the fittest agent
        Simulation::neuralNetworks->CopyResultsBuffers(fittestNNIndexes[0]);

        // save the evolved, trained fittest nn
        sprintf(textBuffer, "%s/%s/fittestNN%lu.txt", Simulation::saveRootFolder, Simulation::saveFittestNNFolder, Simulation::simStartRealTime);
        Simulation::neuralNetworks->neuralNetworks[fittestNNIndexes[0]]->Save(textBuffer);

        GraphicsAndUI::nnVisible = false;
        Simulation::training = false;

        if (Simulation::testingLayerErrors)
            Simulation::generationDuration = Simulation::generationDurationLayerTesting;
        else
            Simulation::generationDuration = Simulation::generationDurationFoodTesting;

        Simulation::neuralNetworks->ResetSpikeCounts();

        GraphicsAndUI::SetNNsSameXYPosAndSetRedVoltages(); // set the nns to overlay each other and the temperature affected nns' membrane voltage graphs to red
    }
    else // if there are errors copy the best and mutate
    {
        Simulation::neuralNetworks->CopyWeights(fittestNNIndexes); // generate from fittest two
        Simulation::neuralNetworks->Mutate();
    }

    // reset the Simulation::agents' pos, fitness and other variables for the new generation
    Simulation::agents->Reset();
    Environment::foodForAgents_ptr->GenerateAndSaveFoodPosForAgents(); // regenerate the food
}

#ifdef _WIN32
void SimThreadFunction(void *param)
#elif __linux__
void *SimThreadFunction(void *param)
#endif
{
    Simulation::randomGenerator.seed(Simulation::randomSeed); // seed the random number generator for the thread
    srand(Simulation::randomSeed);

    uint32_t setCount = 0;

    GraphicsAndUI::SetNNsSameXYPosAndSetRedVoltages(); // set the NNs to overlay each other and the temperature affected NNs' membrane voltage graphs to red

    Simulation::SetCurrentRFStimulusDurationAndInterval();

    uint32_t rfTemperaturePulseCount = 0;

    if (Simulation::training == false)
    {
        if (Simulation::testingLayerErrors)
            Simulation::generationDuration = Simulation::generationDurationLayerTesting;
        else
            Simulation::generationDuration = Simulation::generationDurationFoodTesting;
    }

    while (Simulation::processing)
    {
        // save the membrane voltages trace at saveMembraneVoltagesTraceGraphTime
        if (Simulation::generationTime >= Simulation::saveMembraneVoltagesTraceGraphTime && Simulation::saveMembraneVoltagesTraceGraphTime != 0)
        {
            Simulation::agents->saveMembraneVoltagesTraceGraph();

            Simulation::saveMembraneVoltagesTraceGraphTime = 0;
        }

        if (!GraphicsAndUI::paused)
        {
            uint32_t neuronsCount = Simulation::neuralNetworks->neuralNetworks[0]->neurons->count;

            for (uint32_t i = 0; i < neuronsCount; i++)
            {
                HH_NeuralNetworks::NEURAL_NOISE[i] = (double)rand() / RAND_MAX * Simulation::MAX_NEURAL_NOISE - Simulation::MAX_NEURAL_NOISE / 2;
            }

            Simulation::generationTime = Simulation::agents->agents[0]->generationTime;

            if (!Simulation::training) // if testing
            {
                // if new RF stimulus
                if (Simulation::generationTime > Simulation::startRFStimulusTime + Simulation::currentRFStimulusDuration + Simulation::currentRFStimulusInterval)
                {
                    rfTemperaturePulseCount++;

                    if (Simulation::averagingPulseSequencesResults && rfTemperaturePulseCount >= Simulation::maxRFTemperaturePulseBeforeReset)
                    {
                        Simulation::rfTemperaturePulseSequenceCount++;

                        if (Simulation::rfTemperaturePulseSequenceCount <= Simulation::maxRFTemperaturePulseSequenceTrials || (Simulation::rfTemperaturePulseSequenceCount % 10) == 0) // save graph data if less than the required testing trials or every 10 after
                        {
                            Simulation::resultsAndGraphs->SaveGraphs();
                        }

                        Simulation::resultsAndGraphs->rfTargetTemperatureGraph->ResetIndexes();
                        Simulation::resultsAndGraphs->rfCurrentTemperatureGraph->ResetIndexes();
                        Simulation::neuralNetworks->ResetHH();
                        Simulation::neuralNetworks->ResetSpikeCounts();

                        Simulation::nextStimulusIndex = 0;
                        rfTemperaturePulseCount = 0;
                    }

                    // get a new target temperature duration and intervals for the Simulation::agents
                    // this is chosen randomly so that the times vary
                    Simulation::SetCurrentRFStimulusDurationAndInterval();

                    Simulation::startRFStimulusTime = Simulation::generationTime;
                }

                if (Simulation::generationTime - Simulation::startRFStimulusTime < Simulation::currentRFStimulusDuration) // stimulating with RF induced temperatures
                {
                    Simulation::agents->SetTargetTemperature(Simulation::referenceTemperature, 0); // set the temperature for the reference agent

                    // set the temperatures for the other Simulation::agents
                    double rfInducedTemperature;
                    for (uint32_t agentIndex = 1; agentIndex < Simulation::agents->count; agentIndex++)
                    {
                        // get the specified temperature for each agent
                        rfInducedTemperature = Simulation::rfInducedTemperaturesFromPulsesForAgents[agentIndex - 1];

                        Simulation::agents->SetTargetTemperature(rfInducedTemperature, agentIndex);
                    }
                }
                else // not RF stimulating, all Simulation::agents set to reference temperature as target
                {
                    Simulation::agents->SetTargetTemperature(Simulation::referenceTemperature);
                }
            }

            if (Simulation::generationTime == 0 && Simulation::totalSimTime > 1) // generationTime has been set to 0 so start a new generation
            {
                Simulation::startGenerationTime = Simulation::generationTime;

                if (Simulation::training) // if training, evolving
                {
                    GenerateNextGeneration(); // check fitness, if errors, mutate and create new generation, otherwise create new generation from fittest agent, end training and start testing

                    Simulation::agents->ResetTraces();

                    Simulation::resultsAndGraphs->ResetTotalVoltageDifferencesForLayerAndTotalErrorPercentageForLayerResultingFromRFTemperatureIncreases();
                }
                else
                {
                    if (Simulation::nnAvgsTesting)
                    {
                        Simulation::trainingGeneration++;

                        if (Simulation::resetTraining)
                            Simulation::training = true; // for experiments regenerating NN to get averages for layer testing

                        Simulation::generationDuration = Simulation::generationDurationEvolvingTraining;

                        Simulation::agents->RegenerateNNConnections();

                        Simulation::resultsAndGraphs->ResetTotalVoltageDifferencesForLayerAndTotalErrorPercentageForLayerResultingFromRFTemperatureIncreases();
                    }

                    Environment::foodForAgents_ptr->GenerateAndSaveFoodPosForAgents();
                    Environment::foodForAgents_ptr->SetFoodColor(Food::FoodDefaultColor);

                    Simulation::agents->ResetTraces();
                }

                Simulation::agents->Reset();

                // starting a new generation so also reset the start of the RF stimulus time
                Simulation::startRFStimulusTime = 0;

                Simulation::generations++;
            }

            // process the stimulus of the Simulation::agents
            if (Simulation::agents->Process() > 0) // Process() returns the number of Simulation::agents that have processed the result of the stimulus
            {                                      // so if > 0 then save the data for the result graphs
                if (!Simulation::training)         // if testing
                {
                    Simulation::resultsAndGraphs->DetermineLayerPercentageDifferences();
                    Simulation::resultsAndGraphs->GraphAvgLayerPercentageDifferences();

                    if (Simulation::trainingGeneration < Simulation::maxTestingGenerations || ((Simulation::trainingGeneration + 1) % 10) == 0) // save graph data if less than the required testing generations or every 10 after
                    {                                                                                                                           // Simulation::trainingGeneration + 1 because starts at 0 and saves the graphs after every processed result during training generation
                        if (Simulation::resultsAndGraphs->voltageDifferencesSamples > 0)
                        {
                            Simulation::resultsAndGraphs->SaveGraphs();
                        }
                    }
                }
            }

            if (!Simulation::training) // if testing graph results
            {
                Simulation::resultsAndGraphs->DetermineElectromagneticInducedTotalVoltageDifferences(); // determine the effects of the RF induced temperature increases on the membrane voltages

                Simulation::resultsAndGraphs->GraphAvgRFMembraneVoltageDifferences();
            }

            // graph the current spike count
            Simulation::resultsAndGraphs->GraphCurrentExcitoryRFSpikeCount();

            // graph the current RF temperature
            Simulation::resultsAndGraphs->GraphCurrentRFTemperature();

            Simulation::deltas = Simulation::generationTime / Simulation::deltaTime;

            // increment the simulation time
            Simulation::totalSimTime += Simulation::deltaTime;
        }
    }
}

int main(int argc, char *argv[])
{
    //char cwd[1024];
    //getcwd(cwd, sizeof(cwd));

    atexit(FreeMemory);

    glutInit(&argc, argv);
    GraphicsAndUI::InitGL();

    Simulation::Initialize(argv[1]);

    Compatibility::LaunchThread(SimThreadFunction, NULL);
    Compatibility::Delay(1000);

    glutMainLoop();

    getchar();
}
