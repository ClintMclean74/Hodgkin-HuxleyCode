#include <string.h>
#include <sysinfoapi.h>

#include "GL/glew.h"
#include "GraphicsUtilities.h"
#include "HH_Neurons.h"
#include "HH_NeuralNetwork.h"
#include "HH_NeuralNetworks.h"
#include "Simulation.h"

HH_Neurons::HH_Neurons(uint32_t layers, uint32_t* rowCountsForColumns, void* neuralNetwork)
{
    ////voltagesFile = fopen("nnVoltages.txt", "w");

    this->layers = layers;
    ////this->rowCountsForColumns = rowCountsForColumns;
    this->rowCountsForColumns = new uint32_t[HH_NeuralNetwork::MAX_LAYERS];

    for (uint32_t i=0; i<layers; i++)
    {
        this->rowCountsForColumns[i] = rowCountsForColumns[i];
    }

    this->neuralNetwork = neuralNetwork;

    neurons = new HH_Neuron_ptr[HH_NeuralNetwork::MAX_LAYERS];

    resultsBuffers = new ResultsBuffer_ptr[HH_NeuralNetwork::MAX_LAYERS];

    count = 0;
    for (uint32_t i=0; i<layers; i++)
    {
        neurons[i] = new HH_Neuron[rowCountsForColumns[i]];

        resultsBuffers[i] = new ResultsBuffer(rowCountsForColumns[i], rowCountsForColumns[i] * 10);

        count+=rowCountsForColumns[i];
    }

    for (uint32_t i=layers; i<HH_NeuralNetwork::MAX_LAYERS; i++)
    {
        resultsBuffers[i] = NULL;
    }

    for (uint32_t i=0; i<layers; i++)
    {
        for (uint32_t j=0; j<rowCountsForColumns[i]; j++)
            neurons[i][j].neuralNetwork = neuralNetwork;
    }

    double rndWeight;
    double rndWeightScale = (10.0 / rowCountsForColumns[1]) * 2.93;

    for (uint32_t i=1; i<layers; i++)
    {
        for (uint32_t j=0; j<rowCountsForColumns[i]; j++)
        {
            for (uint32_t k=0; k<rowCountsForColumns[i-1]; k++)
            {
                rndWeight = (double) rand()/ RAND_MAX * 2 - 1;
                ////rndWeight = (double) rand()/ RAND_MAX  - 0.5;
                ////rndWeight *= 0.01; //scale the weights so that they don't overload the network
                ////rndWeight *= 0.09999999999999999999; //scale the weights so that they don't overload the network
                ////rndWeight *= 0.0001; //scale the weights so that they don't overload the network
                rndWeight *= 0.2; //scale the weights so that they don't overload the network

                ////rndWeight = 0;
                neurons[i][j].connections.Add(i-1, k, rndWeight);
                ////neurons[i][j].connections.Add(i-1, k, 0);
            }
        }

    }

    double temperature = 37;

    pos.x = 0;

    Vector pos;
    for (uint32_t i=0; i<layers; i++)
    {
        for (uint32_t j=0; j<rowCountsForColumns[i]; j++)
        {
            pos = GetNewGraphXY(i, j);
            neurons[i][j].SetPos(pos.x, pos.y, 0.0);
        }

    }

    graphColor = new Color(0, 1, 0);

    currentQ[0] = 0;

    mostRecentStimulus[0] = 0;
}

void HH_Neurons::Load(FILE* file)
{
    for (uint32_t i=0; i<layers; i++)
    {
        for (uint32_t j=0; j<rowCountsForColumns[i]; j++)
        {
            neurons[i][j].Load(file);
        }
    }

    char textBuffer[255];
    fgets(textBuffer, 255, file);
    uint32_t resultsBuffersCount = atoi(textBuffer);

    ////delete resultsBuffers;
    resultsBuffers = new ResultsBuffer_ptr[resultsBuffersCount];

    uint32_t resultsBufferLength;
    for (uint32_t i=0; i<layers; i++)
    {
        ////delete resultsBuffers[i];

        fgets(textBuffer, 255, file);
        resultsBufferLength = atoi(textBuffer);

        resultsBuffers[i] = new ResultsBuffer(rowCountsForColumns[i], rowCountsForColumns[i] * 10);
        resultsBuffers[i]->currentIndex = resultsBufferLength;

        resultsBuffers[i]->Load(file);
    }

}

void HH_Neurons::AddLayersFromNN(void* network)
{
    HH_NeuralNetwork* networkPtr = (HH_NeuralNetwork*) network;

    uint32_t k = 1;
    for (uint32_t i = layers; i < layers + networkPtr->layers-1; i++)
    {
        neurons[i] = new HH_Neuron[rowCountsForColumns[k]];

        for (uint32_t j=0; j<rowCountsForColumns[k]; j++)
        {
            neurons[i][j].CopyWeights(&networkPtr->neurons->neurons[k][j], 0, layers - 1);
        }

        for (uint32_t j=0; j<rowCountsForColumns[k]; j++)
            neurons[i][j].neuralNetwork = neuralNetwork;
    }

    layers = layers + networkPtr->layers-1;

    Vector pos;
    for (uint32_t i=0; i<layers; i++)
    {
        for (uint32_t j=0; j<rowCountsForColumns[i]; j++)
        {
            pos = GetNewGraphXY(i, j);
            neurons[i][j].SetPos(pos.x, pos.y, 0.0);
        }
    }
}

/*void HH_Neurons::AddNNHiddenLayer(void* network)
{
    HH_NeuralNetwork* networkPtr = (HH_NeuralNetwork*) network;

    uint32_t prevLastLayerIndex = 1;

    //first create a new layer for the previous last layer
    neurons[2] = new HH_Neuron[3];

    //then copy the previous last layer's weights into the new layer
    for (uint32_t j=0; j<3; j++)
    {
        neurons[2][j].CopyWeights(&networkPtr->neurons->neurons[1][j], 0, 1);
    }

    //delete the previous last layer
    delete [] neurons[1];

    //now create the additional layer's neurons
    ////neurons[prevLastLayerIndex] = new HH_Neuron[rowCountsForColumns[prevLastLayerIndex-1]];
    neurons[1] = new HH_Neuron[8];

    double rndWeight;
    for (uint32_t j=0; j<3; j++)
        {
            for (uint32_t k=0; k<8; k++)
            {
                rndWeight = (double) rand()/ RAND_MAX * 2 - 1;
                rndWeight *= 0.2; //scale the weights so that they don't overload the network
                neurons[1][j].connections.Add(0, k, rndWeight);
            }
        }

    ////layers++;

    //copy the last layer row count
    rowCountsForColumns[2] = rowCountsForColumns[1];

    //insert the new hidden layer row count
    rowCountsForColumns[1] = rowCountsForColumns[0];

    Vector pos;
    for (uint32_t i=0; i<layers; i++)
    {
        for (uint32_t j=0; j<rowCountsForColumns[i]; j++)
        {
            pos = GetNewGraphXY(i, j);
            neurons[i][j].SetPos(pos.x, pos.y, 0.0);
        }
    }

    for (uint32_t i=0; i<layers; i++)
    {
        for (uint32_t j=0; j<rowCountsForColumns[i]; j++)
            neurons[i][j].neuralNetwork = neuralNetwork;
    }
}
*/

