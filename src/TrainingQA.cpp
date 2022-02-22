#include <cmath>
#include <string.h>
#include <iostream>

#include "TrainingQA.h"

TrainingQA::TrainingQA(uint32_t layer1Size, uint32_t resultLayerSize)
{
    this->layer1Size = layer1Size;
    this->resultLayerSize = resultLayerSize;

    /*
    q = new char[layer1Size*3+1];
    a = new char[resultLayerSize*3+1];
    */

    q = new char[255];
    a = new char[255];

    uint8_t j = 0;

    for (int i=0; i<layer1Size; i++)
    {
        //if (j>0)
            //q[j++] = ',';

        q[j++] = '0';
    }

    q[j++] = 0;

    j = 0;

    for (int i=0; i<resultLayerSize; i++)
    {
        //if (j>0)
            //a[j++] = ',';

        a[j++] = '0';
    }

    a[j++] = 0;


    //strcpy(q, "0,0,0,0");
    //strcpy(a, "0,0,0,0");

    /*for (uint8_t i = 0; i<size; i++)
    {
        q[i] = '0';
        a[i] = '0';
    }*/
};

void TrainingQA::Initialize()
{
    double value;
    for(uint32_t i = 0; i < layer1Size; i++)
    {
        //q[i] = (uint8_t) ((double) rand()/ RAND_MAX * 1.0 + 0.5);
        value = (double) rand()/ RAND_MAX * 1.0 + 0.5;

        if (value >= 1)
            q[i] = '1';
        else
            q[i] = '0';

        /*
        value = (double) rand()/ RAND_MAX * 1.0 + 0.5;

        if (value >= 1)
            a[i] = '1';
        else
            a[i] = '0';
            */
    }

    for(uint32_t i = 0; i < resultLayerSize; i++)
    {
        //q[i] = (uint8_t) ((double) rand()/ RAND_MAX * 1.0 + 0.5);
        value = (double) rand()/ RAND_MAX * 1.0 + 0.5;

        if (value >= 1)
            a[i] = '1';
        else
            a[i] = '0';

    }
}

void TrainingQA::Set(const char q[], const char a[])
{
    strcpy(this->q, q);
    strcpy(this->a, a);
}

double TrainingQA::CheckQ(uint32_t* qLayerSignalsBuffer)
{
    double error = 0;

    uint32_t length = strlen(q);

    uint32_t j = 0;
    for(uint32_t i = 0; i < length; i++)
    {
        if (qLayerSignalsBuffer[j]==1)
        {
            int grc = 1;
        }

        if (q[i]=='1' || q[i]=='0')
        {
            if (!((qLayerSignalsBuffer[j] == 1 && q[i]=='1') || (qLayerSignalsBuffer[j] == 0 && q[i]=='0')))
            {
                error += 1;
            }

            j++;
        }
    }

    return error;
}

double TrainingQA::CheckA(uint32_t* resultLayerSignalsBuffer, uint32_t resultLength, bool print)
{
    double error = 0;

    uint32_t length = strlen(a);

    uint32_t j = 0;
    for(uint32_t i = 0; i < length; i++)
    {
        if (resultLayerSignalsBuffer[j]==1)
        {
            int grc = 1;
        }

        if (a[i]=='1' || a[i]=='0')
        {
            if (a[i]=='1')
                error += abs(resultLayerSignalsBuffer[j] - 1);
            else
                if (a[i]=='0')
                    error += resultLayerSignalsBuffer[j];

            /*if (!((resultLayerSignalsBuffer[j] == 1 && a[i]=='1') || (resultLayerSignalsBuffer[j] == 0 && a[i]=='0')))
            {
                error += 1;
            }*/

            j++;

            if (j == resultLength)
                break;
        }
    }

    /*if (strcmp(q, "0,0,0,0,0,0") != 0)
    {
        char text[255];
        sprintf(text, "Answer: %s", a);
        std::cout << text<< "\n";
        sprintf(text, "Result: %i,%i,%i,%i,%i,%i", resultLayerSignalsBuffer[0], resultLayerSignalsBuffer[1], resultLayerSignalsBuffer[2], resultLayerSignalsBuffer[3], resultLayerSignalsBuffer[4], resultLayerSignalsBuffer[5]);
        std::cout << text<< "\n";
        sprintf(text, "Error: %f", error);
        std::cout << text<< "\n\n";
    }
    */

    return error;
}

TrainingQA::~TrainingQA()
{

};
