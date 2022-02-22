#include <string.h>
#include <stdlib.h>
#include "NN_Connection.h"

NN_Connection::NN_Connection(uint32_t column, uint32_t row, double synapseConductance)
{
    this->column = column;
    this->row = row;

    this->synapseConductance = synapseConductance;

    neuralConnectionActivityTime = 0;
}

void NN_Connection::Load(FILE* file)
{
    char textBuffer[255];
    memset(textBuffer, 0, 255);

    fgets(textBuffer, 255, file);

    char *ptr = strtok(textBuffer, ",");

    column = atoi(ptr);
    ptr = strtok(NULL, ",");

    row = atoi(ptr);
    ptr = strtok(NULL, ",");

    synapseConductance = atof(ptr);

    /* while (ptr != NULL)
    {
        ptr = strtok (NULL, ",");
    }*/
}

void NN_Connection::Save(FILE *file)
{
    char textBuffer[255];

    sprintf(textBuffer, "%d,%d,%f\n", column, row, synapseConductance);

    uint32_t length = strlen(textBuffer);
    fwrite(textBuffer, sizeof(char), length, file);
}
