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

#include <math.h>
#include <algorithm>
#include <string.h>
#include <windows.h>
#include <io.h>

#include "Environment.h"
#include "HH_NeuralNetwork.h"
#include "Agent.h"
#include "GL/glew.h"
#include "MathUtilities.h"
#include "Environment.h"
#include "Simulation.h"
#include "Agents.h"
#include "GraphicsAndUI.h"

Agent::Agent()
{
    index = Agents::count;

    if (index > 0)
        resetLoadingData = true;

    char textBuffer[255];

    sprintf(textBuffer, "%s/%s/agent%i_%lu.txt", Simulation::saveRootFolder, Simulation::agentSaveLogFolder, index, Simulation::simStartRealTime);
    agentLog = fopen(textBuffer, "w+");

    if (Simulation::loadingData)
    {
        sprintf(textBuffer, "%s/%s/nnVoltages%i_1738528546.txt", Simulation::saveRootFolder, Simulation::loadAgentAllMembraneVoltagesFolder, index);

        loadingVoltageDataFile = fopen(textBuffer, "r+");

        if(loadingVoltageDataFileBinary)
        {
            loadingVoltageDataFileBinary.open(textBuffer, std::ifstream::binary);

            // get length of file
            loadingVoltageDataFileBinary.seekg(0, loadingVoltageDataFileBinary.end);
            size_t length = static_cast<size_t>(loadingVoltageDataFileBinary.tellg());

            loadingVoltageDataFileBinary.seekg(0, loadingVoltageDataFileBinary.beg);

            // read whole contents of the file to a buffer at once
            loadingVoltageDataBuffer = new char[length];
            loadingVoltageDataFileBinary.read(loadingVoltageDataBuffer, length);
            loadingVoltageDataFileBinary.close();
        }
    }
    else if (Simulation::saveAgentAllMembraneVoltagesTrace)
    {
        sprintf(textBuffer, "%s/%s/nnVoltages%i_%i.txt", Simulation::saveRootFolder, Simulation::saveAgentAllMembraneVoltagesFolder, index, Simulation::simStartRealTime);
        savingVoltageDataFileBinary.open(textBuffer, std::ofstream::binary);
    }

    generationTime = 0;
    prevGenerationTime = 0;

    resetHHTime = 0;
    startStimTime = 0;

    length = Agent::radius/1.5;
    neuronRadius = Agent::radius/15;
    width = Agent::radius;

    gridStart.Set((uint32_t)(((double) index)/Environment::ROWS) * Environment::GRID_WIDTH, (index % Environment::ROWS) * Environment::GRID_HEIGHT, 0);
    gridEnd.Set(gridStart.x + Environment::GRID_WIDTH, gridStart.y + Environment::GRID_HEIGHT, 0);

    pointing.Set(1, 0.0, 0);
    refPointing.Set(1, 0.0, 0);

    velocity = 50;
    rotationDelta = 10;

    closestFoodIndex = -1;
    currentStimulus = "";
    agentColor.Set(255.0/255, 145.0/255, 44.0/255);
    neuronActiveColor.Set(255.0/255, 145.0/255, 44.0/255);

    neuralNetwork = NULL;

    traceList = new RecurrentList(300);

    spikeCountsArrays = new uint32_t_ptr[HH_NeuralNetwork::LAYERS];
    resultStr = NULL;

    Reset();
}

Agent::Agent(Vector pos)
{
    Agent();

    this->pos = pos;
}

void Agent::SetNN(void* neuralNetwork)
{
    this->neuralNetwork = neuralNetwork;

    uint32_t length;

    for (uint32_t i=0; i<HH_NeuralNetwork::LAYERS;i++)
    {
        length = ((HH_NeuralNetwork*) neuralNetwork)->neurons->rowCountsForColumns[i];
        spikeCountsArrays[i] = new uint32_t[length];

        memset(spikeCountsArrays[i], 0, length * 4);
    }
}

void Agent::GetSpikeCountArraysForLayers(uint32_t **spikeCountsArrays)
{
    ((HH_NeuralNetwork*) neuralNetwork)->neurons->GetSpikeCountArraysForLayers(spikeCountsArrays);
}

double Agent::GetTemperature()
{
    return ((HH_NeuralNetwork*) neuralNetwork)->temperature;
}

