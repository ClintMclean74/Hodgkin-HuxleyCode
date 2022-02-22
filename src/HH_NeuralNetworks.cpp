#include <string.h>
#include "HH_NeuralNetworks.h"
#include "HH_NeuralNetwork.h"
#include "GL/glew.h"
#include "GraphicsUtilities.h"

typedef double* double_ptr;

HH_NeuralNetworks::HH_NeuralNetworks(Agents *agents)
{
    ////voltagesFile = fopen("nnVoltages.txt", "w");

    visible = true;
    HH_NeuralNetwork::ROWS = new uint32_t[HH_NeuralNetwork::LAYERS];


    HH_NeuralNetwork::ROWS[0] = HH_NeuralNetwork::layer1Length;
    uint8_t i = 1;


    /*////for (; i<HH_NeuralNetwork::LAYERS-1; i++)
        HH_NeuralNetwork::ROWS[i] = 8;
        */

    /*////for (; i<HH_NeuralNetwork::LAYERS-1; i++)
        HH_NeuralNetwork::ROWS[i] = 7;*/

    ////HH_NeuralNetwork::ROWS[i-1] = 1;

    ////for (; i<HH_NeuralNetwork::LAYERS-1; i++)
        ////HH_NeuralNetwork::ROWS[i] = 9;

    for (; i<HH_NeuralNetwork::LAYERS-1; i++)
        HH_NeuralNetwork::ROWS[i] = HH_NeuralNetwork::hiddenLayerLength;

    HH_NeuralNetwork::ROWS[i] = HH_NeuralNetwork::resultLayerLength;

    /*////for (; i<HH_NeuralNetwork::LAYERS-1; i++)
        HH_NeuralNetwork::ROWS[i] = HH_NeuralNetwork::resultLayerLength;

    HH_NeuralNetwork::ROWS[i] = HH_NeuralNetwork::resultLayerLength;
    */

    this->agents = agents;
}