void HH_Neurons::AddNNHiddenLayer(void* network)
{
    HH_NeuralNetwork* networkPtr = (HH_NeuralNetwork*) network;

    uint32_t prevLastLayerIndex = layers - 1;

    //first create a new layer for the previous last layer
    neurons[layers] = new HH_Neuron[rowCountsForColumns[prevLastLayerIndex]];

    //then copy the previous last layer's weights into the new layer
    for (uint32_t j=0; j<rowCountsForColumns[prevLastLayerIndex]; j++)
    {
        neurons[layers][j].CopyWeights(&networkPtr->neurons->neurons[prevLastLayerIndex][j], prevLastLayerIndex-1, prevLastLayerIndex);
    }

    //delete the previous last layer
    delete [] neurons[prevLastLayerIndex];

    //now create the additional layer's neurons
    neurons[prevLastLayerIndex] = new HH_Neuron[rowCountsForColumns[prevLastLayerIndex-1]];

    //copy the weights to the second layer for the new layer if the original layers > 2
    if (layers > 2)
    {
        /*original for (uint32_t j=0; j<rowCountsForColumns[1]; j++)
        {
            neurons[prevLastLayerIndex][j].CopyWeights(&networkPtr->neurons->neurons[1][j], 0, prevLastLayerIndex-1);
        }*/

        for (uint32_t j=0; j<rowCountsForColumns[prevLastLayerIndex-1]; j++)
        {
            neurons[prevLastLayerIndex][j].CopyWeights(&networkPtr->neurons->neurons[prevLastLayerIndex-1][j], prevLastLayerIndex-2, prevLastLayerIndex-1);
        }
    }
    else //randomize the weights
    {
    /*for (uint32_t j=0; j<rowCountsForColumns[prevLastLayerIndex]; j++)
    {
        neurons[layers][j].CopyWeights(&networkPtr->neurons->neurons[prevLastLayerIndex][j], prevLastLayerIndex-1, prevLastLayerIndex);
    }*/

    double rndWeight;
    for (uint32_t j=0; j<rowCountsForColumns[prevLastLayerIndex-1]; j++)
        {
            for (uint32_t k=0; k<rowCountsForColumns[prevLastLayerIndex-1]; k++)
            {
                rndWeight = (double) rand()/ RAND_MAX * 2 - 1;
                rndWeight *= 0.2; //scale the weights so that they don't overload the network
                neurons[prevLastLayerIndex][j].connections.Add(prevLastLayerIndex-1, k, rndWeight);
            }
        }
    }

    layers++;
    networkPtr->layers = layers;

    //copy the last layer row count
    rowCountsForColumns[layers-1] = rowCountsForColumns[prevLastLayerIndex];

    //insert the new hidden layer row count
    rowCountsForColumns[layers-2] = rowCountsForColumns[prevLastLayerIndex-1];

    Vector pos;
    for (uint32_t i=0; i<layers; i++)
    {
        for (uint32_t j=0; j<rowCountsForColumns[i]; j++)
        {
            pos = GetNewGraphXY(i, j);
            neurons[i][j].SetPos(pos.x, pos.y, 0.0);
        }
    }

    for (uint32_t i=0; i<layers; i++)
    {
        for (uint32_t j=0; j<rowCountsForColumns[i]; j++)
            neurons[i][j].neuralNetwork = neuralNetwork;
    }

    resultsBuffers[layers-1] = new ResultsBuffer(rowCountsForColumns[layers-1], rowCountsForColumns[layers-1] * 10);
}

void HH_Neurons::JoinLayer(uint32_t startIndex, uint32_t endIndex)
{
    for (uint32_t j=0; j<rowCountsForColumns[startIndex]; j++)
    {
        ////neurons[0][j].connections.Add(layers-1, j, 1);
        neurons[startIndex][j].connections.Add(endIndex, j, 1);
    }
}

void HH_Neurons::Save(FILE* file)
{
    char textBuffer[255];
    sprintf(textBuffer, "%d\n", layers);

    uint32_t length = strlen(textBuffer);
    fwrite(textBuffer, sizeof(char), length, file);

    for (uint32_t i=0; i<layers; i++)
    {
        sprintf(textBuffer, "%d\n", rowCountsForColumns[i]);
        length = strlen(textBuffer);
        fwrite(textBuffer, sizeof(char), length, file);
    }

    for (uint32_t i=0; i<layers; i++)
    {
        for (uint32_t j=0; j<rowCountsForColumns[i]; j++)
        {
            neurons[i][j].Save(file);
        }
    }

    sprintf(textBuffer, "%d\n", layers);
    fwrite(textBuffer, sizeof(char), strlen(textBuffer), file);

    for (uint32_t i=0; i<layers; i++)
    {
        resultsBuffers[i]->Save(file);
    }
}

void HH_Neurons::Copy(HH_Neurons* srcNeurons)
{
    ////CopyResultsBuffers(srcNeurons->resultsBuffers);
    ClearResultsBuffers();
    for (uint32_t i=0; i<layers; i++)
    {
        for (uint32_t j=0; j<rowCountsForColumns[i]; j++)
        {
            neurons[i][j].CopyWeights(&srcNeurons->neurons[i][j]);
        }
    }
}

void HH_Neurons::Mutate(double rate, uint32_t startLayerIndex, uint32_t endLayerIndex)
{
    if (startLayerIndex == HH_NeuralNetwork::MAX_LAYERS && endLayerIndex == HH_NeuralNetwork::MAX_LAYERS)
    {
    for (uint32_t i=0; i<layers; i++)
    {
        for (uint32_t j=0; j<rowCountsForColumns[i]; j++)
        {
            neurons[i][j].Mutate(rate);
        }
    }
    }
    else
    {
        if (startLayerIndex == HH_NeuralNetwork::MAX_LAYERS)
            startLayerIndex = 0;

        if (endLayerIndex == HH_NeuralNetwork::MAX_LAYERS)
            endLayerIndex = layers-1;

        for (uint32_t i=startLayerIndex; i<=endLayerIndex; i++)
        {
            for (uint32_t j=0; j<rowCountsForColumns[i]; j++)
            {
                neurons[i][j].Mutate(rate);
            }
        }
    }

    /*for (uint32_t i=0; i<size;i++)
    {
        neurons[i].Mutate(rate);
    }*/
}

/*void HH_Neurons::MutateScale(double rate)
{
    for (uint32_t i=0; i<size;i++)
    {
        neurons[i].MutateScale(rate);
    }
}*/

/*
void HH_Neurons::SetTemperature(double temperature)
{
    for (uint32_t i=0; i<layers; i++)
    {
        for (uint32_t j=0; j<rowCountsForColumns[i]; j++)
        {
            neurons[i][j].SetTemperature(temperature);
        }
    }

    /*for (uint32_t i=0; i<size;i++)
    {
        neurons[i].SetTemperature(temperature);
    }*/
/*}

void HH_Neurons::SetTemperature(double temperature, uint32_t column)
{
    for (uint32_t j=0; j<rowCountsForColumns[column]; j++)
        neurons[column][j].SetTemperature(temperature);
}
*/

void HH_Neurons::SetTemperature(double temperature, int32_t column, int32_t row)
{
    if (column == -1)
    {
        for (uint32_t i=0; i<layers; i++)
        {
            for (uint32_t j=0; j<rowCountsForColumns[i]; j++)
            {
                neurons[i][j].SetTemperature(temperature);
            }
        }
    }
    else
    {
        if (row == -1)
        {
            for (uint32_t j=0; j<rowCountsForColumns[column]; j++)
                neurons[column][j].SetTemperature(temperature);
        }
        else
            neurons[column][row].SetTemperature(temperature);
    }
}

