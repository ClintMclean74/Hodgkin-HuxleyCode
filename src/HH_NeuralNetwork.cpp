#include <string.h>
#include "GL/glew.h"
#include "HH_NeuralNetwork.h"
#include "HH_NeuralNetworks.h"
#include "MathUtilities.h"



HH_NeuralNetwork::HH_NeuralNetwork(uint32_t layers, uint32_t* rowCountsForColumns, Vector* pos)
{
    this->layers = layers;
    this->rowCountsForColumns = rowCountsForColumns;

    if (pos != NULL)
    {
        this->pos = *pos;
    }

    //weightsMutationRateMutationRate = 0.00000000000;
    //weightsMutationRateMutationRate = 0.000025;
    ////weightsMutationRateMutationRate = 0.00025;
    weightsMutationRateMutationRate = 0.0025;
    //weightsMutationRate = 0.35;
    //weightsMutationRate = 0.00025;
    ////original weightsMutationRate = 0.25;
    weightsMutationRate = 1.0;
    //weightsMutationRate = 0.0;
    ////weightsMutationRate = 10.0;

    //weightsScale = 1;

    ////weightsScale = 0.001;
    //weightsScale = 0.0;

    ////weightsScale = 0.0035;
    ////weightsScale = 0.01;
    ////weightsScale = 0.003;
    ////original weightsScale = 0.0025; //works for 30 x 9_30

    weightsScale = 0.0050;

    rotationMutationRate = 1;

    velocityMutationRate = 1;

    neurons = new HH_Neurons(layers, rowCountsForColumns, this);

    if (!HH_NeuralNetworks::NEURAL_NOISE)
        HH_NeuralNetworks::NEURAL_NOISE = new double[neurons->count];
}

HH_NeuralNetwork* HH_NeuralNetwork::Load(char* fileName)
{
    FILE* file = fopen(fileName, "r+");

    char textBuffer[255];

    fgets(textBuffer, 255, file);
    uint32_t layers = atoi(textBuffer);

    uint32_t* rowCountsForColumns = new uint32_t[layers];

    for(uint32_t i = 0; i<layers; i++)
    {
        fgets(textBuffer, 255, file);

        rowCountsForColumns[i] = atoi(textBuffer);
    }

    HH_NeuralNetwork* neuralNetwork = new HH_NeuralNetwork(layers, rowCountsForColumns);

    neuralNetwork->neurons->Load(file);

    fgets(textBuffer, 255, file);
    neuralNetwork->weightsScale = atof(textBuffer);

    fclose(file);

    return neuralNetwork;
}

void HH_NeuralNetwork::AddLayersFromNN(HH_NeuralNetwork* network)
{
    uint32_t additionalLayers = network->layers-1;

    uint32_t* newRows = new uint32_t[layers + additionalLayers];

    for(uint32_t i = 0; i<layers; i++)
    {
        newRows[i] = rowCountsForColumns[i];
    }

    uint32_t j = 1;
    for(uint32_t i = layers; i < layers + additionalLayers; i++)
    {
        newRows[i] = network->rowCountsForColumns[j];

        j++;
    }

    layers = layers + additionalLayers;

    delete rowCountsForColumns;
    rowCountsForColumns = newRows;
    neurons->rowCountsForColumns = newRows;

    neurons->AddLayersFromNN(network);
}

void HH_NeuralNetwork::JoinLayer(uint32_t startIndex, uint32_t endIndex)
{
    recurrent = true;
    neurons->JoinLayer(startIndex, endIndex);
}

void HH_NeuralNetwork::Save(char* fileName)
{
    FILE* file = fopen(fileName, "w+");

    neurons->Save(file);

    char textBuffer[255];

    sprintf(textBuffer, "%f\n", weightsScale);

    uint32_t length = strlen(textBuffer);
    fwrite(textBuffer, sizeof(char), length, file);

    fclose(file);
}

void HH_NeuralNetwork::Copy(HH_NeuralNetwork* neuralNetwork)
{
    agent->rotationDelta = neuralNetwork->agent->rotationDelta;
    agent->velocity = neuralNetwork->agent->velocity;

    weightsMutationRate = neuralNetwork->weightsMutationRate;
    weightsScale = neuralNetwork->weightsScale;

    neurons->Copy(neuralNetwork->neurons);
}

