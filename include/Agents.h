#ifndef AGENTS_H
#define AGENTS_H

#include "pstdint.h"
#include "Agent.h"

class Agents
{
    public:
        Agent_ptr *agents;
        static uint32_t count;

        static uint32_t maxSize;

        HANDLE* agentProcessedStimIntervalSemaphores;
        HANDLE* agentProcessedGenerationSemaphores;

        Agents();
        Agents(uint32_t maxSize);
        double GetTotalFitness();
        double GetAvgFitness();
        void ReGenerate();
        void ResetTraces();
        void Add(Agent *agent);
        void AddNewNNHiddenLayerToAgents();
        void SetTemperature(double temperature);
        void ActivateNeuronsFromFoodAngle();
        void ProcessIfEating();
        void LaunchProcessingThreads();
        void ReleaseStimIntervalSemaphores();
        void ReleaseSemaphores();
        void Process(uint32_t agentIndex = maxSize);
        void Draw();
        virtual ~Agents();
};

#endif // AGENTS_H
