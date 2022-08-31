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

#include <iostream>
#include <cmath>
#include <process.h>
#include <tgmath.h>
#include <string.h>

#include <random>
#include <io.h>
#include <limits>

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


char textBuffer[255];

void FreeMemory(void)
{
    for (uint32_t i=0; i < Simulation::stimulusArrayLength; i++)
        delete Simulation::stimulusArray[i];
}

void GenerateNextGeneration()
{
    uint32_t* fittestNNIndexes = Simulation::neuralNetworks->GetFittestNNIndexes();

    //check if trained, evolved
    if (Simulation::neuralNetworks->neuralNetworks[fittestNNIndexes[0]]->agent->fitness >= 0) //if there aren't errors (negative values, fitness >= 0) then copy the nn's and test
    {
        Simulation::neuralNetworks->CopyWeights(fittestNNIndexes[0]); //generate new generation from the fittest agent
        Simulation::neuralNetworks->CopyResultsBuffers(fittestNNIndexes[0]);

        //save the evolved, trained fittest nn
        sprintf(textBuffer, "%s/%s/fittestNN%lu.txt", Simulation::saveRootFolder, Simulation::saveFittestNNFolder, Simulation::simStartRealTime);
        Simulation::neuralNetworks->neuralNetworks[fittestNNIndexes[0]]->Save(textBuffer);

        GraphicsAndUI::nnVisible = false;
        ////Sleep(1000);

        Simulation::training = false;

        if (Simulation::testingLayerErrors)
            Simulation::generationDuration = Simulation::generationDurationLayerTesting;
        else
            Simulation::generationDuration = Simulation::generationDurationFoodTesting;

        Simulation::neuralNetworks->ResetSpikeCounts();

        GraphicsAndUI::SetNNsSameXYPosAndSetRedVoltages(); //set the nns to overlay each other and the temperature affected nns' membrane voltage graphs to red
    }
    else //if there are errors copy the best and mutate
    {
        Simulation::neuralNetworks->CopyWeights(fittestNNIndexes); //generate from fittest two
        Simulation::neuralNetworks->Mutate();

    }

    //reset the Simulation::agents' pos, fitness and other variables for the new generation
    Simulation::agents->Reset();
    Environment::foodForAgents_ptr->GenerateAndSaveFoodPos();//regenerate the food
}