void Agent::Reset()
{
    fitness = 0;
    pointing.Set(1, 0.0, 0);

    pos.Set((uint32_t)(((double) index)/Environment::ROWS) * Environment::GRID_WIDTH + Environment::GRID_WIDTH/2, (index % Environment::ROWS) * Environment::GRID_HEIGHT + Environment::GRID_HEIGHT/2, 0);

    if (neuralNetwork)
        ((HH_NeuralNetwork*) neuralNetwork)->ResetHH();
}

void Agent::RegenerateNNConnections()
{
    if (neuralNetwork)
    {
        ((HH_NeuralNetwork*) neuralNetwork)->RegenerateConnections();
        ((HH_NeuralNetwork*) neuralNetwork)->neurons->ClearResultsBuffers();
    }
}

void Agent::ResetTrace()
{
    traceList->Reset();
}

void Agent::AddTrace(char* description)
{
    PosAngle *posAngle = new PosAngle();
    posAngle->pos = pos;
    posAngle->angle = pointing;
    strcpy(posAngle->description, description);
    traceList->Add((void *) posAngle);
}

void Agent::Forward()
{
    Vector newPos = pos + pointing * velocity;

    if (newPos.x < gridStart.x)
        newPos.x = gridStart.x;

    if (newPos.x > gridEnd.x)
        newPos.x = gridEnd.x;

    if (newPos.y < gridStart.y)
        newPos.y = gridStart.y;

    if (newPos.y > gridEnd.y)
        newPos.y = gridEnd.y;

    pos = newPos;

    AddTrace("Forward");
}

void Agent::Rotate(double angle)
{
    pointing.Rotate(angle * MathUtilities::degreeToRadian);

    if (angle<0)
        AddTrace("Rotate Right");
    else
        AddTrace("Rotate Left");
}

void Agent::ActivateNeuronsFromFoodAngle()
{
    if (closestFoodIndex != -1 && Environment::foodForAgents_ptr->foodArray[closestFoodIndex] != NULL)
        Environment::foodForAgents_ptr->foodArray[closestFoodIndex]->color.Set(*FoodForAgents::foodColor);

    closestFoodIndex = Environment::foodForAgents_ptr->GetClosestFood(pos, index);

    if (closestFoodIndex != -1)
    {
        Environment::foodForAgents_ptr->foodArray[closestFoodIndex]->color.Set(*FoodForAgents::closestFoodColor);

        double newDistanceToFood = Environment::foodForAgents_ptr->GetDistanceToFood(pos, closestFoodIndex);

        distanceToFood = newDistanceToFood;

        Vector vectorToFood = Environment::foodForAgents_ptr->foodArray[closestFoodIndex]->pos - pos;
        double newAngleToFood = vectorToFood.CalculateAngle(pointing);

        angleToFood = newAngleToFood;

        if (angleToFood > 0.174532925199432) // greater than 10 degrees
        {
            currentStimulus = Simulation::foodLeftStimulus;
        }
        else if (angleToFood < -0.174532925199432) //less than -10 degrees
        {
            currentStimulus = Simulation::foodRightStimulus;
        }
        else //food with 20 degrees field of view
        {
            currentStimulus = Simulation::foodForwardStimulus;
        }

        ((HH_NeuralNetwork*) neuralNetwork)->SetQ(currentStimulus);
    }
    else
        ((HH_NeuralNetwork*) neuralNetwork)->ClearQ();
}

/*////void Agent::WriteToAgentLog()
{
    sprintf(textBuffer, "Simulation Time: %f", Simulation::generationTime);
    uint32_t length = strlen(textBuffer);
    fwrite(textBuffer, sizeof(char), length, agentLog);
    fwrite("\n", sizeof(char), 1, agentLog);

    sprintf(textBuffer, "This Agent Generation Time: %f", generationTime);
    length = strlen(textBuffer);
    fwrite(textBuffer, sizeof(char), length, agentLog);
    fwrite("\n", sizeof(char), 1, agentLog);

    fwrite(currentStimulus, sizeof(char), strlen(currentStimulus), agentLog);
    fwrite("\n", sizeof(char), 1, agentLog);

    resultStr = ((HH_NeuralNetwork*) neuralNetwork)->neurons->GetStringFromResult(((HH_NeuralNetwork*) neuralNetwork)->neurons->layers-1);

    fwrite(resultStr, sizeof(char), strlen(resultStr), agentLog);
    fwrite("\n", sizeof(char), 1, agentLog);
    fflush(agentLog);
}*/