void HH_NeuralNetworks::InitializeNNs(char* fileName)
{
        /*////HH_NeuralNetwork* loadedNeuralNetwork2;
        HH_NeuralNetwork* loadedNeuralNetwork3;

        loadedNeuralNetwork2 = HH_NeuralNetwork::Load("neuralnetworks/nn2cols_3_8.txt");
        loadedNeuralNetwork3 = HH_NeuralNetwork::Load("neuralnetworks/nn2cols_8_3.txt");
        */

    pos.x = -Graph::WIDTH * (HH_NeuralNetworks::NETWORKS_COLUMNS/2);

    for (uint32_t i = 0; i < HH_NeuralNetworks::NETWORKS_COLUMNS; i++)
    {
        pos.y = -Graph::HEIGHT * (HH_NeuralNetworks::NETWORKS_ROWS/2);

        for (uint32_t j = 0; j < HH_NeuralNetworks::NETWORKS_ROWS; j++)
        {
            if (strlen(fileName)==0)
            {
                neuralNetworks[neuralNetworksCount] = new HH_NeuralNetwork(HH_NeuralNetwork::LAYERS, HH_NeuralNetwork::ROWS, &pos);
            }
            else
            {
                neuralNetworks[neuralNetworksCount] = HH_NeuralNetwork::Load(fileName);

                if (neuralNetworksCount == 0)
                    HH_NeuralNetwork::LAYERS = neuralNetworks[neuralNetworksCount]->layers;
            }

            neuralNetworks[neuralNetworksCount]->agent = new Agent();
            neuralNetworks[neuralNetworksCount]->agent->SetNN(neuralNetworks[neuralNetworksCount]);
            agents->Add(neuralNetworks[neuralNetworksCount]->agent);

            if (neuralNetworksCount>0)
            {
                    neuralNetworks[neuralNetworksCount]->Copy(neuralNetworks[0]);
                    neuralNetworks[neuralNetworksCount]->neurons->CopyResultsBuffers(neuralNetworks[0]->neurons->resultsBuffers);
            }

            ////neuralNetworks[neuralNetworksCount] = HH_NeuralNetwork::Load("neuralnetworks/nn3cols_8_3_8_3.txt");
            ////neuralNetworks[neuralNetworksCount] = HH_NeuralNetwork::Load("neuralnetworks/nn2cols_3_8_+1.txt");
            ////neuralNetworks[neuralNetworksCount] = HH_NeuralNetwork::Load("neuralnetworks/nn2cols_8_3.txt");
            ////original neuralNetworks[neuralNetworksCount] = HH_NeuralNetwork::Load("neuralnetworks/nn4cols_8_3_8_3.txt"); //changed bias value for 7 index -0.876268 to -0.79 and also for 1'st neuron of 3 in result layer: 2,1,0.712640 to  2,1,0.812640

            /*////
            neuralNetworks[neuralNetworksCount] = HH_NeuralNetwork::Load("neuralnetworks/nn2cols_8_3.txt");
        for(uint8_t i = 0; i<1; i++)
        {
            ////loadedNeuralNetwork2 = HH_NeuralNetwork::Load("neuralnetworks/nn2cols_3_8.txt");
            neuralNetworks[neuralNetworksCount]->AddLayersFromNN(loadedNeuralNetwork2);

            ////loadedNeuralNetwork2 = HH_NeuralNetwork::Load("neuralnetworks/nn2cols_8_3.txt");
            neuralNetworks[neuralNetworksCount]->AddLayersFromNN(loadedNeuralNetwork3);
        }
        */

            neuralNetworks[neuralNetworksCount++]->pos = pos;

            pos.y += (HH_NeuralNetwork::HEIGHT + HH_NeuralNetwork::Y_GAP);
        }

        pos.x += (HH_NeuralNetwork::WIDTH + HH_NeuralNetwork::X_GAP);
    }

    ////if (strlen(fileName)>0)
        ////Mutate();

    ////agents->AddNewNNHiddenLayerToAgents();
}

void HH_NeuralNetworks::Add(HH_NeuralNetwork *neuralNetwork)
{
    neuralNetworks[neuralNetworksCount] = neuralNetwork;

    Vector pos;

    pos.x = -Graph::WIDTH * (HH_NeuralNetworks::NETWORKS_COLUMNS/2);

    pos.y = -Graph::HEIGHT * (HH_NeuralNetworks::NETWORKS_ROWS/2);

    ////pos.y += neuralNetworksCount * (HH_NeuralNetwork::HEIGHT + HH_NeuralNetwork::Y_GAP);

    neuralNetworks[neuralNetworksCount]->pos = pos;


    neuralNetworksCount++;
}

uint32_t* HH_NeuralNetworks::GetFittestNNIndexes()
{
    double maxFitness = -1;

    for (uint32_t i = 0; i < neuralNetworksCount; i++)
    {
        if (neuralNetworks[i]->agent->fitness > maxFitness || maxFitness == -1)
        {
            maxFitness = neuralNetworks[i]->agent->fitness;
            fittestIndexes[0] = i;
        }
    }

    maxFitness = -1;

    for (uint32_t i = 0; i < neuralNetworksCount; i++)
    {
        if (i != fittestIndexes[0])
        {
            if (neuralNetworks[i]->agent->fitness > maxFitness || maxFitness == -1)
            {
                maxFitness = neuralNetworks[i]->agent->fitness;
                fittestIndexes[1] = i;
            }
        }
    }

    ////if (maxFitness == -1)
        fittestIndexes[1] = fittestIndexes[0];


    return &fittestIndexes[0];
}

