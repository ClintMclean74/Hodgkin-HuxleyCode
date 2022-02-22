#ifndef ResultsBuffer_H
#define ResultsBuffer_H TRUE

#include "pstdint.h"
#include "TrainingSet.h"
#include "HH_Neuron.h"

enum Action {None, RotateRight, Forward, RotateLeft};

class Result
{
    public:
        char* questionBuffer;
        uint32_t* resultBuffer;
        Action action;
        uint32_t length;

        Result(uint32_t length);
        void Load(FILE* file);
        void Save(FILE* file);
        bool Equals(char* q);
        bool Equals(HH_Neuron* result);
        bool Equals(uint32_t* result);
        bool Equals(char* q, uint32_t* result);
        bool Equals(char* q, HH_Neuron* result);

        /*////bool operator==(HH_Neuron* result);
        void operator=(HH_Neuron* result);
        */
        void operator=(Result result);
};

typedef Result* Result_ptr;


class ResultsBuffer
{
    public:
        uint32_t maxSize = 100;
        uint32_t currentIndex = 0;
        uint32_t error = 0;

        Result_ptr* buffer;

        ResultsBuffer(uint32_t resultLength, uint32_t bufferSize);
        void Load(FILE* file);
        void Save(FILE* file);
        void operator=(ResultsBuffer srcBuffer);

        void Add(char* q, HH_Neuron* result);
        void Add(char* q, HH_Neuron* result, Action action);
        Result* Existing(char* q, uint32_t* result);
        Result* Existing(char* q, HH_Neuron* result);
        uint32_t GetExistingQCount(char* q);
        uint32_t GetExistingACount(HH_Neuron* result);
        Action GetActionForResult(uint32_t* result);
        void Copy(ResultsBuffer* srcBuffer);
        static bool Zeros(char* str);
        void Clear();


        ~ResultsBuffer();
};

typedef ResultsBuffer* ResultsBuffer_ptr;

#endif // ResultsBuffer_H