void HH_Neurons::SetNoise(double noise, int32_t column)
{
    if (column == -1)
    {
        for (uint32_t i=0; i<layers; i++)
        {
            for (uint32_t j=0; j<rowCountsForColumns[i]; j++)
            {
                neurons[i][j].SetNoise(noise);
            }
        }
    }
    else
    {
        for (uint32_t j=0; j<rowCountsForColumns[column]; j++)
            neurons[column][j].SetNoise(noise);
    }
}

void HH_Neurons::SetSynStimulus(double stimulus)
{
    for (uint32_t i=0; i<layers; i++)
    {
        for (uint32_t j=0; j<rowCountsForColumns[i]; j++)
        {
            neurons[i][j].SetSynStimulus(stimulus);
        }
    }

    /*for (uint32_t i=0; i<size;i++)
    {
        neurons[i].SetSynStimulus(stimulus);
    }*/
}

void HH_Neurons::SetSynStimulus(double stimulus, int32_t column, int32_t row)
{
    neurons[column][row].SetSynStimulus(stimulus);
    //neurons[index].SetSynStimulus(stimulus);
}

void HH_Neurons::SetQ(char* q)
{
    strcpy(currentQ, q);
    uint32_t qLength = strlen(q);
    uint32_t n=0;

    bool stimulus = false;
    for (uint32_t i=0; i<qLength;i++)
    {
        if (q[i] == '1')
        {
            stimulus = true;

            if (HH_NeuralNetworks::simHH)
                neurons[0][n].setStimulus = 0.01;
            else
                neurons[0][n].setConductance = HH_NeuralNetwork::Layer1ActiveSynapticConductance;
        }
        else
            if (q[i] == '0')
            {
                neurons[0][n].setStimulus = 0;
                neurons[0][n].setConductance = 0.0;
            }

        n++;

        if (n>=rowCountsForColumns[0])
            break;
    }

    if (stimulus)
        ////strcpy(mostRecentStimulus, q);
        strcpy(mostRecentStimulus, currentQ);
}

void HH_Neurons::Reinforce(double value, bool usedConnections)
{
    for (uint32_t i=0; i<layers; i++)
    {
        for (uint32_t j=0; j<rowCountsForColumns[i]; j++)
            neurons[i][j].Reinforce(value, usedConnections);
    }
}

//void HH_Neurons::Process(double stimulus, double Simulation::deltaTime, uint32_t startStimulus, uint32_t endStimulus)
void HH_Neurons::Process()
{
    if (!Simulation::loadingData)
    {
        uint32_t k =0;
        for (uint32_t i=0; i<layers; i++)
        {
            for (uint32_t j=0; j<rowCountsForColumns[i]; j++)
                neurons[i][j].Process(HH_NeuralNetworks::NEURAL_NOISE[k++]);
        }
    }
    else
    {
        uint32_t k =0;
        ////for (uint32_t i=0; i<layers; i++)
        uint32_t i=0;
        {
            for (uint32_t j=0; j<rowCountsForColumns[i]; j++)
                neurons[i][j].Process(HH_NeuralNetworks::NEURAL_NOISE[k++]);
        }

        i=layers - 1;
        {
            for (uint32_t j=0; j<rowCountsForColumns[i]; j++)
                neurons[i][j].Process(HH_NeuralNetworks::NEURAL_NOISE[k++]);
        }
    }
}

/*////void HH_Neurons::LoadVoltageData(FILE* file)
{
    uint32_t layerIndex, neuronsCountForLayer;
    double voltage;

    char textBuffer[999999];
    memset(textBuffer, 0, 999999);

        char *ptr;

        fgets(textBuffer, 999999, file);

            ptr = strtok(textBuffer, ",");
            layerIndex = atoi(ptr);

            ptr = strtok(NULL, ",");
            neuronsCountForLayer = atoi(ptr);

        uint32_t j = 0;
        while (ptr != NULL)
        {
            ptr = strtok (NULL, ",");

            voltage = atof(ptr);

            if (layerIndex != 0 || j > 9)
                int grc = 1;

            neurons[layerIndex][j].membraneVoltage = voltage;

            ////neurons[layerIndex][j].AddMembraneVoltageToGraphValues();

            j++;
        }
}*/

void HH_Neurons::LoadVoltagesForlayer(FILE* file)
{
    uint32_t layerIndex, neuronsCountForLayer;
    double voltage;

    char textBuffer[999999];
    memset(textBuffer, 0, 999999);

        char *ptr;

        fgets(textBuffer, 999999, file);

            ptr = strtok(textBuffer, ",");
            layerIndex = atoi(ptr);

            ptr = strtok(NULL, ",");
            neuronsCountForLayer = atoi(ptr);

        uint32_t j = 0;
        while (ptr != NULL)
        {
            ptr = strtok (NULL, ",");

            voltage = atof(ptr);

            if (layerIndex != 0 || j > 9)
                int grc = 1;

            neurons[layerIndex][j].membraneVoltage = voltage;

            ////neurons[layerIndex][j].AddMembraneVoltageToGraphValues();

            j++;
        }
}

void HH_Neurons::LoadVoltageData(FILE* file)
{
    LoadVoltagesForlayer(file);
    LoadVoltagesForlayer(file);
}

uint32_t HH_Neurons::LoadVoltagesForLayerBinary(char* buffer, uint32_t offset)
{
    uint32_t layerIndex = *reinterpret_cast<uint32_t*>(buffer + offset);
    offset += sizeof(uint32_t);

    ////std::cout << "layerIndex = "<< layerIndex;
    ////std::cout << "\n\n";

    uint32_t neuronsCountForLayer = *reinterpret_cast<uint32_t*>(buffer + offset);
    offset += sizeof(uint32_t);

    ////std::cout << "neuronsCountForLayer = "<< neuronsCountForLayer;
    ////std::cout << "\n\n";

    double voltage;

    uint32_t j = 0;
    for (uint32_t i=0; i<neuronsCountForLayer; i++)
    {
        voltage = *reinterpret_cast<double*>(buffer + offset);
        offset += sizeof(double);

        if (voltage > -70)
            int grc = 1;

        neurons[layerIndex][j].membraneVoltage = voltage;

        j++;
    }

    return offset;
}

uint32_t HH_Neurons::LoadVoltageDataBinary(char* buffer, uint32_t offset)
{
    offset = LoadVoltagesForLayerBinary(buffer, offset);
    offset = LoadVoltagesForLayerBinary(buffer, offset);

    return offset;
}


void HH_Neurons::SaveVoltagesForLayer(FILE* file, uint32_t layerIndex)
{
    char textBuffer[255];
    size_t length;

    sprintf(textBuffer, "%i, ", layerIndex);
    length = strlen(textBuffer);
    fwrite(textBuffer, sizeof(char), length, file);

    sprintf(textBuffer, "%i", rowCountsForColumns[layerIndex]);
    length = strlen(textBuffer);
    fwrite(textBuffer, sizeof(char), length, file);

    for (uint32_t j=0; j<rowCountsForColumns[layerIndex]; j++)
    {
        sprintf(textBuffer, ",%f", neurons[layerIndex][j].membraneVoltage);
        length = strlen(textBuffer);
        fwrite(textBuffer, sizeof(char), length, file);
    }

    fwrite("\n", sizeof(char), 1, file);
}

void HH_Neurons::SaveVoltages(FILE* file)
{
    uint32_t layerIndex=0;

    SaveVoltagesForLayer(file, 0);
    SaveVoltagesForLayer(file, layers-1);
}

