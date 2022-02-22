#ifndef TrainingSet_H
#define TrainingSet_H TRUE

#include "pstdint.h"
#include "TrainingQA.h"

class TrainingSet
{
    private:
        static const uint32_t MAX_QAs = 256;

        TrainingQA_ptr* trainingQAs;

    public:
        static double stimulusDurationHH;
        static double stimulusIntervalHH;
        static double preStimulusDuration;
        static double InitializationInterval;

        static double stimulusDuration;
        static double stimulusInterval;

        static double stimulusDurationEvolved;
        static double stimulusIntervalEvolved;

        static uint32_t qLength;
        static uint32_t resultLength;
        static char inputNullStr[255];

        uint32_t size;
        uint32_t currentSubSetSize;
        static uint32_t maxSubsetSize;

        int32_t currentQAIndex;

        TrainingSet(uint32_t size);
        TrainingQA* GetNextQA();

        ~TrainingSet();
};


#endif // TrainingSet_H