void SimThreadFunction(void *param)
{
    uint32_t setCount = 0;

    GraphicsAndUI::SetNNsSameXYPosAndSetRedVoltages(); //set the nns to overlay each other and the temperature affected nns' membrane voltage graphs to red

    Simulation::SetCurrentRFStimulusDurationAndInterval();

    uint32_t rfTemperaturePulseCount = 0;

    ////if (rfTemperaturePulseCount >= Simulation::maxRFTemperaturePulseBeforeReset-1)
                        ////Simulation::currentRFStimulusInterval *= 10;

    if (Simulation::training == false)
    {
        if (Simulation::testingLayerErrors)
            Simulation::generationDuration = Simulation::generationDurationLayerTesting;
        else
            Simulation::generationDuration = Simulation::generationDurationFoodTesting;
    }

    while (Simulation::processing)
    {
        //save the membrane voltages trace at saveMembraneVoltagesTraceGraphTime
        if (Simulation::generationTime >= Simulation::saveMembraneVoltagesTraceGraphTime && Simulation::saveMembraneVoltagesTraceGraphTime != 0)
        {
            Simulation::agents->saveMembraneVoltagesTraceGraph();

            Simulation::saveMembraneVoltagesTraceGraphTime = 0;
        }

        /*////
        if (Simulation::stimulus)
        {
            Simulation::neuralNetworks->SetSynStimulus(Simulation::stimulusValue, 0, 0);
        }
        else
        {
            Simulation::neuralNetworks->SetSynStimulus(0.0);
        }

        if (incStimulus)
        {
            Simulation::stimulusValue += Simulation::stimulusValueInc;

            incStimulus = false;
        }
        else
            if (decStimulus)
            {
                Simulation::stimulusValue -= Simulation::stimulusValueInc;

                decStimulus = false;
            }
            */

        if (!GraphicsAndUI::paused)
        {
            uint32_t neuronsCount = Simulation::neuralNetworks->neuralNetworks[0]->neurons->count;

            for (uint32_t i = 0; i < neuronsCount; i++)
            {
                HH_NeuralNetworks::NEURAL_NOISE[i] = (double) rand()/RAND_MAX * HH_NeuralNetworks::MAX_NEURAL_NOISE - HH_NeuralNetworks::MAX_NEURAL_NOISE/2;
            }

            Simulation::generationTime = Simulation::agents->agents[0]->generationTime;

            if (!Simulation::training) //if testing
            {
                //if new rf stimulus
                if (Simulation::generationTime > Simulation::startRFStimulusTime + Simulation::currentRFStimulusDuration + Simulation::currentRFStimulusInterval)
                {
                    //get a new target temperature duration and intervals for the Simulation::agents
                    //this is chosen randomly so that the times vary
                    ////Simulation::SetCurrentRFStimulusDurationAndInterval();

                    rfTemperaturePulseCount++;

                    ////if (rfTemperaturePulseCount = Simulation::maxRFTemperaturePulseBeforeReset)
                        ////Simulation::currentRFStimulusInterval *= 10;

                    if (Simulation::averagingPulseSequencesResults && rfTemperaturePulseCount >= Simulation::maxRFTemperaturePulseBeforeReset)
                    {
                        Simulation::rfTemperaturePulseSequenceCount++;

                        if (Simulation::rfTemperaturePulseSequenceCount <= Simulation::maxRFTemperaturePulseSequenceTrials || (Simulation::rfTemperaturePulseSequenceCount % 10) == 0) //save graph data if less than the required testing trials or every 10 after
                        {
                            Simulation::resultsAndGraphs->SaveGraphs();

                            /*////
                            Simulation::resultsAndGraphs->rfTemperatureLessNormalTemperatureCurrentSpikeCountGraph->Save("", 0.1);
                            Simulation::resultsAndGraphs->normalTemperatureCurrentSpikeCountGraph->Save("", 0.1);
                            Simulation::resultsAndGraphs->rfTargetTemperatureGraph->Save("", 0.1);
                            Simulation::resultsAndGraphs->accruedErrorPercentageGraph->Save("", 0.1);
                            */
                        }

                        /*////Simulation::resultsAndGraphs->rfTemperatureLessNormalTemperatureCurrentSpikeCountGraph->ResetIndexes();
                        Simulation::resultsAndGraphs->normalTemperatureCurrentSpikeCountGraph->ResetIndexes();
                        Simulation::resultsAndGraphs->accruedErrorPercentageGraph->ResetIndexes();
                        */
                        Simulation::resultsAndGraphs->rfTargetTemperatureGraph->ResetIndexes();
                        Simulation::resultsAndGraphs->rfCurrentTemperatureGraph->ResetIndexes();
                        Simulation::neuralNetworks->ResetHH();
                        Simulation::neuralNetworks->ResetSpikeCounts();

                        Simulation::nextStimulusIndex = 0;
                        rfTemperaturePulseCount = 0;
                    }

                    Simulation::SetCurrentRFStimulusDurationAndInterval();

                    ////Simulation::currentRFStimulusDuration += (rfTemperaturePulseCount*1.5);
                    ////Simulation::currentRFStimulusInterval += (rfTemperaturePulseCount*1.5);
                    ////if (rfTemperaturePulseCount >= Simulation::maxRFTemperaturePulseBeforeReset - 1)
                        ///Simulation::currentRFStimulusInterval *= 10;


                    Simulation::startRFStimulusTime = Simulation::generationTime;
                }

                if (Simulation::generationTime - Simulation::startRFStimulusTime < Simulation::currentRFStimulusDuration)//stimulating with RF induced temperatures
                {
                    Simulation::agents->SetTargetTemperature(Simulation::referenceTemperature, 0);//set the temperature for the reference agent

                    //set the temperatures for the other Simulation::agents
                    double rfInducedTemperature;
                    for (uint32_t agentIndex=1; agentIndex<Simulation::agents->count; agentIndex++)
                    {
                        //get the specified temperature for each agent
                        rfInducedTemperature = Simulation::rfInducedTemperaturesFromPulsesForAgents[agentIndex-1];

                        Simulation::agents->SetTargetTemperature(rfInducedTemperature, agentIndex);
                    }
                }
                else //not RF stimulating, all Simulation::agents set to reference temperature as target
                {
                    Simulation::agents->SetTargetTemperature(Simulation::referenceTemperature);
                }
            }

            if (Simulation::generationTime == 0 && Simulation::totalSimTime > 1)//generationTime has been set to 0 so start a new generation
            {
                Simulation::startGenerationTime = Simulation::generationTime;

                if (Simulation::training) //if training, evolving
                {
                    GenerateNextGeneration(); //check fitness, if errors, mutate and create new generation, otherwise create new generation from fittest agent, end training and start testing

                    Simulation::agents->ResetTraces();
                }
                else
                {
                    if (Simulation::nnAvgsTesting)
                    {
                        Simulation::trainingGeneration++;

                        Simulation::training = true; //for experiments regenerating nn to get averages for layer testing
                        Simulation::generationDuration = Simulation::generationDurationEvolvingTraining;

                        Simulation::agents->RegenerateNNConnections();

                        Simulation::resultsAndGraphs->ResetTotalVoltageDifferencesForLayerAndTotalErrorPercentageForLayerResultingFromRFTemperatureIncreases();
                    }

                    Environment::foodForAgents_ptr->GenerateAndSaveFoodPos();
                    Environment::foodForAgents_ptr->SetFoodColor(Food::FoodDefaultColor);

                    Simulation::agents->ResetTraces();

                    /*////if (Simulation::nnAvgsTesting)
                    {
                        drawingGraphics = true;
                    }*/
                }

                Simulation::agents->Reset();

                //starting a new generation so also reset the start of the rf stimulus time
                Simulation::startRFStimulusTime = 0;

                Simulation::generations++;
            }

            //process the stimulus of the Simulation::agents
            if (Simulation::agents->Process() > 0) //Process() returns the number of Simulation::agents that have processed the result of the stimulus
            {                          //so if > 0 then save the data for the result graphs
                if (!Simulation::training) //if testing
                {
                    Simulation::resultsAndGraphs->DetermineAndGraphElectromagneticSpikeCountErrors();

                    if (Simulation::trainingGeneration < Simulation::maxTestingGenerations || (Simulation::trainingGeneration % 10) == 0) //save graph data if less than the required testing generations or every 10 after
                    {
                        if (Simulation::resultsAndGraphs->voltageDifferencesSamples > 0)
                        {
                            Simulation::resultsAndGraphs->SaveGraphs();
                        }
                    }
                }
            }

            if (!Simulation::training) //if testing
                if (Simulation::trainingGeneration < Simulation::maxTestingGenerations) //and if less than the required testing generations
                    Simulation::resultsAndGraphs->DetermineElectromagneticInducedTotalVoltageDifferences(); //determine the effects of the RF induced temperature increases on the membrane voltages



            Simulation::deltas =  Simulation::generationTime / Simulation::deltaTime;

            /*double round, remainder;

            round = MathUtilities::Round(Simulation::generationTime, 2);

            round = MathUtilities::Round(0.18000000000000002, 2);


            if (Simulation::generationTime == 1)
                int grc = 1;

            remainder = fmod(round, Simulation::graphDataPointIntervalDeltas);

            ////remainder = fmod(MathUtilities::Round(Simulation::generationTime, 2), Simulation::graphDataPointInterval);

            ////double fmodResult = fmod(round, Simulation::graphDataPointIntervalDeltas);
            ////if (fmodResult>-0.0000000000001 && fmodResult<0.0000000000001) //essentially zero
            */
            if (Simulation::deltas % Simulation::graphDataPointIntervalDeltas == 0)
            {
                //graph the voltage differences
                    Simulation::resultsAndGraphs->GraphRFMembraneVoltageDifferences();

                    Simulation::resultsAndGraphs->DetermineAndGraphElectromagneticSpikeCountErrors();

                //graph the current spike count
                Simulation::resultsAndGraphs->GraphCurrentExcitoryRFSpikeCount();

                //graph the current rf temperature
                Simulation::resultsAndGraphs->GraphCurrentRFTemperature();

            }

            //increment the simulation time
            Simulation::totalSimTime += Simulation::deltaTime;
        }
    }
}

int main(int argc, char *argv[])
{
    atexit(FreeMemory);

    glutInit(&argc, argv);
    GraphicsAndUI::InitGL();

    Simulation::Initialize(argv[1]);

    HANDLE threadHandle = (HANDLE)_beginthread(SimThreadFunction, 0, NULL);

    Sleep(1000);//wait for a second while initializing

    glutMainLoop();

    getchar();
}
