#include <time.h>
#include <string.h>

#include <bitset>
#include "TrainingSet.h"
#include "HH_NeuralNetwork.h"

//#define TRAININGSET0
////#define TRAININGSET8TO3
////#define TRAININGSET3TO8
////#define TRAININGSET8TO8
#define TRAININGSETVISION

char* ValueToResultIndex(uint8_t value, char* text, uint32_t arrayLength)
{
    if (value == 0)
    {
        memset(text, '0', arrayLength);
        return text;
    }

    uint32_t i = 0;
    uint32_t j = arrayLength-1;

    for (i = 1; i<arrayLength; i++)
    {
        if (i == value)
            text[j--] = '1';
        else
            text[j--] = '0';
    }

    text[j] = '0';

    return text;
}

TrainingSet::TrainingSet(uint32_t size)
{
    this->size = size;
    currentSubSetSize = 2;
    ////currentSubSetSize = maxSubsetSize;

    trainingQAs = new TrainingQA_ptr[MAX_QAs];
    currentQAIndex = -1;

    time_t t;
    srand((unsigned) time(&t));

    for (uint32_t i = 0; i<size; i++)
    {
        trainingQAs[i] = new TrainingQA(HH_NeuralNetwork::ROWS[0], HH_NeuralNetwork::ROWS[HH_NeuralNetwork::LAYERS - 1]);
        //trainingQAs[i]->Initialize();

        //trainingQAs[i]->Set("1, 0, 1, 0", "0, 1, 0, 0");
    }


    uint32_t i =0;

    std::string str1;
    std::string str2;

    char text[256];

    memset(text, 0, 256);
    const char* cStr;

    #ifdef TRAININGSETVISION
            trainingQAs[0]->Set("00000111", "001");
            trainingQAs[1]->Set("00000000", "000");
            trainingQAs[2]->Set("00011000", "010");
            trainingQAs[3]->Set("00000000", "000");
            trainingQAs[4]->Set("11100000", "100");
            trainingQAs[5]->Set("00000000", "000");
    #else
    uint32_t index;
    for (i=1; i<256; i++)
    {
        #ifdef TRAININGSET0
        trainingQAs[i]->Set("1", "1");
        #endif // TRAININGSET0

        str1 = std::bitset<8>(i).to_string();
        cStr = str1.c_str();


        #ifdef TRAININGSET8TO8
        ////8 bits to 8 index
        ////index = 255 - i;

        index = (double) rand() / RAND_MAX * 255;


        str1 = std::bitset<8>(index).to_string();
        cStr = str1.c_str();
        trainingQAs[i]->Set(cStr, ValueToResultIndex(index, text, TrainingSet::qLength));
        #endif

        #ifdef TRAININGSET8TO3
        ////8 index to 3 bits
        cStr = &cStr[8-TrainingSet::resultLength];
        trainingQAs[i]->Set(ValueToResultIndex(i, text, TrainingSet::qLength), cStr);
        #endif

        #ifdef TRAININGSET3TO8
        ////3 bits to 8 index
        cStr = &cStr[8-TrainingSet::qLength];

        if (i==7)
            trainingQAs[i]->Set(cStr, ValueToResultIndex(1, text, TrainingSet::resultLength));
        else
            trainingQAs[i]->Set(cStr, ValueToResultIndex(i+1, text, TrainingSet::resultLength));

        #endif
    }
    #endif // TRAININGSETVISION

    for (i=0; i<HH_NeuralNetwork::ROWS[0]; i++)
    {
        TrainingSet::inputNullStr[i] = '0';
    }

    TrainingSet::inputNullStr[i] = 0;


    /*trainingQAs[i++]->Set("1, 0", "1");
    trainingQAs[i++]->Set("0, 1", "1");
    trainingQAs[i++]->Set("1, 1", "0");
    trainingQAs[i++]->Set("0, 0", "0");
    */


    /*
    trainingQAs[i++]->Set("1, 0, 0, 0", "1, 0, 0, 0");//1 = 1
    trainingQAs[i++]->Set("0, 1, 0, 0", "0, 1, 0, 0");//2 = 2
    trainingQAs[i++]->Set("1, 1, 0, 0", "1, 1, 0, 0");//3 = 3

    trainingQAs[i++]->Set("1, 1, 1, 0", "0, 0, 1, 0");//3 + 1 = 4
    trainingQAs[i++]->Set("1, 1, 0, 1", "1, 0, 1, 0");//3 + 2 = 5
    trainingQAs[i++]->Set("1, 1, 1, 1", "0, 1, 1, 0");//3 + 3 = 6
    */


    /*trainingQAs[i++]->Set("0, 0, 1, 0", "1, 0, 0, 0");//1 = 1
    trainingQAs[i++]->Set("1, 0, 0, 0", "1, 0, 0, 0");//1 = 1
    trainingQAs[i++]->Set("0, 1, 0, 0", "0, 1, 0, 0");//2 = 2
    trainingQAs[i++]->Set("1, 1, 0, 0", "1, 1, 0, 0");//3 = 3

    trainingQAs[i++]->Set("1, 0, 1, 0", "0, 1, 0, 0");//1 + 1 = 2

    trainingQAs[i++]->Set("0, 1, 1, 0", "1, 1, 0, 0");//2 + 1 = 3
    trainingQAs[i++]->Set("1, 0, 0, 1", "1, 1, 0, 0");//1 + 2 = 3

    trainingQAs[i++]->Set("1, 1, 1, 0", "0, 0, 1, 0");//3 + 1 = 4
    trainingQAs[i++]->Set("0, 1, 0, 1", "0, 0, 1, 0");// 2 + 2 = 4
    trainingQAs[i++]->Set("1, 0, 1, 1", "0, 0, 1, 0");//1 + 3 = 4

    trainingQAs[i++]->Set("1, 1, 0, 1", "1, 0, 1, 0");//3 + 2 = 5

    trainingQAs[i++]->Set("0, 1, 1, 1", "1, 0, 1, 0");//2 + 3 = 5
    trainingQAs[i++]->Set("1, 1, 1, 1", "0, 1, 1, 0");//3 + 3 = 6
    */


    /*trainingQAs[i++]->Set("0, 0, 1, 0", "1, 0, 0, 0, 0, 0");//1 = 1
    trainingQAs[i++]->Set("1, 0, 0, 0", "1, 0, 0, 0, 0, 0");//1 = 1
    trainingQAs[i++]->Set("0, 1, 0, 0", "0, 1, 0, 0, 0, 0");//2 = 2
    trainingQAs[i++]->Set("1, 1, 0, 0", "0, 0, 1, 0, 0, 0");//3 = 3

    trainingQAs[i++]->Set("1, 0, 1, 0", "0, 1, 0, 0, 0, 0");//1 + 1 = 2

    trainingQAs[i++]->Set("0, 1, 1, 0", "0, 0, 1, 0, 0, 0");//2 + 1 = 3
    trainingQAs[i++]->Set("1, 0, 0, 1", "0, 0, 1, 0, 0, 0");//1 + 2 = 3

    trainingQAs[i++]->Set("1, 1, 1, 0", "0, 0, 0, 1, 0, 0");//3 + 1 = 4
    trainingQAs[i++]->Set("0, 1, 0, 1", "0, 0, 0, 1, 0, 0");// 2 + 2 = 4
    trainingQAs[i++]->Set("1, 0, 1, 1", "0, 0, 0, 1, 0, 0");//1 + 3 = 4

    trainingQAs[i++]->Set("1, 1, 0, 1", "0, 0, 0, 0, 1, 0");//3 + 2 = 5

    trainingQAs[i++]->Set("0, 1, 1, 1", "0, 0, 0, 0, 1, 0");//2 + 3 = 5
    trainingQAs[i++]->Set("1, 1, 1, 1", "0, 0, 0, 0, 0, 1");//3 + 3 = 6
    */


    /*
    trainingQAs[i++]->Set("1,0,0,0,0,0","1,0,0,0,0,0");//1 = 1
    trainingQAs[i++]->Set("0,1,0,0,0,0","0,1,0,0,0,0");//1 = 2
    trainingQAs[i++]->Set("0,0,1,0,0,0","0,0,1,0,0,0");//2 = 3


    trainingQAs[i++]->Set("1,0,0,1,0,0","0,1,0,0,0,0");//1 + 1 = 2

    trainingQAs[i++]->Set("1,0,0,0,1,0","0,0,1,0,0,0");//1 + 2 = 3
    trainingQAs[i++]->Set("1,0,0,0,0,1","0,0,0,1,0,0");//1 + 3 = 4

    trainingQAs[i++]->Set("0,1,0,1,0,0","0,0,1,0,0,0");//2 + 2 = 3
    trainingQAs[i++]->Set("0,1,0,0,1,0","0,0,0,1,0,0");//2 + 2 = 4
    trainingQAs[i++]->Set("0,1,0,0,0,1","0,0,0,0,1,0");//2 + 3 = 5

    trainingQAs[i++]->Set("0,0,1,1,0,0","0,0,0,1,0,0");//3 + 1 = 4

    trainingQAs[i++]->Set("0,0,1,0,1,0","0,0,0,0,1,0");//3 + 2 = 5

    trainingQAs[i++]->Set("0,0,1,0,0,1","0,0,0,0,0,1");//3 + 3 = 6
    */
};