void HH_Neurons::SaveVoltagesForLayerBinary(std::ofstream* file, uint32_t layerIndex)
{
    file->write(reinterpret_cast<const char*> (&layerIndex), sizeof(uint32_t));
    file->write(reinterpret_cast<const char*> (&rowCountsForColumns[layerIndex]), sizeof(uint32_t));

    for (uint32_t j=0; j<rowCountsForColumns[layerIndex]; j++)
    {
        file->write(reinterpret_cast<const char*> (&neurons[layerIndex][j].membraneVoltage), sizeof(double));
    }
}

void HH_Neurons::SaveVoltagesBinary(std::ofstream* file)
{
    uint32_t layerIndex=0;

    SaveVoltagesForLayerBinary(file, 0);
    SaveVoltagesForLayerBinary(file, layers-1);
}

void HH_Neurons::AddResultsBuffers()
{
    if (strcmp(mostRecentStimulus, "")!=0 && strcmp(mostRecentStimulus, "000000000")!=0)
    {
        uint32_t* spikeCountsArray = new uint32_t[HH_NeuralNetwork::MAX_ROW_COUNT];
    ////existingQACount = 0;
    HH_NeuralNetwork* networkPtr = (HH_NeuralNetwork*) neuralNetwork;
    Result *result;
    for (uint32_t i=0; i<layers;i++)
    {
        ////GetSpikeCountsArrayForLayer(i, spikeCountsArray);
        ////if (resultsBuffers[i]->Existing(mostRecentStimulus, neurons[i]))
        result = resultsBuffers[i]->Existing(mostRecentStimulus, GetSpikeCountsArrayForLayer(i, spikeCountsArray));

        if (result)
        {
            networkPtr->agent->action = result->action;
        }
        else
        {
            if (strcmp(mostRecentStimulus, "000000111")==0)
                networkPtr->agent->action = Action::RotateRight;
            else
                if (strcmp(mostRecentStimulus, "000111000")==0)
                    networkPtr->agent->action = Action::Forward;
                else
                    if (strcmp(mostRecentStimulus, "111000000")==0)
                        networkPtr->agent->action = Action::RotateLeft;

            resultsBuffers[i]->Add(mostRecentStimulus, neurons[i], networkPtr->agent->action);
        }

        ////existingQACount += resultsBuffers[i]->GetExistingCount(currentQ);
    }

        delete spikeCountsArray;
    }
}

char* HH_Neurons::GetStringFromResult(uint32_t index)
{
    for (uint32_t i=0; i<rowCountsForColumns[index]; i++)
    {
        itoa(neurons[index][i].spikeCount, &resultStr[i], 10);
    }

    return resultStr;
}

Action HH_Neurons::GetResultAction()
{
    ////char* strResult = GetStringFromResult(layers - 1);

    uint32_t* spikeCountsArray = new uint32_t[HH_NeuralNetwork::MAX_ROW_COUNT];

    Action action = resultsBuffers[layers-1]->GetActionForResult(GetSpikeCountsArrayForLayer(layers - 1, spikeCountsArray));
    ////resultsBuffers[layers-1]->GetAction(strResult);

    delete spikeCountsArray;

    return  action;
}

uint32_t HH_Neurons::GetExistingQCount(char* q, uint32_t layerIndex)
{
    return resultsBuffers[layerIndex]->GetExistingQCount(q);
}

uint32_t HH_Neurons::GetExistingACount(HH_Neuron* result, uint32_t layerIndex)
{
    return resultsBuffers[layerIndex]->GetExistingACount(result);
}

Vector HH_Neurons::GetNewGraphXY(uint32_t column, uint32_t row)
{
    Vector newGraphXY;

	//newGraphXY.x = ((HH_NeuralNetwork *) neuralNetwork)->pos.x;

	////newGraphXY.x = (column * (Graph::WIDTH + Graph::X_GAP));

	newGraphXY.x = ((column % HH_NeuralNetwork::MAX_LAYER_COLUMN_WIDTH) * (Graph::WIDTH + Graph::X_GAP));

	if (column >= HH_NeuralNetwork::MAX_LAYER_COLUMN_WIDTH)
        newGraphXY.x += Graph::WIDTH + Graph::X_GAP;


	//newGraphXY.y = ((HH_NeuralNetwork *) neuralNetwork)->pos.y;

	newGraphXY.y = row * (Graph::HEIGHT + Graph::Y_GAP);


	uint32_t maxNeuronRows = 0;
	for (uint32_t i=0; i<layers; i++)
    {
        if (rowCountsForColumns[i] > maxNeuronRows)
            maxNeuronRows= rowCountsForColumns[i];
    }

	newGraphXY.y -= (column / HH_NeuralNetwork::MAX_LAYER_COLUMN_WIDTH) * (maxNeuronRows * (Graph::HEIGHT + Graph::Y_GAP) + Graph::Y_GAP);

	return newGraphXY;
}


/*float HH_Neurons::GetNewGraphX(uint32_t neuronCount)
{
	float newGraphX = ((HH_NeuralNetwork *) neuralNetwork)->pos.x;

	columns = neuronCount / rowCountsForColumns[layers-1];

	newGraphX += (columns * (Graph::WIDTH + Graph::X_GAP));

	return newGraphX;
}

float HH_Neurons::GetNewGraphY(uint32_t neuronCount)
{
	float newGraphY = ((HH_NeuralNetwork *) neuralNetwork)->pos.y;

	columns = neuronCount / rowCountsForColumns[layers-1];

	neuronCount -= (columns * rowCountsForColumns[layers-1]);

	int rowCount = neuronCount % rowCountsForColumns[layers-1];

	/*for (int i = 0; i < rowCount; i++)
	{
		newGraphY  -= (graphs[i]->height + Graph::GAP);
	}
	*/
/*
	newGraphY  += rowCount * (Graph::HEIGHT + Graph::Y_GAP);

	//newGraphY -= gap * placedGraphsCount;

	return newGraphY;
}
*/

void HH_Neurons::AdjustSynapseConductanceForColumn(uint32_t column, double value)
{
    /*uint32_t columnStartIndex = column * rowCountsForColumns[layers-1];
    uint32_t columnEndIndex = columnStartIndex + rowCountsForColumns[layers-1];

    for (uint32_t i=columnStartIndex; i<columnEndIndex;i++)
    {
        neurons[i].AdjustAllSynapseConductance(value);
    }*/

    for (uint32_t i=0; i<rowCountsForColumns[column]; i++)
        neurons[column][i].AdjustAllSynapseConductance(value);
}

void HH_Neurons::GetSignalCountForEachThirdOfResultLayer(uint32_t *signalCountsForEachThirdOfResultLayer)
{
    uint32_t j = HH_NeuralNetwork::LAYERS - 1;

    for (uint32_t i=0; i<3;i++)
        signalCountsForEachThirdOfResultLayer[i] = 0;

        for (uint32_t i=0; i<HH_NeuralNetwork::ROWS[j];i++)
        {
            if (neurons[j][i].membraneVoltage > 0)
            {
                if (i<HH_NeuralNetwork::ROWS[j]/3)
                    signalCountsForEachThirdOfResultLayer[0]++;
                else
                    if (i>=HH_NeuralNetwork::ROWS[j]/3 && i<HH_NeuralNetwork::ROWS[j]*2.0/3.0)
                        signalCountsForEachThirdOfResultLayer[1]++;
                    else
                        if (i>=HH_NeuralNetwork::ROWS[j]*2.0/3.0)
                            signalCountsForEachThirdOfResultLayer[2]++;
            }
    }
}