/*uint32_t* HH_NeuralNetworks::GetFittestNNIndexes()
{
    double minError = -1;
    uint32_t* fittestIndexes = new uint32_t[2];

    for (uint32_t i = 0; i < neuralNetworksCount; i++)
    {
        if (neuralNetworks[i]->neurons->totalError < minError || minError == -1)
        {
            minError = neuralNetworks[i]->neurons->totalError;
            fittestIndexes[0] = i;
        }
    }

    minError = -1;

    for (uint32_t i = 0; i < neuralNetworksCount; i++)
    {
        if (i != fittestIndexes[0])
        {
            if (neuralNetworks[i]->neurons->totalError < minError || minError == -1)
            {
                minError = neuralNetworks[i]->neurons->totalError;
                fittestIndexes[1] = i;
            }
        }
    }

    if (minError == -1)
        fittestIndexes[1] = fittestIndexes[0];


    return fittestIndexes;
}
*/

void HH_NeuralNetworks::SetResultsBuffers(ResultsBuffer_ptr* srcBuffers)
{
    for (uint32_t i = 0; i < neuralNetworksCount; i++)
    {
        neuralNetworks[i]->neurons->SetResultsBuffers(srcBuffers);
    }
}

void HH_NeuralNetworks::CopyWeights(uint32_t* indexes)
{
    uint32_t startIndex = 0;
    uint32_t endIndex = neuralNetworksCount/2;

    for (uint32_t i = startIndex; i < endIndex; i++)
    {
        if (i != indexes[0])
            neuralNetworks[i]->Copy(neuralNetworks[indexes[0]]);
    }

    for (uint32_t i = endIndex; i < neuralNetworksCount; i++)
    {
        if (i != indexes[1])
            neuralNetworks[i]->Copy(neuralNetworks[indexes[1]]);
    }
}

void HH_NeuralNetworks::Mutate(uint32_t startLayerIndex, uint32_t endLayerIndex)
{
    for (uint32_t i = 1; i < neuralNetworksCount; i++)
    {
        neuralNetworks[i]->Mutate(startLayerIndex, endLayerIndex);
    }
}

/*////void HH_NeuralNetworks::ResetTotalErrorAndResultsBuffer()
{
    for (uint32_t i = 0; i < neuralNetworksCount; i++)
    {
        neuralNetworks[i]->neurons->error = 0;
        neuralNetworks[i]->neurons->totalError = 0;

        neuralNetworks[i]->neurons->ClearResultsBuffers();

        /*neuralNetworks[i]->electromagneticErrorForTemperature[neuralNetworks[i]->electromagneticErrorForTemperatureCount].error = neuralNetworks[i]->neurons->totalElectromagneticError;
        neuralNetworks[i]->electromagneticErrorForTemperature[neuralNetworks[i]->electromagneticErrorForTemperatureCount].temperature = neuralNetworks[i]->rfTemperature;

        neuralNetworks[i]->electromagneticErrorForTemperatureCount++;
        */
/*////
        neuralNetworks[i]->neurons->totalElectromagneticError = 0;

        neuralNetworks[i]->neurons->totalElectromagneticErrorCount = 0;


        for (uint8_t j = 0; j < HH_NeuralNetwork::LAYERS; j++)
    {
        neuralNetworks[i]->neurons->errorsToRefNeuronLayer[j].totalErrorDuration = 0;
        neuralNetworks[i]->neurons->errorsToRefNeuronLayer[j].totalSignallingDuration = 0;

        neuralNetworks[i]->neurons->errorsToRefNeuronLayer[j].currentErrorDurationStartTime = 0;
        neuralNetworks[i]->neurons->errorsToRefNeuronLayer[j].currentSignallingDurationStartTime = 0;

        neuralNetworks[i]->neurons->errorsToRefNeuronLayer[j].totalNeuronsInErrorCount = 0;
        neuralNetworks[i]->neurons->errorsToRefNeuronLayer[j].totalPatternErrorCount = 0;
    }

    }


    /*for (uint32_t j = 1; j < neuralNetworksCount; j++)
    {
        std::cout << "Neural Network: " << j << "\n";

        for (uint32_t i = 0; i < neuralNetworks[j]->electromagneticErrorForTemperatureCount; i++)
        {
            std::cout << "Electromagnetic Error For Temperature: " << neuralNetworks[j]->electromagneticErrorForTemperature[i].temperature << " = " << neuralNetworks[j]->electromagneticErrorForTemperature[i].error << "\n";
        }

        std::cout << "\n\n";
    }*/