//ProcessResult(): processes the final layer neural activation pattern, adds the
//stimulus, result and action mappings to buffers, determines the effect on fitness (errors) for training and also writes to the agent logs
uint32_t Agent::ProcessResult()
{
    uint32_t existingACount;
    startStimTime = generationTime + Simulation::resetHHDuration;
    nextStimulusIndex++;

    if (nextStimulusIndex >= Simulation::stimulusArrayLength)
        nextStimulusIndex = 0;

    if (Simulation::training)
        sprintf(textBuffer, "Training Agent:\n");
    else
        sprintf(textBuffer, "Testing Agent:\n");

    uint32_t textLength = strlen(textBuffer);
    fwrite(textBuffer, sizeof(char), textLength, agentLog);

    sprintf(textBuffer, "Simulation Time: %f", Simulation::generationTime);
    textLength = strlen(textBuffer);
    fwrite(textBuffer, sizeof(char), textLength, agentLog);
    fwrite("\n", sizeof(char), 1, agentLog);

    sprintf(textBuffer, "This Agent Generation Time: %f", generationTime);
    textLength = strlen(textBuffer);
    fwrite(textBuffer, sizeof(char), textLength, agentLog);
    fwrite("\n", sizeof(char), 1, agentLog);

    fwrite(currentStimulus, sizeof(char), strlen(currentStimulus), agentLog);

    resultStr = ((HH_NeuralNetwork*) neuralNetwork)->neurons->GetStringFromResult(((HH_NeuralNetwork*) neuralNetwork)->neurons->layers-1);

    fwrite(resultStr, sizeof(char), strlen(resultStr), agentLog);
    fwrite("\n", sizeof(char), 1, agentLog);
    fflush(agentLog);

    if (Simulation::training) //training so add the stimulus, result and determined action from stimulus mappings to the agents buffer
        ((HH_NeuralNetwork*) neuralNetwork)->neurons->AddStimulusResultActionMappingsToBuffers();

    //when the agent is being tested it will use the action stored for the activations, we also get it here to write it to the training logs
    action = ((HH_NeuralNetwork*) neuralNetwork)->neurons->GetResultAction();

    if (action == Action::None) //an action has not been stored for the activation pattern, these unknown patterns result from RF temperature induced errors
    {
        sprintf(textBuffer, "Selecting Random Action\n: ");
        textLength = strlen(textBuffer);
        fwrite(textBuffer, sizeof(char), textLength, agentLog);

        double randomDbl = (1 + (double) rand() / RAND_MAX * 3);
        action = static_cast<Action> ((uint32_t) randomDbl);

        if (action == 4) //numeral value for action should be 1, 2 or 3. rand() generates 4, since max is inclusive
            action = (Action) 3; //so set to 3
    }

    switch (action)
    {
        case (Action::RotateRight):
            Rotate(-rotationDelta);
            sprintf(textBuffer, "Action::RotateRight");
        break;
        case (Action::RotateLeft):
            Rotate(rotationDelta);
            sprintf(textBuffer, "Action::RotateLeft");
        break;
        case (Action::Forward):
            Forward();
            sprintf(textBuffer, "Action::Forward");
        break;
        case (Action::None):
            sprintf(textBuffer, "Action::None");
        break;
    }

    textLength = strlen(textBuffer);
    fwrite(textBuffer, sizeof(char), textLength, agentLog);
    fwrite("\n", sizeof(char), 1, agentLog);

    fflush(agentLog);

    sprintf(textBuffer, "Original Fitness: %f\n", fitness);
    textLength = strlen(textBuffer);
    fwrite(textBuffer, sizeof(char), textLength, agentLog);

    //adjust the agents' fitness according to whether the result activation pattern is in the buffer more than once
    //It should only be there once so that each stimulus has it's own result activation pattern, i.e, three actions for three stimulus patterns
    existingACount = ((HH_NeuralNetwork*) neuralNetwork)->neurons->GetExistingACount(((HH_NeuralNetwork*) neuralNetwork)->neurons->neurons[((HH_NeuralNetwork*) neuralNetwork)->neurons->layers-1], ((HH_NeuralNetwork*) neuralNetwork)->neurons->layers-1);

    if (existingACount>1)
    {
        fitness -= existingACount;
    }

    //also reduce the fitness if there isn't an activation pattern, since we want a stimulus to produce a resulting activation pattern
    //so that we can check the effects of temperature on these
    if (ResultsBuffer::Zeros(resultStr))
        fitness--;

    sprintf(textBuffer, "New Fitness: %f\n", fitness);
    textLength = strlen(textBuffer);
    fwrite(textBuffer, sizeof(char), textLength, agentLog);

    sprintf(textBuffer, "Existing A count: %i\n", existingACount);
    textLength = strlen(textBuffer);
    fwrite(textBuffer, sizeof(char), textLength, agentLog);

    fwrite("\n", sizeof(char), 1, agentLog);
    fflush(agentLog);

    ((HH_NeuralNetwork*) neuralNetwork)->ResetHH(); //Reset Neurons for next stimulus

    if (index > 0)
        resetLoadingData = true;

    GetSpikeCountArraysForLayers(spikeCountsArrays);

    /*////if (Simulation::stimulus)
    {
        if (Simulation::findStimulusValue)
        {
            if (index == 0)
            {
                if (((HH_NeuralNetwork*) neuralNetwork)->GetTotalSpikeCount() == 0)
                {
                    Simulation::stimulusFound = false;

                    Simulation::minStimulusValue = Simulation::stimulusValue;
                    Simulation::stimulusValue = (Simulation::maxStimulusValue + Simulation::stimulusValue) / 2;
                }
                else
                {
                    Simulation::stimulusFound = true;
                }
            }
            else
            {
                if (((HH_NeuralNetwork*) neuralNetwork)->GetTotalSpikeCount() > 0)
                {
                    if (Simulation::findStimulusValue)
                    {
                        Simulation::stimulusFound = false;

                        Simulation::maxStimulusValue = Simulation::stimulusValue;
                        Simulation::stimulusValue = (Simulation::minStimulusValue + Simulation::stimulusValue) / 2;
                    }
                }
                else
                {
                    Simulation::stimulusFound = true;
                }
            }
        }

        if (Simulation::findWeightValue)
        {
            if (index == 0)
            {
                if (((HH_NeuralNetwork*) neuralNetwork)->GetSpikeCountsForLayer(1) == 0)
                {
                    Simulation::weightFound = false;

                    Simulation::minWeightValue = Simulation::weightValue;

                    if (Simulation::weightValue == Simulation::maxWeightValue)
                    {
                        Simulation::maxWeightValue = Simulation::maxWeightValue * 2;
                        Simulation::weightValue = Simulation::maxWeightValue;
                    }
                    else
                        Simulation::weightValue = (Simulation::maxWeightValue + Simulation::weightValue) / 2;
                }
                else
                {
                    Simulation::weightFound = true;
                }
            }
            else
            {
                if (((HH_NeuralNetwork*) neuralNetwork)->GetSpikeCountsForLayer(1) > 0)
                {
                    if (Simulation::findWeightValue)
                    {
                        Simulation::weightFound = false;
                        Simulation::maxWeightValue = Simulation::weightValue;

                        if (Simulation::weightValue == Simulation::minWeightValue)
                        {
                            Simulation::minWeightValue = Simulation::minWeightValue / 2;
                            Simulation::weightValue = Simulation::minWeightValue;
                        }
                        else
                            Simulation::weightValue = (Simulation::minWeightValue + Simulation::weightValue) / 2;
                    }
                }
            }

            ((HH_NeuralNetwork*) neuralNetwork)->neurons->SetSynapseConductance(Simulation::weightValue);
        }

        ((HH_NeuralNetwork*) neuralNetwork)->ResetHH();
    }
    */

    ((HH_NeuralNetwork*) neuralNetwork)->ResetSpikeCounts();
}