/*///uint32_t_ptr* HH_Neurons::GetLayersSignaling(bool print)
{
    if (!currentQA)
        return NULL;

    for (uint32_t j=0; j<layers;j++)
    {
        for (uint32_t i=0; i<rowCountsForColumns[j];i++)
        {
            if (neurons[j][i].membraneVoltage > 0)
                layerSignals[j][i] = 1;
            else
                layerSignals[j][i] = 0;
        }
    }
        for (uint32_t i=0; i<rowCountsForColumns[0];i++)
            layer1SignalsBuffer[i] = layerSignals[0][i];

        error = currentQA->CheckQ(layer1SignalsBuffer);

        for (uint32_t i=0; i<rowCountsForColumns[layers-1];i++)
        {
            resultLayerSignalsBuffer[i] = layerSignals[layers-1][i];
        }

        ////error = currentQA->CheckA(resultLayerSignalsBuffer, rowCountsForColumns[layers-1], print);


        uint32_t resultSignalCount = 0;
        for (uint32_t i=0; i<HH_NeuralNetwork::ROWS[HH_NeuralNetwork::LAYERS-1];i++)
        {
            if (resultLayerSignalsBuffer[i] > 0)
                resultSignalCount++;
            //resultSignalCount += resultLayerSignalsBuffer[i];
        }


        error = 0;

        ////if (strcmp(currentQA->q, "0,0,0,0")!=0)
        if (strcmp(currentQA->q, "00000000")!=0)
        {
        //if (resultSignalCount <= HH_NeuralNetwork::ROWS[HH_NeuralNetwork::LAYERS-1] && !resultsBuffers->Existing(resultLayerSignalsBuffer))
        if (!resultsBuffers->Existing(resultLayerSignalsBuffer))
        {
            resultsBuffers->Add(resultLayerSignalsBuffer);
        }
        else
        {
            //if (strcmp(currentQA->q, previousQAs[0]->q)!=0 && strcmp(previousQAs[0]->q, "0,0,0,0")!=0)
            //if (strcmp(previousQA->q, "0,0,0,0")!=0)
                //if (strcmp(currentQA->q, previousQA->q)!=0)
                    error = 1;
                    //error = 0;
        }
        }

        ////error = 1;
        totalError += error;


        if (print && strcmp(currentQA->q, TrainingSet::inputNullStr) != 0)
        {
        char text[255];
        sprintf(text, "Answer: %s", currentQA->a);
        std::cout << text<< "\n";
        sprintf(text, "Result: %i,%i,%i,%i,%i,%i,%i,%i", resultLayerSignalsBuffer[0], resultLayerSignalsBuffer[1], resultLayerSignalsBuffer[2], resultLayerSignalsBuffer[3], resultLayerSignalsBuffer[4], resultLayerSignalsBuffer[5], resultLayerSignalsBuffer[6], resultLayerSignalsBuffer[7]);
        std::cout << text<< "\n";
        sprintf(text, "Error: %f", error);
        std::cout << text<< "\n";
        std::cout << "\n";
        }


    previousQA = currentQA;

    newLayer1Signals = false;

    return layerSignals;
}
*/

/*void HH_Neurons::DetermineError(bool print)
{
    if (currentQA)
    {
        //Check for QA errors
        ////error = currentQA->CheckA(resultLayerSignalsBuffer, rowCountsForColumns[layers-1], print);

        error = 0;
        if (strcmp(currentQA->q, TrainingSet::inputNullStr) != 0)
        {
            for (uint32_t i=0; i<layers;i++)
            {
                ////if (!resultsBuffers->Existing(resultLayerSignalsBuffer) && resultSignalCount > 0)
                if (resultsBuffers[i]->Existing(neurons[i]) || GetSpikeCountsForLayer(i) == 0)
                {
                    resultsBuffers[i]->error++;

                    error++;
                }
                else
                {
                    resultsBuffers[i]->Add(neurons[i]);
                }
            }
        }

        totalError = GetTotalError();

        for (uint32_t j=0; j<layers;j++)
        {
            for (uint32_t i=0; i<rowCountsForColumns[j];i++)
            {
                neurons[j][i].spikeCount = 0;
            }
        }

        if (print && strcmp(currentQA->q, TrainingSet::inputNullStr) != 0)
    {
        char text[255];
        sprintf(text, "Answer: %s", currentQA->a);
        std::cout << text<< "\n";
        //sprintf(text, "Result: %i,%i,%i,%i,%i,%i", resultLayerSignalsBuffer[0], resultLayerSignalsBuffer[1], resultLayerSignalsBuffer[2], resultLayerSignalsBuffer[3], resultLayerSignalsBuffer[4], resultLayerSignalsBuffer[5]);
        //sprintf(text, "Result: %i,%i,%i,%i", resultLayerSignalsBuffer[0], resultLayerSignalsBuffer[1], resultLayerSignalsBuffer[2], resultLayerSignalsBuffer[3]);
        sprintf(text, "Result: %i,%i,%i,%i,%i,%i,%i,%i", resultLayerSignalsBuffer[0], resultLayerSignalsBuffer[1], resultLayerSignalsBuffer[2], resultLayerSignalsBuffer[3], resultLayerSignalsBuffer[4], resultLayerSignalsBuffer[5], resultLayerSignalsBuffer[6], resultLayerSignalsBuffer[7]);
        std::cout << text<< "\n";
        sprintf(text, "Error: %f", error);
        std::cout << text<< "\n";
        if (error==0)
        {
            sprintf(text, "Results added to Buffer");
            std::cout << text<< "\n";
        }
        else
        {
            sprintf(text, "Result already existing in Buffer");
            std::cout << text<< "\n";
        }
        std::cout << "\n";
    }

        newLayer1Signals = false;
    }
}
*/

/*
uint32_t_ptr* HH_Neurons::GetLayersSignaling(bool print)
{
    if (!currentQA)
        return NULL;

    for (uint32_t j=0; j<HH_NeuralNetwork::LAYERS;j++)
    {
        for (uint32_t i=0; i<HH_NeuralNetwork::ROWS[j];i++)
        {
            if (neurons[j][i].membraneVoltage > 0)
                layerSignals[j][i] = 1;
            else
                layerSignals[j][i] = 0;

            //layerSignals[j][k++] = neurons[i].spikeCount;
        }
    }

    if (newLayer1Signals)
    {


    /*for (uint32_t j=0; j<HH_NeuralNetwork::LAYERS;j++)
    {
        k=0;

        for (uint32_t i=columnStartIndex; i<columnEndIndex;i++)
        {
            if (neurons[i].membraneVoltage > 0)
                layerSignals[j][k++] = 1;
            else
                layerSignals[j][k++] = 0;

            //layerSignals[j][k++] = neurons[i].spikeCount;
        }

        columnStartIndex += HH_NeuralNetwork::ROWS;
        columnEndIndex += HH_NeuralNetwork::ROWS;
    }
    */
