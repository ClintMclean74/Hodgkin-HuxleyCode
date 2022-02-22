#include <math.h>
#include <algorithm>
#include <string.h>
#include <windows.h>

#include "Environment.h"
#include "HH_NeuralNetwork.h"
#include "Agent.h"
#include "GL/glew.h"
#include "MathUtilities.h"
#include "Environment.h"
#include "Simulation.h"
#include "Agents.h"
#include "GraphicsUtilities.h"

Agent::Agent()
{
    index = Agents::count;

    char textBuffer[255];
    sprintf(textBuffer, "agent_logs/agent%i_%i.txt", index, Simulation::simStartRealTime);
    agentLog = fopen(textBuffer, "w+");

    ////sprintf(textBuffer, "agent_voltages/nnVoltages%i_%i.txt", index, Simulation::simStartRealTime);

    ////sprintf(textBuffer, "agent_voltages/nnVoltages%i_560657531.txt", index, Simulation::simStartRealTime);



    if (Simulation::loadingData)
    {
        ////sprintf(textBuffer, "agents_3nn_30layers_9_30_37_37_00002_37_00004/agent_voltages/nnVoltages%i_642929968.txt", index, Simulation::simStartRealTime);
        ////sprintf(textBuffer, "agents_2nn_9_30_37_37_000001/agent_voltages/nnVoltages%i_816196296.txt", index, Simulation::simStartRealTime);
        sprintf(textBuffer, "agent_voltages/nnVoltages%i_910055203.txt", index, Simulation::simStartRealTime);

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
    else if (Simulation::savingData)
    {
        ////voltagesFile = fopen(textBuffer, "w+");
        ////sprintf(textBuffer, "agent_logs/agent%i_%i.txt", index, Simulation::simStartRealTime);

        sprintf(textBuffer, "agent_voltages/nnVoltages%i_%i.txt", index, Simulation::simStartRealTime);
        savingVoltageDataFileBinary.open(textBuffer, std::ofstream::binary);
    }

    startNextStimIntervalSemaphore = CreateSemaphore(NULL, 0, 10, NULL);
    agentProcessedStimIntervalSemaphore = CreateSemaphore(NULL, 0, 10, NULL);

    startNextGenerationSemaphore = CreateSemaphore(NULL, 0, 10, NULL);
    agentProcessedGenerationSemaphore = CreateSemaphore(NULL, 0, 10, NULL);

    generationTime = 0;
    prevGenerationTime = 0;

    startStimTime = 0;


    length = Agent::radius/1.5;
    neuronRadius = Agent::radius/15;
    width = Agent::radius;


    /*////length = 200;

    Agent::radius = length*1.5;
    neuronRadius = length/10;

    ////width = Agent::radius*0.3;
    width = Agent::radius;
    */

    gridStart.Set((uint32_t)(((double) index)/Environment::ROWS) * Environment::GRID_WIDTH, (index % Environment::ROWS) * Environment::GRID_HEIGHT, 0);
    gridEnd.Set(gridStart.x + Environment::GRID_WIDTH, gridStart.y + Environment::GRID_HEIGHT, 0);

    pointing.Set(1, 0.0, 0);
    refPointing.Set(1, 0.0, 0);

    /*////
    velocity = 1;
    rotationDelta = 0.2;
    */

    velocity = 50;
    rotationDelta = 10;


    strcpy(foodRightStimulus, "000000111");
    strcpy(foodForwardStimulus, "000111000");
    strcpy(foodLeftStimulus, "111000000");
    strcpy(noFoodStimulus, "000000000");

    stimulusArray[0] = foodRightStimulus;
    stimulusArray[1] = foodForwardStimulus;
    stimulusArray[2] = foodLeftStimulus;
    stimulusArray[3] = noFoodStimulus;

    /*////strcpy(foodRightStimulus, "001");
    strcpy(foodForwardStimulus, "010");
    strcpy(foodLeftStimulus, "100");
    strcpy(noFoodStimulus, "000");
    */

    closestFoodIndex = -1;

    currentStimulus = "";

    ////*resultsBuffers[i] = *srcNeurons->resultsBuffers[i];

    agentColor.Set(255.0/255, 145.0/255, 44.0/255);
    neuronActiveColor.Set(255.0/255, 145.0/255, 44.0/255);

    neuralNetwork = NULL;

    resultStr = NULL;

    traceList = new RecurrentList(300);

    spikeCountsArrays = new uint32_t_ptr[HH_NeuralNetwork::LAYERS];

    ReGenerate();
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

void Agent::ReGenerate()
{
    fitness = 0;
    ////pos.Set((double) rand() / RAND_MAX * Environment::rangeX, (double) rand() / RAND_MAX * Environment::rangeY, 0);

    pointing.Set(1, 0.0, 0);

    pos.Set((uint32_t)(((double) index)/Environment::ROWS) * Environment::GRID_WIDTH + Environment::GRID_WIDTH/2, (index % Environment::ROWS) * Environment::GRID_HEIGHT + Environment::GRID_HEIGHT/2, 0);

    if (neuralNetwork)
        ((HH_NeuralNetwork*) neuralNetwork)->ResetHH();
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

    /*////if (pos.x<gridStart.x)
        pos.x = gridEnd.x;

    if (pos.x>gridEnd.x)
        pos.x = gridStart.x;

    if (pos.y<gridStart.y)
        pos.y = gridEnd.y;

    if (pos.y>gridEnd.y)
        pos.y = gridStart.y;
        */

    /*////if (newPos.x<gridStart.x)
        return;

    if (newPos.x>gridEnd.x)
        return;

    if (newPos.y<gridStart.y)
        return;

    if (newPos.y>gridEnd.y)
        return;
        */

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

        /*////if (newDistanceToFood <= distanceToFood)
            fitness += 0.1;
        else
            fitness -= 0.1;
            */

        distanceToFood = newDistanceToFood;

        Vector vectorToFood = Environment::foodForAgents_ptr->foodArray[closestFoodIndex]->pos - pos;
        double newAngleToFood = vectorToFood.CalculateAngle(pointing);

        /*////if (abs(newAngleToFood) <= abs(angleToFood))
            fitness += 0.1;
        else
            fitness -= 0.1;
            */

        angleToFood = newAngleToFood;

        ////angleToFood = (double) rand() / RAND_MAX * 6.28 - 3.14;

        if (angleToFood > 0.19625)
        {
            //std::cout << "Q = "<< foodLeftStimulus;
            //std::cout << "\n\n";
            currentStimulus = foodLeftStimulus;
            //((HH_NeuralNetwork*) neuralNetwork)->SetQ(foodLeftStimulus);
        }
        else if (angleToFood < -0.19625)
        {
            //std::cout << "Q = "<< foodRightStimulus;
            //std::cout << "\n\n";
            currentStimulus = foodRightStimulus;
            //((HH_NeuralNetwork*) neuralNetwork)->SetQ(foodRightStimulus);
        }
        else
        {
            //std::cout << "Q = "<< foodForwardStimulus;
            //std::cout << "\n\n";
            currentStimulus = foodForwardStimulus;
            //((HH_NeuralNetwork*) neuralNetwork)->SetQ(foodForwardStimulus);
        }


        /*////currentStimulus = stimulusArray[currentStimulusIndex++];

        if (currentStimulusIndex >= stimulusArrayLength)
            currentStimulusIndex = 0;
            */

        /*
        std::cout << "Agent index = "<< index;
        std::cout << "\n\n";
        std::cout << "Q = "<< currentStimulus;
        std::cout << "\n\n";
        */

        ((HH_NeuralNetwork*) neuralNetwork)->SetQ(currentStimulus);
    }
    else
        ((HH_NeuralNetwork*) neuralNetwork)->SetQ(noFoodStimulus);
}

void AgentProcessThread(void* param)
{
    Agent* agent = ((Agent *) param);

    srand(agent->index);

    if (Simulation::threading)
    {
        WaitForSingleObject(agent->startNextStimIntervalSemaphore, 100000);
        WaitForSingleObject(agent->startNextGenerationSemaphore, 100000);
    }

    while(true)
    {
        ////std::cout << "WaitForSingleObject(startNextGenerationSemaphore, 100000); " << agent->index;
        ////std::cout << "\n\n";
        if (Simulation::threading)
        {
            agent->prevGenerationTime = agent->generationTime;

            agent->Process();

            if (agent->generationTime >= Simulation::generationDuration || (agent->generationTime == 0 && agent->prevGenerationTime != 0))
            {
                ////std::cout << "ReleaseSemaphore(agent->agentProcessedGenerationSemaphore, 1, NULL); " << agent->index;
                ////std::cout << "\n\n";
                agent->generationTime = 0;
                ReleaseSemaphore(agent->agentProcessedGenerationSemaphore, 1, NULL);
                WaitForSingleObject(agent->startNextGenerationSemaphore, 100000);
            }
        }


        /*if (agent->sleepIndex > 1)
        {
            Sleep(Simulation::lag);
            agent->sleepIndex = 0;
        }*/

        while(Simulation::paused)
            Sleep(100);


        if (Simulation::lag > 0)
        {
        for(uint32_t i=0; i<(uint32_t)Simulation::lag;i++)
            Sleep(1);

        uint32_t max = 1/remainder(Simulation::lag, 1);

        if (agent->sleepIndex > max)
        {
            Sleep(Simulation::lag);
            agent->sleepIndex = 0;
        }

        agent->sleepIndex++;
        }
    }
}

void Agent::LaunchProcessingThread()
{
    agentProcessThreadHandle = Threading::CreateThread(AgentProcessThread, this);
}

void Agent::Process()
{
    ////if (!(index >= 0 && index <= 9))
    ////std::cout << "Agent Process: " << index << "\n";
    if (index == 1)
        int grc = 1;

    if (Simulation::stimulus)
    {
        uint32_t existingQCount, existingACount;
        ////if (generationTime >= startStimTime && generationTime < startStimTime + HH_NeuralNetwork::stimDuration && (index==0 || Simulation::currentSimTime>69))
        if (generationTime >= startStimTime && generationTime < startStimTime + HH_NeuralNetwork::stimDuration)
        {
            ////((HH_NeuralNetwork*) neuralNetwork)->ResetSpikeCounts();

            ////currentStimulus = stimulusArray[currentStimulusIndex];



        /*////if (angleToFood > 0.19625)
        {
            //std::cout << "Q = "<< foodLeftStimulus;
            //std::cout << "\n\n";
            currentStimulus = foodLeftStimulus;
            //((HH_NeuralNetwork*) neuralNetwork)->SetQ(foodLeftStimulus);
        }
        else if (angleToFood < -0.19625)
        {
            //std::cout << "Q = "<< foodRightStimulus;
            //std::cout << "\n\n";
            currentStimulus = foodRightStimulus;
            //((HH_NeuralNetwork*) neuralNetwork)->SetQ(foodRightStimulus);
        }
        else
        {
            //std::cout << "Q = "<< foodForwardStimulus;
            //std::cout << "\n\n";
            currentStimulus = foodForwardStimulus;
            //((HH_NeuralNetwork*) neuralNetwork)->SetQ(foodForwardStimulus);
        }
        */

        /*
        std::cout << "Agent index = "<< index;
        std::cout << "\n\n";
        std::cout << "Q = "<< currentStimulus;
        std::cout << "\n\n";
        */

            if (Simulation::trainingOrEvolution || Simulation::testingLayerErrors)
            {
                currentStimulus = stimulusArray[currentStimulusIndex];
                ((HH_NeuralNetwork*) neuralNetwork)->SetQ(currentStimulus);
            }
            else
            {
                ActivateNeuronsFromFoodAngle();

                /*////Action action = ((HH_NeuralNetwork*) neuralNetwork)->neurons->GetResultAction();

                    switch (action)
                    {
                        case (Action::RotateRight):
                           Rotate(-rotationDelta);
                        break;
                        case (Action::RotateLeft):
                            Rotate(-rotationDelta);
                        break;
                        case (Action::Forward):
                            Forward();
                        break;
                    }*/
            }
        }
        else
            if (generationTime >= startStimTime + HH_NeuralNetwork::stimDuration && generationTime < startStimTime + HH_NeuralNetwork::stimIntervalTimeFromStimStart)
                ((HH_NeuralNetwork*) neuralNetwork)->SetQ(noFoodStimulus);
            else
            {
                if (startStimTime < generationTime)
                {
                    startStimTime = generationTime;
                    ////angleToFood = (double) rand() / RAND_MAX * 1.57 - 0.785;

                    currentStimulusIndex++;
                    ////currentStimulusIndex=1;

                    if (currentStimulusIndex >= stimulusArrayLength - 1)
                        currentStimulusIndex = 0;


                    sprintf(textBuffer, "Simulation Time: %f", Simulation::currentSimTime);
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

                    if (Simulation::trainingOrEvolution)
                        ((HH_NeuralNetwork*) neuralNetwork)->neurons->AddResultsBuffers();
                    else
                    {
                        action = ((HH_NeuralNetwork*) neuralNetwork)->neurons->GetResultAction();

                        bool uncertain = false;

                        if (action == Action::None)
                        {
                            sprintf(textBuffer, "Selecting Random Action: ");

                            double randomDbl = (1 + (double) rand() / RAND_MAX * 3);

                            action = static_cast<Action> ((uint32_t) randomDbl);
                            ////action = static_cast<Action> (2);

                            if (action == 4)
                                action = (Action) 3;
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

                        uint32_t length = strlen(textBuffer);
                        fwrite(textBuffer, sizeof(char), length, agentLog);
                        fwrite("\n", sizeof(char), 1, agentLog);

                        fflush(agentLog);
                    }

                        sprintf(textBuffer, "Original Fitness: %f\n", fitness);
                        length = strlen(textBuffer);
                        fwrite(textBuffer, sizeof(char), length, agentLog);


                    existingQCount = ((HH_NeuralNetwork*) neuralNetwork)->neurons->GetExistingQCount(currentStimulus, ((HH_NeuralNetwork*) neuralNetwork)->neurons->layers-1);

                    /*//original if (existingQCount>1)
                        fitness -= existingQCount;
                        */

                    existingACount = ((HH_NeuralNetwork*) neuralNetwork)->neurons->GetExistingACount(((HH_NeuralNetwork*) neuralNetwork)->neurons->neurons[((HH_NeuralNetwork*) neuralNetwork)->neurons->layers-1], ((HH_NeuralNetwork*) neuralNetwork)->neurons->layers-1);

                    /*std::cout << "currentStimulus: " << currentStimulus;
                    std::cout << "\n\n";
                    std::cout << "Result Layer: " << ((HH_NeuralNetwork*) neuralNetwork)->neurons->neurons[((HH_NeuralNetwork*) neuralNetwork)->neurons->layers-1][0].spikeCount << " " << ((HH_NeuralNetwork*) neuralNetwork)->neurons->neurons[((HH_NeuralNetwork*) neuralNetwork)->neurons->layers-1][1].spikeCount << " " << ((HH_NeuralNetwork*) neuralNetwork)->neurons->neurons[((HH_NeuralNetwork*) neuralNetwork)->neurons->layers-1][2].spikeCount;
                    std::cout << "\n\n";
                    */

                    if (existingACount>1)
                    {
                        fitness -= existingACount;
                        /*std::cout << "fitness: " << fitness;
                        std::cout << "existingACount: " << existingACount;
                        std::cout << "existingACount>1" << "\n\n";
                        */
                    }

                    ////if (strcmp(resultStr, "0000000000")==0)
                    if (ResultsBuffer::Zeros(resultStr))
                    ////if (strcmp(resultStr, HH_NeuralNetwork::resultNullStr)==0)
                        fitness--;


                    sprintf(textBuffer, "New Fitness: %f\n", fitness);
                    length = strlen(textBuffer);
                    fwrite(textBuffer, sizeof(char), length, agentLog);


                    sprintf(textBuffer, "Existing Q count: %i\n", existingQCount);
                    length = strlen(textBuffer);
                    fwrite(textBuffer, sizeof(char), length, agentLog);

                    sprintf(textBuffer, "Existing A count: %i\n", existingACount);
                    length = strlen(textBuffer);
                    fwrite(textBuffer, sizeof(char), length, agentLog);

                    fwrite("\n", sizeof(char), 1, agentLog);

                    fflush(agentLog);

                    ((HH_NeuralNetwork*) neuralNetwork)->ResetHH(); //Reset Neurons for next stimulus
                }
                else //new generation
                {
                    startStimTime = 0;
                    ((HH_NeuralNetwork*) neuralNetwork)->ResetHH();
                }


                GetSpikeCountArraysForLayers(spikeCountsArrays);

                ReleaseSemaphore(agentProcessedStimIntervalSemaphore, 1, NULL);
                WaitForSingleObject(startNextStimIntervalSemaphore, 100000);

                ((HH_NeuralNetwork*) neuralNetwork)->ResetSpikeCounts();
            }
    }
    else
        ((HH_NeuralNetwork*) neuralNetwork)->SetQ(noFoodStimulus);

    ////if (!Simulation::loadingData)
    {
        if (Simulation::loadingData)
    {
        /*////char textBuffer[9999];
        memset(textBuffer, 0, 9999);
        fgets(textBuffer, 9999, loadingVoltageDataFile);

        generationTime = atof(textBuffer);

        ((HH_NeuralNetwork*) neuralNetwork)->neurons->LoadVoltageData(loadingVoltageDataFile);
        */


        generationTime = *reinterpret_cast<double*>(loadingVoltageDataBuffer + loadingVoltageDataBufferOffset);
        loadingVoltageDataBufferOffset += sizeof(double);

        ////std::cout << "generationTime = "<< generationTime;
        ////std::cout << "\n\n";

        loadingVoltageDataBufferOffset = ((HH_NeuralNetwork*) neuralNetwork)->neurons->LoadVoltageDataBinary(loadingVoltageDataBuffer, loadingVoltageDataBufferOffset);

        ////if ((uint32_t) (generationTime * 100) % 3 == 0)
            ////Sleep(7);
        /*////if (Simulation::nnVisible)
            Sleep(10);
            */
    }
    if (index == 1)
        int grc = 1;

        ((HH_NeuralNetwork*) neuralNetwork)->Process();

        if (Simulation::savingData)
        {

            savingVoltageDataFileBinary.write(reinterpret_cast<const char*> (&generationTime), sizeof(double));// Size of vector
////osfile.write(reinterpret_cast<const char*> (&vec[0]), sz*sizeof(float));
////osfile.write(reinterpret_cast<const char*> (&a), sizeof(unsigned int));
////osfile.write(reinterpret_cast<const char*> (&b), sizeof(unsigned int));

        ////sprintf(textBuffer, "%f", generationTime);
        ////fwrite(textBuffer, sizeof(char), strlen(textBuffer), voltagesFile);
        ////fwrite("\n", sizeof(char), 1, voltagesFile);

        /*////savingVoltageDataFileBinary.write(textBuffer, strlen(textBuffer));
        savingVoltageDataFileBinary.flush();

        ////((HH_NeuralNetwork*) neuralNetwork)->neurons->SaveVoltages(voltagesFile);
        ((HH_NeuralNetwork*) neuralNetwork)->neurons->SaveVoltagesBinary(&savingVoltageDataFileBinary);
        */

        ((HH_NeuralNetwork*) neuralNetwork)->neurons->SaveVoltagesBinary(&savingVoltageDataFileBinary);
        savingVoltageDataFileBinary.flush();
        ////savingVoltageDataFileBinary.close();

        ////Simulation::savingData = false;
        }

        /*////Action action = ((HH_NeuralNetwork*) neuralNetwork)->neurons->GetResultAction();

                    switch (action)
                    {
                        case (Action::RotateRight):
                           Rotate(-rotationDelta);
                        break;
                        case (Action::RotateLeft):
                            Rotate(rotationDelta);
                        break;
                        case (Action::Forward):
                            Forward();
                        break;
                    }
                    */

        int32_t closestFoodIndex = FoodAvailable();

        if (closestFoodIndex > -1)
            EatFood(closestFoodIndex);

        if (!Simulation::loadingData)
            generationTime += Simulation::deltaTime;

        ////((HH_NeuralNetwork*) neuralNetwork)->neurons->LoadVoltageData(loadingVoltageDataFile);
    }

    /*generationTime = (uint32_t )((generationTime +0.005) * 100);
    generationTime = ((double) generationTime)/100;
    */
}

int32_t Agent::FoodAvailable()
{
    return Environment::foodForAgents_ptr->WithinRadius(pos, std::max(width, length)/2, index);
}

void Agent::EatFood(uint32_t closestFoodIndex)
{
    Environment::foodForAgents_ptr->FoodEaten(closestFoodIndex);
    ////Environment::foodForAgents_ptr->RemoveFood(closestFoodIndex);
    ////((HH_NeuralNetwork*) neuralNetwork)->Reinforce(FoodForAgents::reinforcementValue);

    ////fitness++;
    if (!Simulation::trainingOrEvolution)
        fitness+=10;
}

void Agent::Draw()
{
    PosAngle *posAngle;

    traceList->Begin();
    posAngle = (PosAngle *) traceList->GetCurrent();

    ////glColor3f(neuronActiveColor.r, neuronActiveColor.g, neuronActiveColor.b);
    glColor3f(1, 1, 0);

    Vector newPos;
    ////glBegin(GL_LINES);
    glBegin(GL_LINE_STRIP);
    while(posAngle)
    {
        glVertex3f(posAngle->pos.x, posAngle->pos.y, posAngle->pos.z);
        newPos = posAngle->pos + posAngle->angle * velocity;
        glVertex3f(newPos.x, newPos.y, newPos.z);
        glVertex3f(posAngle->pos.x, posAngle->pos.y, posAngle->pos.z);

        ////Draw(posAngle->pos, -refPointing.CalculateAngle(posAngle->angle), 0.1);

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
    while(posAngle)
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
        ////glColor3f(0, 0, 0);
        GraphicsUtilities::DrawTextStr(posAngle->description, posAngle->pos.x, posAngle->pos.y + textYOffset, posAngle->pos.z, 0.05);

        prevPos = posAngle->pos;

        traceList->Next();
        posAngle = (PosAngle *) traceList->GetCurrent();
    };

    glColor3f(neuronActiveColor.r, neuronActiveColor.g, neuronActiveColor.b);
    Draw(pos, -refPointing.CalculateAngle(pointing));
}

void Agent::Draw(Vector agentPos, double angle, double scale, bool justTriangle)
{
    glPushMatrix();
    glTranslatef(agentPos.x, agentPos.y, agentPos.z);


    glRotatef(angle/MathUtilities::degreeToRadian, 0.0, 0.0, 1.0);

    ////glRotatef(45, 0.0, 0.0, 1.0);

    ////glColor3f(neuronActiveColor.r, neuronActiveColor.g, neuronActiveColor.b);

    glScalef(scale, scale, scale);

	glBegin(GL_LINE_STRIP);
        glVertex3f(0, width, 0);
        glVertex3f(length, 0, 0);
        glVertex3f(0, -width, 0);
        glVertex3f(0, width, 0);
    glEnd();

    ////justTriangle = true;

    if (!justTriangle)
    {
    Vector neuronPos(-Agent::radius, 0, 0);
    double neuronAngleDelta = -360/(double)HH_NeuralNetwork::layer1Length;

    neuronPos.Rotate(neuronAngleDelta/2 * MathUtilities::degreeToRadian);
    for (uint32_t i=0; i<HH_NeuralNetwork::layer1Length; i++)
    {
        GraphicsUtilities::DrawCircle(neuronPos.x, neuronPos.y, neuronRadius, 10, currentStimulus[i]=='1');
        neuronPos.Rotate(neuronAngleDelta * MathUtilities::degreeToRadian);
    }

    /*////
    neuronRadius = Agent::radius/15;
    ////neuronRadius = length/10;

    neuronPos.x = -length;
    neuronPos.y = neuronRadius * 3;

    for (int32_t i=2; i>=0; i--)
    {
        GraphicsUtilities::DrawCircle(neuronPos.x, neuronPos.y, neuronRadius, 10, ((uint32_t) action)==(i+1));
        neuronPos.y -= neuronRadius * 2;
    }

    neuronRadius = length/HH_NeuralNetwork::resultLayerLength;

    if (resultStr != NULL)
    {
        neuronPos.x = -length/2;
        neuronPos.y = neuronRadius * HH_NeuralNetwork::resultLayerLength;

        for (int32_t i=HH_NeuralNetwork::resultLayerLength-1; i>=0; i--)
        {
            GraphicsUtilities::DrawCircle(neuronPos.x, neuronPos.y, neuronRadius, 10, resultStr[i] != '0');
            neuronPos.y -= neuronRadius * 2;
        }
    }
    */
    }


	glPopMatrix();
}

Agent::~Agent()
{
    fclose(agentLog);
}

////double Agent::RotationDelta = 30;
double Agent::radius = 300;