uint32_t Agent::Process(bool startInputStimulus)
{
    uint32_t status = 0;

    GetSpikeCountArraysForLayers(spikeCountsArrays);

    ProcessingStatus prevProcessingStatus = processingStatus;

    //Reset the Hodgkin-Huxley neurons before processing the new stimulus so that it's unaffected from previous processing
    if (generationTime >= resetHHTime && generationTime < resetHHTime + Simulation::resetHHDuration)
    {
        processingStatus = ProcessingStatus::ResettingHH;

        /*////if (prevProcessingStatus != ProcessingStatus::ResettingHH)
            ((HH_NeuralNetwork*) neuralNetwork)->ResetHH();
            */
    }
    else //stimulating the first layer, sensory neurons
        if (generationTime >= resetHHTime + Simulation::resetHHDuration && generationTime < resetHHTime + Simulation::resetHHDuration + Simulation::stimDuration)
        {
            if (startInputStimulus)
                processingStatus = ProcessingStatus::Stimulating;
        }
        else //first layer neurons have been stimulated, now process the signals through the network for a specified duration (Simulation::synapticStimulusIntervalFromStimStart), long enough for the signals to activate the final layers
            if (generationTime >= resetHHTime + Simulation::resetHHDuration + Simulation::stimDuration && generationTime < resetHHTime + Simulation::resetHHDuration + Simulation::synapticStimulusIntervalFromStimStart)
            {
                processingStatus = ProcessingStatus::ProcessignStimulus;
            }
            else //now process the final layer activations
                if (generationTime >= resetHHTime + Simulation::resetHHDuration + Simulation::synapticStimulusIntervalFromStimStart)
                {
                    /*////
                    ProcessResult();
                    status = 1;
                    resetHHTime = generationTime;
                    */

                    processingStatus = ProcessingStatus::ProcessedResult;
                }

    switch(processingStatus)
    {
        case(ProcessingStatus::ResettingHH):
            if (prevProcessingStatus != ProcessingStatus::ResettingHH)
                ((HH_NeuralNetwork*) neuralNetwork)->ResetHH();

            ((HH_NeuralNetwork*) neuralNetwork)->ClearQ();
        break;
        case(ProcessingStatus::Stimulating):
            if (Simulation::loadingData && resetLoadingData)
            {
                loadingVoltageDataBufferOffset = 0;
                resetLoadingData = false;
            }

            if (Simulation::training || Simulation::testingLayerErrors)
            {
                currentStimulus = Simulation::stimulusArray[nextStimulusIndex];
                ((HH_NeuralNetwork*) neuralNetwork)->SetQ(currentStimulus);
            }
            else
            {
                ActivateNeuronsFromFoodAngle();
            }
        break;
        case(ProcessingStatus::ProcessignStimulus):
            ((HH_NeuralNetwork*) neuralNetwork)->ClearQ();
        break;
        case(ProcessingStatus::ProcessedResult):
            ProcessResult();
            status = 1;

            resetHHTime = generationTime; //set the resetHHTime to the current generationTime so that the process starts again
        break;
    }

    if (Simulation::loadingData)
    {
        generationTime = *reinterpret_cast<double*>(loadingVoltageDataBuffer + loadingVoltageDataBufferOffset);
        loadingVoltageDataBufferOffset += sizeof(double);

        loadingVoltageDataBufferOffset = ((HH_NeuralNetwork*) neuralNetwork)->neurons->LoadVoltageDataBinary(loadingVoltageDataBuffer, loadingVoltageDataBufferOffset);
    }

    //process the Hodgkin-Huxley network
    ((HH_NeuralNetwork*) neuralNetwork)->Process();

    if (Simulation::saveAgentAllMembraneVoltagesTrace)
    {
        savingVoltageDataFileBinary.write(reinterpret_cast<const char*> (&generationTime), sizeof(double));// Size of vector

        ((HH_NeuralNetwork*) neuralNetwork)->neurons->SaveVoltagesBinary(&savingVoltageDataFileBinary);
        savingVoltageDataFileBinary.flush();
    }

    if (!Simulation::training && !Simulation::testingLayerErrors) //AI food finding task
    {
        int32_t closestFoodIndex = FoodAvailable(); //is food available, close enough to be eaten

        if (closestFoodIndex > -1)
            EatFood(closestFoodIndex);
    }

    if (!Simulation::loadingData)
    {
        generationTime += Simulation::deltaTime; //increment the simulation time

        //reset the generation time if longer than the generation duration
        //the agents are managed in main.cpp when generationTime == 0, i.e, evolve (train) or test the next generation
        //when sufficiently evolved (trained), errors == 0 then the new generation is tested for effects of RF temperature increases
        if (generationTime >= Simulation::generationDuration || (generationTime == 0 && prevGenerationTime != 0))
        {
            generationTime = 0;
            resetHHTime = 0;
        }
    }

    if (Simulation::lag > 0)//use a lag to slow the simulation to check data
        Sleep(Simulation::lag);

    return status;
}

