#ifndef AGENT_H
#define AGENT_H

#include <fstream>

#include "Vector.h"
#include "FoodForAgents.h"
#include "Threading.h"
#include "ResultsBuffer.h"
#include "RecurrentList.h"

typedef uint32_t* uint32_t_ptr;

class PosAngle
{
    public:
        Vector pos;
        Vector angle;
        char description[255];
};

class Agent
{
    private:
        double width;
        double length;

        double neuronRadius;

        Vector gridStart;
        Vector gridEnd;

        char foodLeftStimulus[10];
        char foodForwardStimulus[10];
        char foodRightStimulus[10];
        char noFoodStimulus[10];

        static const uint32_t stimulusArrayLength = 4;
        char* stimulusArray[stimulusArrayLength];

        uint32_t currentStimulusIndex = 0;

        HANDLE agentProcessThreadHandle;

        FoodForAgents *foodForAgents_ptr;
        char textBuffer[255];

        Color agentColor;
        Color neuronActiveColor;

        FILE* agentLog;
        FILE* loadingVoltageDataFile;
        FILE* voltagesFile;
        std::ifstream loadingVoltageDataFileBinary;
        std::ofstream savingVoltageDataFileBinary;
        char* loadingVoltageDataBuffer;
        size_t loadingVoltageDataBufferOffset = 0;

        RecurrentList *traceList;

        void AddTrace(char* description);

    public:
        static double radius;
        uint32_t index;
        uint32_t sleepIndex;
        double rotationDelta;
        void* neuralNetwork;

        uint32_t closestFoodIndex;

        Vector pos;
        Vector pointing;
        Vector refPointing;
        double velocity;
        double distanceToFood;
        double angleToFood;
        char* currentStimulus;
        char* resultStr;
        Action action;

        double fitness;

        double generationTime;
        double prevGenerationTime;

        double startStimTime;

        HANDLE agentProcessedStimIntervalSemaphore;
        HANDLE startNextStimIntervalSemaphore;

        HANDLE startNextGenerationSemaphore;
        HANDLE agentProcessedGenerationSemaphore;

        uint32_t **spikeCountsArrays;

        Agent();
        Agent(Vector pos);
        void SetNN(void* neuralNetwork);
        void GetSpikeCountArraysForLayers(uint32_t **spikeCountsArrays);
        double GetTemperature();
        void DetermineElectromagneticSpikeCountErrors(uint32_t *spikeCounts);
        void ReGenerate();
        void ResetTrace();
        void Forward();
        void Rotate(double angle);
        void ActivateNeuronsFromFoodAngle();
        void LaunchProcessingThread();
        void Process();
        int32_t FoodAvailable();
        void EatFood(uint32_t closestFoodIndex);
        void Draw();
        void Draw(Vector agentPos, double angle, double scale = 1, bool justTriangle = false);

        virtual ~Agent();
};

typedef Agent* Agent_ptr;

#endif // AGENT_H