TrainingQA* TrainingSet::GetNextQA()
{
    currentQAIndex++;

    if (currentQAIndex >= currentSubSetSize)
        currentQAIndex = 0;

    return trainingQAs[currentQAIndex];
};

TrainingSet::~TrainingSet()
{

};


//double TrainingSet::stimulusDuration = 0.1;
//double TrainingSet::stimulusInterval = 3;

////double TrainingSet::stimulusDuration = 0.1;
////double TrainingSet::stimulusInterval = 10;

////double TrainingSet::stimulusDuration = 0.3;
////double TrainingSet::stimulusInterval = 10;
double TrainingSet::stimulusDurationHH = 0.1;
////double TrainingSet::stimulusInterval = 5;
double TrainingSet::stimulusIntervalHH = 30;
////double TrainingSet::stimulusInterval = 60;
////double TrainingSet::stimulusInterval = 100;
//double TrainingSet::stimulusInterval = 1;

double TrainingSet::preStimulusDuration = 10;

////double TrainingSet::InitializationInterval = 30;
double TrainingSet::InitializationInterval = 10;

////double TrainingSet::stimulusDuration = 1;
////double TrainingSet::stimulusInterval = 0;

double TrainingSet::stimulusDuration = 0.1;
double TrainingSet::stimulusInterval = 0.1;