/*
        for (uint32_t i=0; i<HH_NeuralNetwork::ROWS[0];i++)
            layer1SignalsBuffer[i] = layerSignals[0][i];

        error = currentQA->CheckQ(layer1SignalsBuffer);

        for (uint32_t i=0; i<HH_NeuralNetwork::ROWS[HH_NeuralNetwork::LAYERS-1];i++)
        {
            resultLayerSignalsBuffer[i] = layerSignals[HH_NeuralNetwork::LAYERS-1][i];
        }

        ////error = currentQA->CheckA(resultLayerSignalsBuffer, HH_NeuralNetwork::ROWS[HH_NeuralNetwork::LAYERS-1], print);


        uint32_t resultSignalCount = 0;
        for (uint32_t i=0; i<HH_NeuralNetwork::ROWS[HH_NeuralNetwork::LAYERS-1];i++)
        {
            if (resultLayerSignalsBuffer[i] > 0)
                resultSignalCount++;
            //resultSignalCount += resultLayerSignalsBuffer[i];
        }


        error = 0;

        if (strcmp(currentQA->q, "0,0,0,0")!=0)
        {
        //if (resultSignalCount <= HH_NeuralNetwork::ROWS[HH_NeuralNetwork::LAYERS-1] && !resultsBuffers->Existing(resultLayerSignalsBuffer))
        if (!resultsBuffers->Existing(resultLayerSignalsBuffer))
        {
            resultsBuffers->Add(resultLayerSignalsBuffer);
        }
        else
        {
            //if (strcmp(currentQA->q, previousQAs[0]->q)!=0 && strcmp(previousQAs[0]->q, "0,0,0,0")!=0)
            //if (strcmp(previousQA->q, "0,0,0,0")!=0)
                //if (strcmp(currentQA->q, previousQA->q)!=0)
                    error = 1;
                    //error = 0;
        }
        }

        totalError += error;


        if (print && strcmp(currentQA->q, "0,0,0,0") != 0)
    {
        char text[255];
        sprintf(text, "Answer: %s", currentQA->a);
        std::cout << text<< "\n";
        //sprintf(text, "Result: %i,%i,%i,%i,%i,%i", resultLayerSignalsBuffer[0], resultLayerSignalsBuffer[1], resultLayerSignalsBuffer[2], resultLayerSignalsBuffer[3], resultLayerSignalsBuffer[4], resultLayerSignalsBuffer[5]);
        sprintf(text, "Result: %i,%i,%i,%i", resultLayerSignalsBuffer[0], resultLayerSignalsBuffer[1], resultLayerSignalsBuffer[2], resultLayerSignalsBuffer[3]);
        std::cout << text<< "\n";
        sprintf(text, "Error: %f", error);
        std::cout << text<< "\n";
        if (error==0)
        {
            sprintf(text, "Results added to Buffer");
            std::cout << text<< "\n";
        }
        else
        {
            sprintf(text, "Result already existing in Buffer");
            std::cout << text<< "\n";
        }
        std::cout << "\n";
    }

        /*error = 0;

        if (strcmp(currentQA->q, "0,0,0,0")!=0)
        {
            if (resultSignalCount == 0 || resultSignalCount == HH_NeuralNetwork::ROWS[HH_NeuralNetwork::LAYERS-1])
                error = 1;
        }

        totalError += error;
        */


        /*for (uint32_t i=0; i<size;i++)
        {
            neurons[i].spikeCount = 0;
        }*/

    /*bool newLayer2Signals = false;

    for (uint32_t i=0; i<HH_NeuralNetwork::ROWS;i++)
        if (layerSignals[1][i] > 0)
        {
            newLayer2Signals = true;
            break;
        }


    if (newLayer2Signals)
    {
        for (uint32_t i=0; i<HH_NeuralNetwork::ROWS;i++)
        {
            resultLayerSignalsBuffer[i] = layerSignals[1][i];

            //layerSignals[1][i];
        }
    }


    for (uint32_t i=0; i<size;i++)
    {
        neurons[i].spikeCount = 0;
    }*/
/*
    previousQA = currentQA;

    newLayer1Signals = false;
    }

    return layerSignals;
}

uint32_t_ptr* HH_Neurons::GetLayersSignalingHH(bool print)
{
     for (uint32_t j=0; j<HH_NeuralNetwork::LAYERS;j++)
    {
        for (uint32_t i=0; i<HH_NeuralNetwork::ROWS[j];i++)
        {
            /*if (neurons[j][i].membraneVoltage > 0)
                layerSignals[j][i] = 1;
            else
                layerSignals[j][i] = 0;
                */
/*
            layerSignals[j][i] = neurons[j][i].spikeCount;
        }
    }

    /*uint32_t k;

    for (uint32_t j=0; j<HH_NeuralNetwork::LAYERS;j++)
    {
        k=0;

        for (uint32_t i=columnStartIndex; i<columnEndIndex;i++)
        {
            layerSignals[j][k++] = neurons[i].spikeCount;
        }

        columnStartIndex += HH_NeuralNetwork::ROWS;
        columnEndIndex += HH_NeuralNetwork::ROWS;
    }
    */

    /*bool newLayer1Signals = false;

    for (uint32_t i=0; i<HH_NeuralNetwork::ROWS;i++)
        if (layerSignals[0][i] > 0)
        {
            newLayer1Signals = true;
            break;
        }
*/
/*    if (newLayer1Signals)
    {
        for (uint32_t i=0; i<HH_NeuralNetwork::ROWS[HH_NeuralNetwork::LAYERS-1];i++)
        {
            resultLayerSignalsBuffer[i] = layerSignals[HH_NeuralNetwork::LAYERS-1][i];
        }

        //sprintf(graphicsText, "L2BufferN0: %d, L2BufferN1: %i, L2BufferN2: %i, L2BufferN3: %i", resultLayerSignalsBuffer[0], resultLayerSignalsBuffer[1], resultLayerSignalsBuffer[2], resultLayerSignalsBuffer[3]);
        //std::cout << graphicsText << "\n";

        //get a's for previous q's
        /*for (uint32_t i=0; i<HH_NeuralNetwork::ROWS;i++)
        {
            resultLayerSignalsBuffer[i] = layerSignals[HH_NeuralNetwork::LAYERS-1][i];
        }
        */

        ////error = previousQAs[0]->CheckA(resultLayerSignalsBuffer, HH_NeuralNetwork::ROWS[HH_NeuralNetwork::LAYERS-1], print);
/*
        uint32_t resultSignalCount = 0;
        for (uint32_t i=0; i<HH_NeuralNetwork::ROWS[HH_NeuralNetwork::LAYERS-1];i++)
        {
            //if (resultLayerSignalsBuffer[i] > 0)
                //resultSignalCount++;
            resultSignalCount += resultLayerSignalsBuffer[i];
        }

        error = 0;

        if (strcmp(previousQAs[0]->q, "0,0,0,0")!=0)
        //if (strcmp(currentQA->q, "0,0,0,0")!=0)
        {
        if (resultSignalCount <= HH_NeuralNetwork::ROWS[HH_NeuralNetwork::LAYERS-1] && !resultsBuffers->Existing(resultLayerSignalsBuffer))
        {
            resultsBuffers->Add(resultLayerSignalsBuffer);
        }
        else
        {
            error = 1;
            //error = 0;
        }
        }

        /*if (strcmp(previousQAs[0]->q, "0,0,0,0")!=0)
        {
            if (resultSignalCount == 0 || resultSignalCount == HH_NeuralNetwork::ROWS[HH_NeuralNetwork::LAYERS-1])
                error = 1;
        }*/



        /*if (strcmp(previousQAs[0]->q, "0,0,0,0")!=0)
        {
            if (resultSignalCount == 0 || resultSignalCount == HH_NeuralNetwork::ROWS[HH_NeuralNetwork::LAYERS-1])
                error = 1;
        }*/
