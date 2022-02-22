#ifndef SIMULATION_H
#define SIMULATION_H

#include "Threading.h"
#include "pstdint.h"

class Simulation
{
    private:

    public:
        static uint32_t simStartRealTime;
        static double currentSimTime;
        static double deltaTime;
        static double generationDurationEvolvingTraining;
        static double generationDurationLayerTesting;
        static double generationDurationFoodTesting;
        static double generationDuration;
        static uint32_t generations;

        static double stimulusDuration;
        static double stimulusInterval;

        static double lag;
        static double stimulus;
        static bool threading;
        static bool trainingOrEvolution;
        static bool testingLayerErrors;
        static bool loadingData;
        static bool savingData;
        static HANDLE startNextGenerationSemaphore;
        static bool nnVisible;
        static bool paused;
        static uint8_t floatingPointRounding;

        Simulation();
        virtual ~Simulation();
};

#endif // SIMULATION_H