void HH_NeuralNetwork::Mutate(uint32_t startLayerIndex, uint32_t endLayerIndex)
{
    if (!HH_NeuralNetworks::simHH)
    {
        double value;

        /*////value = (double) rand()/ RAND_MAX * rotationMutationRate - rotationMutationRate/2;
        agent->rotationDelta += value;

        value = (double) rand()/ RAND_MAX * velocityMutationRate - velocityMutationRate/2;
        agent->velocity += value;
        */

        value = (double) rand()/ RAND_MAX * weightsMutationRateMutationRate - weightsMutationRateMutationRate/2;
        weightsMutationRate += value;

        if (layers > 2)
        {
            startLayerIndex = layers - 2;
            endLayerIndex = layers - 1;
        }

        neurons->Mutate(weightsMutationRate, startLayerIndex, endLayerIndex);
    }
    else
    {
        double value;

        /*////
        value = (double) rand()/ RAND_MAX * rotationMutationRate - rotationMutationRate/2;
        agent->rotationDelta += value;

        value = (double) rand()/ RAND_MAX * velocityMutationRate - velocityMutationRate/2;
        agent->velocity += value;
        */

        value = (double) rand()/ RAND_MAX * weightsMutationRateMutationRate - weightsMutationRateMutationRate/2;
        weightsMutationRate += value;

        neurons->Mutate(weightsMutationRate, startLayerIndex, endLayerIndex);

        value = (double) rand()/ RAND_MAX * weightsMutationRate - weightsMutationRate/2;

        value/=1000;

        //weightsScale += value*1000;
        weightsScale += value;

        //weightsScale -= 0.001;
        //weightsScale = 0;

        //neurons->MutateScale(weightsMutationRate*1000);
    }
}

/*void HH_NeuralNetwork::SetTemperature(double temperature)
{
    this->temperature = temperature;

    neurons->SetTemperature(temperature);
}

void HH_NeuralNetwork::SetTemperature(double temperature, uint32_t column)
{
    //this->temperature = temperature;

    neurons->SetTemperature(temperature, column);
}*/

void HH_NeuralNetwork::SetTemperature(double temperature, int32_t column, int32_t row)
{
    this->temperature = temperature;

    neurons->SetTemperature(temperature, column, row);
}

void HH_NeuralNetwork::SetNoise(double noise, int32_t column)
{
    //this->temperature = temperature;

    neurons->SetNoise(noise, column);
}

void HH_NeuralNetwork::SetSynStimulus(double stimulus)
{
    neurons->SetSynStimulus(stimulus);
}

void HH_NeuralNetwork::SetSynStimulus(double stimulus, uint32_t column, uint32_t row)
{
    neurons->SetSynStimulus(stimulus, column, row);
}

void HH_NeuralNetwork::SetQ(char* q)
{
    neurons->SetQ(q);

}


void HH_NeuralNetwork::Reinforce(double value, bool usedConnections)
{
    neurons->Reinforce(value, usedConnections);
}