/*
        totalError += error;

        if (print && strcmp(previousQAs[0]->q, "0,0,0,0") != 0)
    {
        char text[255];
        sprintf(text, "Answer: %s", previousQAs[0]->a);
        std::cout << text<< "\n";
        //sprintf(text, "Result: %i,%i,%i,%i,%i,%i", resultLayerSignalsBuffer[0], resultLayerSignalsBuffer[1], resultLayerSignalsBuffer[2], resultLayerSignalsBuffer[3], resultLayerSignalsBuffer[4], resultLayerSignalsBuffer[5]);
        sprintf(text, "Result: %i,%i,%i,%i", resultLayerSignalsBuffer[0], resultLayerSignalsBuffer[1], resultLayerSignalsBuffer[2], resultLayerSignalsBuffer[3]);
        std::cout << text<< "\n";
        sprintf(text, "Error: %f", error);
        std::cout << text<< "\n";
        if (error==0)
        {
            sprintf(text, "Results added to Buffer");
            std::cout << text<< "\n";
        }
        else
        {
            sprintf(text, "Result already existing in Buffer");
            std::cout << text<< "\n";
        }
        std::cout << "\n";
    }

        for (uint32_t i=0; i<HH_NeuralNetwork::ROWS[0];i++)
            layer1SignalsBuffer[i] = layerSignals[0][i];

        for (uint32_t j=0; j<HH_NeuralNetwork::LAYERS;j++)
        {
            for (uint32_t i=0; i<HH_NeuralNetwork::ROWS[j];i++)
            {
                neurons[j][i].spikeCount = 0;
            }
        }

        /*for (uint32_t i=0; i<size;i++)
        {
            neurons[i].spikeCount = 0;
        }
        */
/*
        newLayer1Signals = false;
    }


    /*bool newLayer2Signals = false;

    for (uint32_t i=0; i<HH_NeuralNetwork::ROWS;i++)
        if (layerSignals[1][i] > 0)
        {
            newLayer2Signals = true;
            break;
        }


    if (newLayer2Signals)
    {
        for (uint32_t i=0; i<HH_NeuralNetwork::ROWS;i++)
        {
            resultLayerSignalsBuffer[i] = layerSignals[1][i];

            //layerSignals[1][i];
        }
    }


    for (uint32_t i=0; i<size;i++)
    {
        neurons[i].spikeCount = 0;
    }*/
/*
    return layerSignals;
}
*/

/*uint8_t* HH_Neurons::GetLayer1SignalingBuffer()
{
    return layer1SignalsBuffer;
}*/


/*uint32_t* HH_Neurons::GetLayersSignalingCount()
{
    //uint32_t signalingCounts = 0;

    /*uint32_t signalingCount1 = 0;
    uint32_t signalingCount2 = 0;
    uint32_t signalingCount3 = 0;
    uint32_t signalingCount4 = 0;
    uint32_t signalingCount5 = 0;
    uint32_t signalingCount6 = 0;

    for (uint32_t i=0; i<10;i++)
    {
        signalingCount1 += neurons[i].spikeCount;
    }

    for (uint32_t i=10; i<20;i++)
    {
        signalingCount2 += neurons[i].spikeCount;
    }

    for (uint32_t i=20; i<30;i++)
    {
        signalingCount3 += neurons[i].spikeCount;
    }

    for (uint32_t i=30; i<40;i++)
    {
        signalingCount4 += neurons[i].spikeCount;
    }

    for (uint32_t i=40; i<50;i++)
    {
        signalingCount5 += neurons[i].spikeCount;
    }

    for (uint32_t i=50; i<60;i++)
    {
        signalingCount6 += neurons[i].spikeCount;
    }
    */

/*    signalingCounts[0] = 0;
    signalingCounts[1] = 0;
    signalingCounts[2] = 0;
    signalingCounts[3] = 0;
    signalingCounts[4] = 0;
    signalingCounts[5] = 0;


    uint32_t columnStartIndex, columnEndIndex;

    columnStartIndex = 0;
    columnEndIndex = columnStartIndex + rowCountsForColumns[layers-1];

    for (uint32_t j=0; j<layers;j++)
    {
        for (uint32_t i=columnStartIndex; i<columnEndIndex;i++)
        {
            signalingCounts[j] += neurons[i].spikeCount;
        }

        columnStartIndex += rowCountsForColumns[layers-1];
        columnEndIndex += rowCountsForColumns[layers-1];
    }

    /*if (signalingCounts[0] == rowCountsForColumns[layers-1])
        layer1Activated++;

    if (layer1Activated == 2)
    {
        for (uint32_t j=1; j<6;j++)
        {

            if (signalingCounts[j] > (rowCountsForColumns[layers-1]/2))
                AdjustSynapseConductanceForColumn(j, -0.00001);
            else if (signalingCounts[j] < (rowCountsForColumns[layers-1]/2))
                AdjustSynapseConductanceForColumn(j, 0.00001);
        }

            for (uint32_t i=0; i<size;i++)
            {
                neurons[i].spikeCount = 0;
            }

            layer1Activated = 0;
    }

    for (uint32_t i=0; i<rowCountsForColumns[layers-1];i++)
            {
                neurons[i].spikeCount = 0;
            }


/*    for (uint32_t i=10; i<20;i++)
    {
        signalingCounts[1] += neurons[i].spikeCount;
    }

    for (uint32_t i=20; i<30;i++)
    {
        signalingCounts[2] += neurons[i].spikeCount;
    }

    for (uint32_t i=30; i<40;i++)
    {
        signalingCounts[3] += neurons[i].spikeCount;
    }

    for (uint32_t i=40; i<50;i++)
    {
        signalingCounts[4] += neurons[i].spikeCount;
    }

    for (uint32_t i=50; i<60;i++)
    {
        signalingCounts[5] += neurons[i].spikeCount;
    }
    */

    /*for (uint32_t j=1; j<6;j++)
    {
        //if (signalingCounts[j] != 0)
        /*{
            if (signalingCounts[j] > (rowCountsForColumns[layers-1]/2))
                AdjustSynapseConductanceForColumn(j, -0.0002);
            else if (signalingCounts[j] < (rowCountsForColumns[layers-1]/2))
                AdjustSynapseConductanceForColumn(j, 0.0002);
        }
        else
        {
            int grc = 1;
        }*/

/*
            if (signalingCounts[j] == 0)
                AdjustSynapseConductanceForColumn(j, 0.0000001);
            else if (signalingCounts[j] > (rowCountsForColumns[layers-1]/2))
                AdjustSynapseConductanceForColumn(j, -0.00001);
            else if (signalingCounts[j] < (rowCountsForColumns[layers-1]/2))
                AdjustSynapseConductanceForColumn(j, 0.00001);




            /*if (signalingCounts[j] > (signalingCounts[0]/2))
                AdjustSynapseConductanceForColumn(j, (double) rand()/ RAND_MAX * -0.00001);
            else if (signalingCounts[j] < (signalingCounts[0]/2))
                AdjustSynapseConductanceForColumn(j, (double) rand()/ RAND_MAX * 0.00001);
                */
/*    }

     for (uint32_t i=0; i<size;i++)
        {
            //neurons[i].SetSynStimulus(stimulus);
            neurons[i].spikeCount = 0;
        }
        */
/*
    return signalingCounts;
}*/

double HH_Neurons::GetStrongestABSWeight()
{
    double maxABSWeight, weight;

    for (uint32_t j=0; j<layers;j++)
        {
            for (uint32_t i=0; i<rowCountsForColumns[j];i++)
            {
                weight = neurons[j][i].GetStrongestABSWeight();

                if (weight > maxABSWeight)
                    maxABSWeight = weight;
            }
        }

    return maxABSWeight;
}