int32_t Agent::FoodAvailable()
{
    return Environment::foodForAgents_ptr->WithinRadius(pos, std::max(width, length)/2, index);
}

void Agent::EatFood(uint32_t closestFoodIndex)
{
    Environment::foodForAgents_ptr->FoodEaten(closestFoodIndex);

    if (!Simulation::training)
        fitness+=10;
}

void Agent::Draw()
{
    PosAngle *posAngle;

    traceList->Begin();
    posAngle = (PosAngle *) traceList->GetCurrent();

    glColor3f(1, 1, 0);

    Vector newPos;
    glBegin(GL_LINE_STRIP);
    while(posAngle) //draw the agents movement trace
    {
        glVertex3f(posAngle->pos.x, posAngle->pos.y, posAngle->pos.z);
        newPos = posAngle->pos + posAngle->angle * velocity;
        glVertex3f(newPos.x, newPos.y, newPos.z);
        ////glVertex3f(posAngle->pos.x, posAngle->pos.y, posAngle->pos.z);

        traceList->Next();
        posAngle = (PosAngle *) traceList->GetCurrent();
    };
    glEnd();

    double angle;
    double textYOffset = 0;
    Vector prevPos;
    uint32_t actions = 0;

    traceList->Begin();
    posAngle = (PosAngle *) traceList->GetCurrent();
    while(posAngle) //draw the agents and the text for the action at each trace
    {
        actions++;

        angle = -refPointing.CalculateAngle(posAngle->angle);

        glColor3f(agentColor.r, agentColor.g, agentColor.b);
        Draw(posAngle->pos, angle, 0.1, true);

        if (posAngle->pos == prevPos && actions > 1)
            textYOffset-= 5;
        else
            textYOffset = 0;

        glColor3f(1, 1, 1);
        GraphicsAndUI::DrawTextStr(posAngle->description, posAngle->pos.x, posAngle->pos.y + textYOffset, posAngle->pos.z, 0.05);

        prevPos = posAngle->pos;

        traceList->Next();
        posAngle = (PosAngle *) traceList->GetCurrent();
    };

    glColor3f(neuronActiveColor.r, neuronActiveColor.g, neuronActiveColor.b);
    Draw(pos, -refPointing.CalculateAngle(pointing));
}

