#include <windows.h>
#include "Environment.h"
#include "Agents.h"
#include "MathUtilities.h"
#include "HH_NeuralNetwork.h"
#include "Simulation.h"

Agents::Agents()
{
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

void Agents::ReGenerate()
{
    for(uint32_t i=0; i<count;i++)
    {
        agents[i]->ReGenerate();
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

void Agents::AddNewNNHiddenLayerToAgents()
{
    for(uint32_t i=0; i<count;i++)
    {
        ((HH_NeuralNetwork *) agents[i]->neuralNetwork)->neurons->AddNNHiddenLayer(agents[i]->neuralNetwork);
    }
}

void Agents::SetTemperature(double temperature)
{
    for(uint32_t i=0; i<count;i++)
    {
        ((HH_NeuralNetwork *) agents[i]->neuralNetwork)->SetTemperature(temperature);
    }
}

void Agents::ActivateNeuronsFromFoodAngle()
{
    Environment::foodForAgents_ptr->SetFoodColor(Food::FoodDefaultColor); //set to green, closest food to agent in ActivateNeuronsFromFoodAngle() is set to gold

    for(uint32_t i=0; i<count;i++)
    {
        agents[i]->ActivateNeuronsFromFoodAngle();
    }
}

void Agents::LaunchProcessingThreads()
{
    agentProcessedStimIntervalSemaphores = new HANDLE[count];
    agentProcessedGenerationSemaphores = new HANDLE[count];

    for(uint32_t i=0; i<count;i++)
    {
        agentProcessedStimIntervalSemaphores[i] = agents[i]->agentProcessedStimIntervalSemaphore;
        agentProcessedGenerationSemaphores[i] = agents[i]->agentProcessedGenerationSemaphore;
    }

    for(uint32_t i=0; i<count;i++)
    {
        agents[i]->LaunchProcessingThread();
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

void Agents::ReleaseStimIntervalSemaphores()
{
    for(uint32_t i=0; i<count;i++)
        ReleaseSemaphore(agents[i]->startNextStimIntervalSemaphore, 1, NULL);
}

void Agents::ReleaseSemaphores()
{
    for(uint32_t i=0; i<count;i++)
        ReleaseSemaphore(agents[i]->startNextGenerationSemaphore, 1, NULL);
}

void Agents::Process(uint32_t agentIndex)
{
    if (agentIndex != maxSize)
        agents[agentIndex]->Process();
    else
        for(uint32_t i=0; i<count;i++)
        {
            agents[i]->Process();
        }
}

void Agents::Draw()
{
    for(uint32_t i=0; i<count;i++)
    {
        agents[i]->Draw();
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