////double TrainingSet::stimulusDuration = 1;
////double TrainingSet::stimulusInterval = 1;

//double TrainingSet::nonHHStimulusDuration = 0.3;
//double TrainingSet::nonHHstimulusInterval = 0.3;

//double TrainingSet::nonHHStimulusDuration = 1;
//double TrainingSet::nonHHstimulusInterval = 3;

double TrainingSet::stimulusDurationEvolved = 1;
double TrainingSet::stimulusIntervalEvolved = 1;

char TrainingSet::inputNullStr[255];

#ifdef TRAININGSETVISION
uint32_t TrainingSet::qLength = 8;
uint32_t TrainingSet::resultLength = 3;
#endif // TRAININGSET8TO3

#ifdef TRAININGSET8TO8
uint32_t TrainingSet::qLength = 8;
uint32_t TrainingSet::resultLength = 8;
#endif // TRAININGSET8TO3

#ifdef TRAININGSET0
uint32_t TrainingSet::qLength = 1;
uint32_t TrainingSet::resultLength = 1;
#endif // TRAININGSET0

#ifdef TRAININGSET3TO8
uint32_t TrainingSet::qLength = 3;
uint32_t TrainingSet::resultLength = 8;
#endif // TRAININGSET3TO8

#ifdef TRAININGSET8TO3
uint32_t TrainingSet::qLength = 8;
uint32_t TrainingSet::resultLength = 3;
#endif // TRAININGSET8TO3

////uint32_t TrainingSet::resultLength = 12;
////uint32_t TrainingSet::resultLength = 8;

////uint32_t TrainingSet::maxSubsetSize = 27;
///uint32_t TrainingSet::maxSubsetSize = 8;
////uint32_t TrainingSet::maxSubsetSize = 2;
uint32_t TrainingSet::maxSubsetSize = 6;