////}

void HH_NeuralNetworks::SetTemperature(double temperature, int32_t index, int32_t layerIndex, int32_t neuronIndex)
{
    /*if (layerIndex == -1)
        neuralNetworks[index]->SetTemperature(temperature);
    else
        */
        if (index > -1 && index < neuralNetworksCount)
            neuralNetworks[index]->SetTemperature(temperature, layerIndex, neuronIndex);
        else
        {
            for (uint32_t i = 0; i < neuralNetworksCount; i++)
            {
                neuralNetworks[i]->SetTemperature(temperature);
            }
        }

}

void HH_NeuralNetworks::SetNoise(double noise, uint32_t index, int32_t layerIndex)
{
    if (layerIndex == -1)
        neuralNetworks[index]->SetNoise(noise);
    else
        neuralNetworks[index]->SetNoise(noise, layerIndex);
}

/*void HH_NeuralNetworks::SetQA(TrainingQA* currentQA, uint32_t index)
{
    neuralNetworks[index]->SetQA(currentQA);
}

uint32_t HH_NeuralNetworks::GetSignalingCount()
{
    uint32_t totalSignalingCount = 0;

    for (uint32_t i = 0; i < neuralNetworksCount; i++)
    {
        totalSignalingCount += neuralNetworks[i]->GetSignalingCount();
    }

    return totalSignalingCount;
}

uint32_t HH_NeuralNetworks::GetTotalSpikeCounts()
{
    uint32_t totalSpikeCounts = 0;

    for (uint32_t i = 0; i < neuralNetworksCount; i++)
    {
        totalSpikeCounts += neuralNetworks[i]->GetTotalSpikeCounts();
    }

    return totalSpikeCounts;
}*/

/*////void HH_NeuralNetworks::DetermineError(uint32_t index, bool print)
{
    neuralNetworks[index]->DetermineError(print);
}*/

/*////void HH_NeuralNetworks::DetermineErrorsForPreviousQA(uint32_t printIndex)
{
    for (uint32_t i = 0; i < neuralNetworksCount; i++)
    {
        neuralNetworks[i]->DetermineError(i == printIndex);
    }
}*/

/*void HH_NeuralNetworks::DetermineElectromagneticSignallingErrors(double Simulation::currentSimTime)
{
    if (Simulation::currentSimTime > 0)
    {
    ////double* voltages[HH_NeuralNetwork::LAYERS - 1][TrainingSet::resultLength];

    //double* voltages = new double_ptr[HH_NeuralNetwork::LAYERS - 1][TrainingSet::resultLength];

    //double* voltages = new double_ptr[HH_NeuralNetwork::LAYERS - 1][TrainingSet::resultLength]


    for (uint32_t i = 1; i < neuralNetworksCount; i++)
    {
        neuralNetworks[i]->DetermineElectromagneticSignallingErrors(neuralNetworks[0]->neurons, Simulation::currentSimTime);
    }
    }
}

/*void HH_NeuralNetworks::DetermineElectromagneticSpikeCountErrors()
{
    uint32_t spikeCounts[TrainingSet::resultLength];

    for (uint32_t i = 1; i < neuralNetworksCount; i++)
    {
        neuralNetworks[i]->DetermineElectromagneticSpikeCountErrors(neuralNetworks[0]->neurons->GetSpikeCounts(spikeCounts));
    }
}

void HH_NeuralNetworks::DetermineElectromagneticErrors()
{
    for (uint32_t i = 1; i < neuralNetworksCount; i++)
    {
        neuralNetworks[i]->DetermineElectromagneticErrors(neuralNetworks[0]->neurons->resultLayerSignalsBuffer);
    }
}

void HH_NeuralNetworks::SetLayer1Stimulus(char* layer1Stimulus, uint32_t index)
{
    neuralNetworks[index]->SetLayer1Stimulus(layer1Stimulus);
}*/