//Draw the agent
void Agent::Draw(Vector agentPos, double angle, double scale, bool justTriangle)
{
    glPushMatrix();
    glTranslatef(agentPos.x, agentPos.y, agentPos.z);

    glRotatef(angle/MathUtilities::degreeToRadian, 0.0, 0.0, 1.0);

    glScalef(scale, scale, scale);

	glBegin(GL_LINE_STRIP);
        glVertex3f(0, width, 0);
        glVertex3f(length, 0, 0);
        glVertex3f(0, -width, 0);
        glVertex3f(0, width, 0);
    glEnd();

    if (!justTriangle)
    {
        Vector neuronPos(Agent::radius*1.2, 0, 0);
        double neuronAngleDelta = -160/((double)HH_NeuralNetwork::layer1Length - 1);

        neuronPos.Rotate(-neuronAngleDelta * MathUtilities::degreeToRadian);
        for (uint32_t i=0; i<HH_NeuralNetwork::layer1Length; i++)
        {
            GraphicsAndUI::DrawCircle(neuronPos.x, neuronPos.y, neuronRadius, 10, currentStimulus[i]=='1');
            neuronPos.Rotate(neuronAngleDelta * MathUtilities::degreeToRadian);
        }
    }

	glPopMatrix();
}

Agent::~Agent()
{
    delete traceList;

    for (uint32_t i=0; i<HH_NeuralNetwork::LAYERS;i++)
    {
        delete spikeCountsArrays[i];
    }

    delete [] spikeCountsArrays;

    loadingVoltageDataFileBinary.close();
    savingVoltageDataFileBinary.close();

    fclose(agentLog);
}

double Agent::radius = 300;