void HH_NeuralNetwork::Process()
{
    ////Action action;

    neurons->Process();

    /*////
    signal = 0;

    uint32_t signalCountsForEachThirdOfResultLayer[3];

    signalCountsForEachThirdOfResultLayer[0] = 3;
    signalCountsForEachThirdOfResultLayer[1] = 2;
    signalCountsForEachThirdOfResultLayer[2] = 1;

    neurons->GetSignalCountForEachThirdOfResultLayer(signalCountsForEachThirdOfResultLayer);
    */

    /*////
    action = neurons->GetResultAction();

    switch (action)
    {
        case (Action::RotateRight):
            agent->Rotate(-agent->rotationDelta);
        break;
        case (Action::RotateLeft):
            agent->Rotate(-agent->rotationDelta);
        break;
        case (Action::Forward):
            agent->Forward();
        break;
    }
    */

    /*////
    ////if (neurons->neurons[layers-1][0].membraneVoltage>HH_Neuron::MEMBRANE_VOLTAGE_THRESHOLD)
    if (signalCountsForEachThirdOfResultLayer[0] > signalCountsForEachThirdOfResultLayer[1] && signalCountsForEachThirdOfResultLayer[0] > signalCountsForEachThirdOfResultLayer[2])
    {
        signal++;
        agent->Rotate(agent->rotationDelta);
    }
    else
    ////if (neurons->neurons[layers-1][1].membraneVoltage>HH_Neuron::MEMBRANE_VOLTAGE_THRESHOLD)
    if (signalCountsForEachThirdOfResultLayer[1] > signalCountsForEachThirdOfResultLayer[0] && signalCountsForEachThirdOfResultLayer[1] > signalCountsForEachThirdOfResultLayer[2])
    {
        signal++;
        agent->Forward();
    }
    else
    ////if (neurons->neurons[layers-1][2].membraneVoltage>HH_Neuron::MEMBRANE_VOLTAGE_THRESHOLD)
    if (signalCountsForEachThirdOfResultLayer[2] > signalCountsForEachThirdOfResultLayer[0] && signalCountsForEachThirdOfResultLayer[2] > signalCountsForEachThirdOfResultLayer[1])
    {
        signal++;
        agent->Rotate(-agent->rotationDelta);
    }
    else
    ////if (signal > 1)
        agent->fitness -= 10;
        */

    /*////
    if (neurons->neurons[layers-1][0].membraneVoltage>HH_Neuron::MEMBRANE_VOLTAGE_THRESHOLD)
        agent->Rotate(agent->rotationDelta);

    if (neurons->neurons[layers-1][1].membraneVoltage>HH_Neuron::MEMBRANE_VOLTAGE_THRESHOLD)
        agent->Forward();

    if (neurons->neurons[layers-1][2].membraneVoltage>HH_Neuron::MEMBRANE_VOLTAGE_THRESHOLD)
        agent->Rotate(-agent->rotationDelta);
        */



    /*////agent->velocity = 0.1;

    agent->Forward();

    agent->velocity = 1;
    */
}

void HH_NeuralNetwork::Draw()
{
    glPushMatrix();
    glTranslatef(pos.x, pos.y, pos.z);

    neurons->Draw();

    glPopMatrix();
}

void HH_NeuralNetwork::ResetHH()
{
    neurons->ResetHH();
}

void HH_NeuralNetwork::ResetSpikeCounts()
{
    neurons->ResetSpikeCounts();
}

HH_NeuralNetwork::~HH_NeuralNetwork()
{
    delete neurons;
}

double HH_NeuralNetwork::Layer1ActiveSynapticConductanceHH = 0.001;
double HH_NeuralNetwork::Layer1ActiveSynapticConductance = 0.2;

double HH_NeuralNetwork::WIDTH = 400;
double HH_NeuralNetwork::HEIGHT = 400;
double HH_NeuralNetwork::X_GAP = 120000;
////double HH_NeuralNetwork::X_GAP = 20000;
////double HH_NeuralNetwork::Y_GAP = 6000;
double HH_NeuralNetwork::Y_GAP = 20000;

//uint32_t* HH_NeuralNetwork::ROWS = new uint32_t[HH_NeuralNetwork::LAYERS];
uint32_t* HH_NeuralNetwork::ROWS = NULL;

////uint32_t HH_NeuralNetwork::LAYERS = 60;
uint32_t HH_NeuralNetwork::LAYERS = 30;
////uint32_t HH_NeuralNetwork::LAYERS = 20;
////uint32_t HH_NeuralNetwork::LAYERS = 10;
////uint32_t HH_NeuralNetwork::LAYERS = 6;
////uint32_t HH_NeuralNetwork::LAYERS = 3;
////uint32_t HH_NeuralNetwork::LAYERS = 2;

uint32_t HH_NeuralNetwork::layer1Length = 9;


uint32_t HH_NeuralNetwork::hiddenLayerLength = 30;
uint32_t HH_NeuralNetwork::resultLayerLength = 30;



/*////
uint32_t HH_NeuralNetwork::hiddenLayerLength = 10;
uint32_t HH_NeuralNetwork::resultLayerLength = 10;
*/








double HH_NeuralNetwork::rewardDecayRate = 1;
double HH_NeuralNetwork::weightActivityWeakenRate = 0.001;
double HH_NeuralNetwork::weightRegenerationRate = 0.00001;
double HH_NeuralNetwork::stimIntervalTimeFromStimStart = 70; //works for 30 x 9_30
////double HH_NeuralNetwork::stimIntervalTimeFromStimStart = 700;
////double HH_NeuralNetwork::stimIntervalTimeFromStimStart = 10;


double HH_NeuralNetwork::stimDuration = 0.2;

uint32_t HH_NeuralNetwork::MAX_LAYER_COLUMN_WIDTH = 15;