void HH_NeuralNetworks::Process()
{
    ///HH_NeuralNetworks::NEURAL_NOISE = (double) rand()/RAND_MAX * HH_NeuralNetworks::MAX_NEURAL_NOISE;

    uint32_t neuronsCount = neuralNetworks[0]->neurons->count;

    for (uint32_t i = 0; i < neuronsCount; i++)
    {
        HH_NeuralNetworks::NEURAL_NOISE[i] = (double) rand()/RAND_MAX * HH_NeuralNetworks::MAX_NEURAL_NOISE - HH_NeuralNetworks::MAX_NEURAL_NOISE/2;
    }

    for (uint32_t i = 0; i < neuralNetworksCount; i++)
    {
        neuralNetworks[i]->Process();

        /*if (simHH)
            uint8_t_ptr* layerSignaling = neuralNetworks[i]->neurons->GetLayersSignalingHH();
        else
            uint8_t_ptr* layerSignaling = neuralNetworks[i]->neurons->GetLayersSignaling();
            */
    }
}

void HH_NeuralNetworks::SaveVoltages()
{
    for (uint32_t i = 0; i < neuralNetworksCount; i++)
    {
        neuralNetworks[i]->neurons->SaveVoltages(voltagesFile);
    }
}

/*double HH_NeuralNetworks::GetAverageError()
{
    averageError = 0;

    for (uint32_t i = 0; i < neuralNetworksCount; i++)
    {
        averageError += neuralNetworks[i]->neurons->totalError;
    }

    averageError /= neuralNetworksCount;

    return averageError;
}*/

void HH_NeuralNetworks::Draw()
{
    if (visible)
    {
        for (uint32_t i = 0; i < neuralNetworksCount; i++)
        {
            ////if (i==0)
                neuralNetworks[i]->Draw();
        }
    }

    //averageError = GetAverageError();
}

void HH_NeuralNetworks::ResetHH()
{
    for (uint32_t i = 0; i < neuralNetworksCount; i++)
    {
        neuralNetworks[i]->ResetHH();
    }
}

void HH_NeuralNetworks::ResetSpikeCounts()
{
    for (uint32_t i = 0; i < neuralNetworksCount; i++)
    {
        neuralNetworks[i]->ResetSpikeCounts();
    }
}

HH_NeuralNetworks::~HH_NeuralNetworks()
{
    for (uint32_t i = 0; i < neuralNetworksCount; i++)
    {
        delete neuralNetworks[i];
    }

    delete neuralNetworks;
}

bool HH_NeuralNetworks::simHH = false;
bool HH_NeuralNetworks::electromagneticRF_HH = false;
////double HH_NeuralNetworks::MAX_NEURAL_NOISE = 0.00001;
double HH_NeuralNetworks::MAX_NEURAL_NOISE = 0.0;
////double HH_NeuralNetworks::MAX_NEURAL_NOISE = 0.0000019999999999999999 * 1.0;
////double HH_NeuralNetworks::MAX_NEURAL_NOISE = 0.00007;
////double HH_NeuralNetworks::MAX_NEURAL_NOISE = 0.000023;
////double HH_NeuralNetworks::MAX_NEURAL_NOISE = 0.000019;
////double HH_NeuralNetworks::MAX_NEURAL_NOISE = 0.1;
////double HH_NeuralNetworks::MAX_NEURAL_NOISE = 0.000016;
////double HH_NeuralNetworks::MAX_NEURAL_NOISE = 0.00000016;
double* HH_NeuralNetworks::NEURAL_NOISE = NULL;

double HH_NeuralNetworks::NEURAL_NOISE_DELTA = 0.000001;