Vector HH_Neurons::GetMinMaxWeight()
{
    Vector minMaxWeight, weight;

    for (uint32_t j=0; j<layers;j++)
        {
            for (uint32_t i=0; i<rowCountsForColumns[j];i++)
            {
                weight = neurons[j][i].GetMinMaxWeight();

                if (weight.x < minMaxWeight.x)
                    minMaxWeight.x = weight.x;

                if (weight.y > minMaxWeight.y)
                    minMaxWeight.y = weight.y;
            }
        }

    /*for (uint32_t i=0; i<size;i++)
    {
        weight = neurons[i].GetMinMaxWeight();

        if (weight.x < minMaxWeight.x)
            minMaxWeight.x = weight.x;

        if (weight.y > minMaxWeight.y)
            minMaxWeight.y = weight.y;
    }*/

    return minMaxWeight;
}

void HH_Neurons::Draw()
{
    //GetLayersSignalingCount();

    glColor3f(1, 1, 1);

    ////sprintf(graphicsText, "Temperature %.2f C, Error: %.2f, Total Error: %.2f", neurons[0][0].Tc, error, totalError);
    //GraphicsUtilities::DrawTextStr(graphicsText, ((HH_NeuralNetwork *) neuralNetwork)->pos.x, ((HH_NeuralNetwork *) neuralNetwork)->pos.y + 1000, 0.0, 0.7);
    //GraphicsUtilities::DrawTextStr(graphicsText, ((HH_NeuralNetwork *) neuralNetwork)->pos.x, ((HH_NeuralNetwork *) neuralNetwork)->pos.y - 150, 0.0, 0.7);
    sprintf(graphicsText, "Temperature %.4f C", neurons[0][0].Tc);
    GraphicsUtilities::DrawTextStr(graphicsText, 0, - 150, 0.0, 0.7);

    sprintf(graphicsText, "Stimulus: %s", mostRecentStimulus);
    GraphicsUtilities::DrawTextStr(graphicsText, 0, -250, 0.0, 0.7);

    sprintf(graphicsText, "Result: %d,%d,%d", neurons[layers-1][0].spikeCount, neurons[layers-1][1].spikeCount, neurons[layers-1][2].spikeCount);
    GraphicsUtilities::DrawTextStr(graphicsText, 0, -350, 0.0, 0.7);

    sprintf(graphicsText, "Fitness: %f", ((HH_NeuralNetwork *) neuralNetwork)->agent->fitness);
    GraphicsUtilities::DrawTextStr(graphicsText, 0, -450, 0.0, 0.7);


    /*////sprintf(graphicsText, "Result: %d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d", layer1SignalsBuffer[0],layer1SignalsBuffer[1],layer1SignalsBuffer[2],layer1SignalsBuffer[3],layer1SignalsBuffer[4],layer1SignalsBuffer[5],layer1SignalsBuffer[6],layer1SignalsBuffer[7],layer1SignalsBuffer[8],layer1SignalsBuffer[9],layer1SignalsBuffer[10],layer1SignalsBuffer[11]);
    GraphicsUtilities::DrawTextStr(graphicsText, 0, -250, 0.0, 0.7);

    //sprintf(graphicsText, "Result: %d,%d,%d,%d,%d,%d,", resultLayerSignalsBuffer[0],resultLayerSignalsBuffer[1],resultLayerSignalsBuffer[2],resultLayerSignalsBuffer[3],resultLayerSignalsBuffer[4],resultLayerSignalsBuffer[5]);
    sprintf(graphicsText, "Result: %d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d", resultLayerSignalsBuffer[0],resultLayerSignalsBuffer[1],resultLayerSignalsBuffer[2],resultLayerSignalsBuffer[3],resultLayerSignalsBuffer[4],resultLayerSignalsBuffer[5],resultLayerSignalsBuffer[6],resultLayerSignalsBuffer[7],resultLayerSignalsBuffer[8],resultLayerSignalsBuffer[9],resultLayerSignalsBuffer[10],resultLayerSignalsBuffer[11]);
    GraphicsUtilities::DrawTextStr(graphicsText, 0, -350, 0.0, 0.7);

    if (HH_NeuralNetworks::electromagneticRF_HH)
    {
        sprintf(graphicsText, "Electromagnetic Error: %f", totalElectromagneticError);
        GraphicsUtilities::DrawTextStr(graphicsText, 0, -450, 0.0, 0.7);
    }*/

    //sprintf(graphicsText, "Total Signaling Count for Layers");
    //GraphicsUtilities::DrawTextStr(graphicsText, GetNewGraphX(size-1) + Graph::X_GAP, ((HH_NeuralNetwork *) neuralNetwork)->pos.y + 200, 0.0, 0.9);

    //sprintf(graphicsText, "L1: %.d, L2: %.d", signalingCounts[0], signalingCounts[1], signalingCounts[2], signalingCounts[3], signalingCounts[4], signalingCounts[5]);
    //GraphicsUtilities::DrawTextStr(graphicsText, GetNewGraphX(size-1) + Graph::X_GAP, ((HH_NeuralNetwork *) neuralNetwork)->pos.y, 0.0, 0.9);

    //Vector minMaxWeight = GetMinMaxWeight();
        double maxABSWeight = GetStrongestABSWeight();

        for (uint32_t j=0; j<layers;j++)
        {
            ////sprintf(graphicsText, "Layer Error: %d", resultsBuffers[j]->error);
            ////GraphicsUtilities::DrawTextStr(graphicsText, neurons[j][0].pos.x, neurons[j][0].pos.y - 300, 0.0, 0.7);

            for (uint32_t i=0; i<rowCountsForColumns[j];i++)
            {
                ////if (j>1)
                    neurons[j][i].Draw(maxABSWeight);
            }
        }
}

void HH_Neurons::ResetHH()
{
    for (uint32_t j=0; j<layers;j++)
        {
            for (uint32_t i=0; i<rowCountsForColumns[j];i++)
            {
                neurons[j][i].ResetHH();
            }
        }
}

uint32_t* HH_Neurons::GetSpikeCountsArrayForLayer(uint32_t layerIndex, uint32_t* spikeCountsArray)
{
    for (uint32_t i=0; i<rowCountsForColumns[layerIndex];i++)
    {
        spikeCountsArray[i] = neurons[layerIndex][i].spikeCount;
    }

    return spikeCountsArray;
}

void HH_Neurons::GetSpikeCountArraysForLayers(uint32_t **spikeCountsArrays)
{
    for (uint32_t j=0; j<HH_NeuralNetwork::LAYERS;j++)
    {
        for (uint32_t i=0; i<rowCountsForColumns[j];i++)
        {
            spikeCountsArrays[j][i] = neurons[j][i].spikeCount;
        }
    }
}

void HH_Neurons::ResetSpikeCounts()
{
    for (uint32_t j=0; j<layers;j++)
        {
            for (uint32_t i=0; i<rowCountsForColumns[j];i++)
            {
                neurons[j][i].ResetSpikeCounts();
            }
        }
}

/*////uint32_t HH_Neurons::GetTotalError()
{
    uint32_t totalError = 0;

    for (uint32_t j=0; j<layers;j++)
    {
        totalError += resultsBuffers[j]->error;
    }

    return totalError;
}*/

void HH_Neurons::SetResultsBuffers(ResultsBuffer_ptr* srcBuffers)
{
    resultsBuffers = srcBuffers;
}

void HH_Neurons::CopyResultsBuffers(ResultsBuffer_ptr* srcBuffers)
{
    for (uint32_t i=0; i<layers;i++)
    {
        resultsBuffers[i]->Copy(srcBuffers[i]);
    }
}

void HH_Neurons::ClearResultsBuffers()
{
    for (uint32_t j=0; j<layers;j++)
    {
        resultsBuffers[j]->Clear();
    }
}

HH_Neurons::~HH_Neurons()
{
    delete [] neurons;
}
