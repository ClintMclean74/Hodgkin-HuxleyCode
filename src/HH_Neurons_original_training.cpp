#include <string.h>

#include "GL/glew.h"
#include "GraphicsUtilities.h"
#include "HH_Neurons.h"
#include "HH_NeuralNetwork.h"
#include "HH_NeuralNetworks.h"

HH_Neurons::HH_Neurons(uint32_t size, void* neuralNetwork)
{
    this->size = size;
    this->neuralNetwork = neuralNetwork;

    //neurons = new HH_Neuron[this->size];
    neurons = new HH_Neuron_ptr[HH_NeuralNetwork::LAYERS];

    for (uint32_t i=0; i<HH_NeuralNetwork::LAYERS; i++)
    {
        neurons[i] = new HH_Neuron[HH_NeuralNetwork::ROWS[i]];
    }

    for (uint32_t i=0; i<HH_NeuralNetwork::LAYERS; i++)
    {
        for (uint32_t j=0; j<HH_NeuralNetwork::ROWS[i]; j++)
            neurons[i][j].neuralNetwork = neuralNetwork;
    }


    /*for (uint32_t i=0; i<size;i++)
    {
        neurons[i].neuralNetwork = neuralNetwork;
    }*/

    layerSignals = new uint8_t_ptr[HH_NeuralNetwork::LAYERS];

    for (uint32_t i=0; i<HH_NeuralNetwork::LAYERS;i++)
    {
        layerSignals[i] = new uint8_t[HH_NeuralNetwork::ROWS[0]];
    }

    layer1SignalsBuffer = new uint8_t[HH_NeuralNetwork::ROWS[0]];
    resultLayerSignalsBuffer = new uint8_t[HH_NeuralNetwork::ROWS[HH_NeuralNetwork::LAYERS-1]];

    layer1Activated = 0;

    /*for (uint32_t i=0; i<size;i++)
    {
        neurons[i].neuralNetwork = neuralNetwork;

        if (i>=10)
            neurons[i].connections.Add(i-10, 1.0);
    }*/

    /*for (uint32_t i=0; i<10;i++)
    {
        for (uint32_t j=10; j<20;j++)
            neurons[j].connections.Add(i, 0.00005);
    }

    for (uint32_t i=10; i<20;i++)
    {
        for (uint32_t j=20; j<30;j++)
            neurons[j].connections.Add(i, 0.00008);
    }
    */


    double rndWeight;
    double rndWeightScale = (10.0 / HH_NeuralNetwork::ROWS[1]) * 2.93;

    for (uint32_t i=1; i<HH_NeuralNetwork::LAYERS; i++)
    {
        for (uint32_t j=0; j<HH_NeuralNetwork::ROWS[i]; j++)
        {
            for (uint32_t k=0; k<HH_NeuralNetwork::ROWS[i-1]; k++)
            {
                rndWeight = (double) rand()/ RAND_MAX * 2 - 1;
                //neurons[i][j].connections.Add(i-1, k, rndWeight);
                //neurons[i][j].connections.Add(i, j, 0);
                neurons[i][j].connections.Add(i-1, k, 0);
            }
        }

    }

    /*uint32_t column1StartIndex, column1EndIndex, column2StartIndex, column2EndIndex;

    column1StartIndex = 0;
    column1EndIndex = column1StartIndex + HH_NeuralNetwork::ROWS;
    column2StartIndex = column1EndIndex;
    column2EndIndex = column2StartIndex + HH_NeuralNetwork::ROWS;

    for (uint32_t i=column1StartIndex; i<column1EndIndex;i++)
    {
        for (uint32_t j=column2StartIndex; j<column2EndIndex;j++)
        {
            //rndWeight = 0.00005 + (double) rand()/ RAND_MAX * 0.0000125;

             //rndWeight = 0.000016 + (double) rand()/ RAND_MAX * 0.00000125;
             //rndWeight *= rndWeightScale;

             rndWeight = (double) rand()/ RAND_MAX * 2 - 1;

             //neurons[j].connections.Add(i, rndWeight);
             //neurons[j].connections.Add(i, rndWeight);
             neurons[j].connections.Add(i, 0);
        }
    }

    /*neurons[column2StartIndex].connections.connections[0]->synapseConductance = 1;
    neurons[column2StartIndex].connections.connections[2]->synapseConductance = -1;
    neurons[column2StartIndex+1].connections.connections[0]->synapseConductance = 1;
    neurons[column2StartIndex+1].connections.connections[2]->synapseConductance = 1;
*/


/*    if (HH_NeuralNetwork::LAYERS > 2)
    {
    column1StartIndex += HH_NeuralNetwork::ROWS;
    column1EndIndex += HH_NeuralNetwork::ROWS;
    column2StartIndex += HH_NeuralNetwork::ROWS;
    column2EndIndex += HH_NeuralNetwork::ROWS;

    for (uint32_t i=column1StartIndex; i<column1EndIndex;i++)
    {
        for (uint32_t j=column2StartIndex; j<column2EndIndex;j++)
        {
            //rndWeight = 0.00005 + (double) rand()/ RAND_MAX * 0.000075;
            rndWeight = 0.00002 + (double) rand()/ RAND_MAX * 0.0000075;


            rndWeight *= rndWeightScale;
            neurons[j].connections.Add(i, rndWeight);
        }
    }
    }

    if (HH_NeuralNetwork::LAYERS > 3)
    {
    column1StartIndex += HH_NeuralNetwork::ROWS;
    column1EndIndex += HH_NeuralNetwork::ROWS;
    column2StartIndex += HH_NeuralNetwork::ROWS;
    column2EndIndex += HH_NeuralNetwork::ROWS;

    for (uint32_t i=column1StartIndex; i<column1EndIndex;i++)
    {
        for (uint32_t j=column2StartIndex; j<column2EndIndex;j++)
        {
            //rndWeight = 0.00005 + (double) rand()/ RAND_MAX * 0.00004;
            rndWeight = 0.00005 + (double) rand()/ RAND_MAX * 0.00008;
            //rndWeight = 0.00005 + (double) rand()/ RAND_MAX * 0.0001;

            rndWeight *= rndWeightScale;
            neurons[j].connections.Add(i, rndWeight);
        }
    }
    }

    if (HH_NeuralNetwork::LAYERS > 4)
    {
    column1StartIndex += HH_NeuralNetwork::ROWS;
    column1EndIndex += HH_NeuralNetwork::ROWS;
    column2StartIndex += HH_NeuralNetwork::ROWS;
    column2EndIndex += HH_NeuralNetwork::ROWS;

    for (uint32_t i=column1StartIndex; i<column1EndIndex;i++)
    {
        for (uint32_t j=column2StartIndex; j<column2EndIndex;j++)
        {
            rndWeight = 0.00005 + (double) rand()/ RAND_MAX * 0.00004;
            //rndWeight = 0.00005 + (double) rand()/ RAND_MAX * 0.00008;
            //rndWeight = 0.00005 + (double) rand()/ RAND_MAX * 0.0001;

            rndWeight *= rndWeightScale;
            neurons[j].connections.Add(i, rndWeight);
        }
    }
    }

    if (HH_NeuralNetwork::LAYERS > 5)
    {
    column1StartIndex += HH_NeuralNetwork::ROWS;
    column1EndIndex += HH_NeuralNetwork::ROWS;
    column2StartIndex += HH_NeuralNetwork::ROWS;
    column2EndIndex += HH_NeuralNetwork::ROWS;

    for (uint32_t i=column1StartIndex; i<column1EndIndex;i++)
    {
        for (uint32_t j=column2StartIndex; j<column2EndIndex;j++)
        {
            //rndWeight = 0.00005 + (double) rand()/ RAND_MAX * 0.00008;
            //rndWeight = 0.00005 + (double) rand()/ RAND_MAX * 0.0001;
            rndWeight = 0.00005 + (double) rand()/ RAND_MAX * 0.00004;

            rndWeight *= rndWeightScale;
            neurons[j].connections.Add(i, rndWeight);
        }
    }
    }
    */


    sampleIndex = 0;

    //double startY = -(size/2 * (Graph::HEIGHT + Graph::GAP));

    double temperature = 37;


    pos.x = 0;
    //pos.y = -(HH_NeuralNetwork::ROWS/2 * (Graph::HEIGHT + Graph::Y_GAP));

    /*for (uint32_t i=0; i<size;i++)
    {
        //neurons[i].SetPos(column * (Graph::WIDTH + Graph::GAP), startY + i * (Graph::HEIGHT + Graph::GAP), 0.0);
        neurons[i].SetPos(GetNewGraphX(i), GetNewGraphY(i), 0.0);

        //neurons[i].SetTemperature(temperature);

        //temperature += 2;
    }*/

    Vector pos;
    for (uint32_t i=0; i<HH_NeuralNetwork::LAYERS; i++)
    {
        for (uint32_t j=0; j<HH_NeuralNetwork::ROWS[i]; j++)
        {
            pos = GetNewGraphXY(i, j);
            neurons[i][j].SetPos(pos.x, pos.y, 0.0);
        }

    }

    currentQA = NULL;

    previousQAs = new TrainingQA_ptr[2];

    newLayer1Signals = false;

    error = 0;
    totalError = 0;
}

void HH_Neurons::CopyWeights(HH_Neurons* srcNeurons)
{
    for (uint32_t i=0; i<HH_NeuralNetwork::LAYERS; i++)
    {
        for (uint32_t j=0; j<HH_NeuralNetwork::ROWS[i]; j++)
        {
            neurons[i][j].CopyWeights(&srcNeurons->neurons[i][j]);
        }
    }
    /*for (uint32_t i=0; i<size;i++)
    {
        neurons[i].CopyWeights(&srcNeurons->neurons[i]);
    }*/
}

void HH_Neurons::Mutate(double rate)
{
    for (uint32_t i=0; i<HH_NeuralNetwork::LAYERS; i++)
    {
        for (uint32_t j=0; j<HH_NeuralNetwork::ROWS[i]; j++)
        {
            neurons[i][j].Mutate(rate);
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

void HH_Neurons::SetTemperature(double temperature)
{
    for (uint32_t i=0; i<HH_NeuralNetwork::LAYERS; i++)
    {
        for (uint32_t j=0; j<HH_NeuralNetwork::ROWS[i]; j++)
        {
            neurons[i][j].SetTemperature(temperature);
        }
    }

    /*for (uint32_t i=0; i<size;i++)
    {
        neurons[i].SetTemperature(temperature);
    }*/
}

void HH_Neurons::SetTemperature(double temperature, uint32_t column, uint32_t row)
{
    neurons[column][row].SetTemperature(temperature);
}

void HH_Neurons::SetSynStimulus(double stimulus)
{
    for (uint32_t i=0; i<HH_NeuralNetwork::LAYERS; i++)
    {
        for (uint32_t j=0; j<HH_NeuralNetwork::ROWS[i]; j++)
        {
            neurons[i][j].SetSynStimulus(stimulus);
        }
    }

    /*for (uint32_t i=0; i<size;i++)
    {
        neurons[i].SetSynStimulus(stimulus);
    }*/
}

void HH_Neurons::SetSynStimulus(double stimulus, uint32_t column, uint32_t row)
{
    neurons[column][row].SetSynStimulus(stimulus);
    //neurons[index].SetSynStimulus(stimulus);
}

void HH_Neurons::SetQA(TrainingQA* currentQA, bool setPrevious)
{
    if (setPrevious)
    {
        previousQAs[0] = previousQAs[1];
        previousQAs[1] = currentQA;
    }

    this->currentQA = currentQA;

    uint32_t qLength = strlen(currentQA->q);
    uint32_t n=0;

    for (uint32_t i=0; i<qLength;i++)
    {
        if (currentQA->q[i] == '1')
            //neurons[n++].setConductance = 0.01;
            //neurons[n++].setConductance = 0.1;
            neurons[0][n++].setConductance = HH_NeuralNetwork::Layer1ActiveSynapticConductance;
        else
            if (currentQA->q[i] == '0')
                neurons[0][n++].setConductance = 0.0;
    }

    /*if (totalError>0)
    {
    double value;

    for (uint32_t i=n; i<HH_NeuralNetwork::ROWS[0];i++)
    {
        value = (double) rand()/ RAND_MAX;

        if (value > 0.5)
            neurons[0][n++].setConductance = HH_NeuralNetwork::Layer1ActiveSynapticConductance;
        else
            neurons[0][n++].setConductance = 0;
    }
    }*/


    if (setPrevious)
        newLayer1Signals = true;
}

void HH_Neurons::DetermineError(bool print)
{
    if (HH_NeuralNetworks::simHH)
        GetLayersSignalingHH(print);
    else
        GetLayersSignaling(print);
}

void HH_Neurons::SetLayer1Stimulus(char *layer1Stimulus)
{
    uint32_t n=0;
    for (uint32_t i=0; i<strlen(layer1Stimulus);i++)
    {
        if (layer1Stimulus[i] == '1')
            neurons[0][n++].setConductance = 0.01;
        else
            if (layer1Stimulus[i] == '0')
                neurons[0][n++].setConductance = 0.0;
    }
}

void HH_Neurons::SetReceivingStimulus(bool value)
{
    //for (uint32_t i=0; i<size;i++)
    for (uint32_t i=0; i<HH_NeuralNetwork::ROWS[0];i++)
    {
        neurons[0][i].receivingStimulus = value;
    }

    for (uint32_t i=HH_NeuralNetwork::ROWS[0]; i<size;i++)
    {
        neurons[0][i].receivingStimulus = true;
    }
}

//void HH_Neurons::Process(double stimulus, double delatTime, uint32_t startStimulus, uint32_t endStimulus)
void HH_Neurons::Process(double delatTime)
{
    int  num_samples = 100000;

    for (uint32_t i=0; i<HH_NeuralNetwork::LAYERS; i++)
    {
        for (uint32_t j=0; j<HH_NeuralNetwork::ROWS[i]; j++)
            neurons[i][j].Process(delatTime);
    }

    //for (uint32_t i=0; i<size;i++)
        //neurons[i].Reset();
/*
    //for (sampleIndex = 0; sampleIndex < num_samples; sampleIndex++)
    {
        for (uint32_t i=0; i<size;i++)
        {
            //neurons[i].SetSynStimulus(stimulus);
            neurons[i].Process(delatTime);

            //if (i<4)
                //std::cout << "V: " << neurons[i].membraneVoltage << "\n";

        }
    }
    */

    //for (sampleIndex = 0; sampleIndex < num_samples; sampleIndex++)
    /*{
        //if (sampleIndex>=startStimulus && sampleIndex<=endStimulus)
            neurons[0].Process(delatTime, stimulus);
        //else
            //neurons[0].Process(delatTime, 0);

        /*membraneVoltageValues[j] = neurons[0].membraneVoltage;
        mValues[j] = sim.m;
        hValues[j] = sim.h;
        nValues[j] = sim.n;

        graph_NaCurrent[j] = sim.naCurrent;
        graph_KCurrent[j] = sim.kCurrent;
        graph_LCurrent[j] = sim.lCurrent;
        */
    //}

    //std::cout << "sampleIndex: " << sampleIndex << "\n";

    sampleIndex++;
}

Vector HH_Neurons::GetNewGraphXY(uint32_t column, uint32_t row)
{
    Vector newGraphXY;

	newGraphXY.x = ((HH_NeuralNetwork *) neuralNetwork)->pos.x;

	newGraphXY.x += (column * (Graph::WIDTH + Graph::X_GAP));


	newGraphXY.y = ((HH_NeuralNetwork *) neuralNetwork)->pos.y;

	newGraphXY.y += row * (Graph::HEIGHT + Graph::Y_GAP);

	return newGraphXY;
}


/*float HH_Neurons::GetNewGraphX(uint32_t neuronCount)
{
	float newGraphX = ((HH_NeuralNetwork *) neuralNetwork)->pos.x;

	columns = neuronCount / HH_NeuralNetwork::ROWS;

	newGraphX += (columns * (Graph::WIDTH + Graph::X_GAP));

	return newGraphX;
}

float HH_Neurons::GetNewGraphY(uint32_t neuronCount)
{
	float newGraphY = ((HH_NeuralNetwork *) neuralNetwork)->pos.y;

	columns = neuronCount / HH_NeuralNetwork::ROWS;

	neuronCount -= (columns * HH_NeuralNetwork::ROWS);

	int rowCount = neuronCount % HH_NeuralNetwork::ROWS;

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
    /*uint32_t columnStartIndex = column * HH_NeuralNetwork::ROWS;
    uint32_t columnEndIndex = columnStartIndex + HH_NeuralNetwork::ROWS;

    for (uint32_t i=columnStartIndex; i<columnEndIndex;i++)
    {
        neurons[i].AdjustAllSynapseConductance(value);
    }*/

    for (uint32_t i=0; i<HH_NeuralNetwork::ROWS[column]; i++)
        neurons[column][i].AdjustAllSynapseConductance(value);
}

uint8_t_ptr* HH_Neurons::GetLayersSignaling(bool print)
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

        for (uint32_t i=0; i<HH_NeuralNetwork::ROWS[0];i++)
            layer1SignalsBuffer[i] = layerSignals[0][i];

        error = currentQA->CheckQ(layer1SignalsBuffer);

        for (uint32_t i=0; i<HH_NeuralNetwork::ROWS[HH_NeuralNetwork::LAYERS-1];i++)
        {
            resultLayerSignalsBuffer[i] = layerSignals[HH_NeuralNetwork::LAYERS-1][i];
        }

        error = currentQA->CheckA(resultLayerSignalsBuffer, HH_NeuralNetwork::ROWS[HH_NeuralNetwork::LAYERS-1], print);
        totalError += error;


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

    return layerSignals;
}

uint8_t_ptr* HH_Neurons::GetLayersSignalingHH(bool print)
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
    if (newLayer1Signals)
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

        error = previousQAs[0]->CheckA(resultLayerSignalsBuffer, HH_NeuralNetwork::ROWS[HH_NeuralNetwork::LAYERS-1], print);

        totalError += error;

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

    return layerSignals;
}

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
    columnEndIndex = columnStartIndex + HH_NeuralNetwork::ROWS;

    for (uint32_t j=0; j<HH_NeuralNetwork::LAYERS;j++)
    {
        for (uint32_t i=columnStartIndex; i<columnEndIndex;i++)
        {
            signalingCounts[j] += neurons[i].spikeCount;
        }

        columnStartIndex += HH_NeuralNetwork::ROWS;
        columnEndIndex += HH_NeuralNetwork::ROWS;
    }

    /*if (signalingCounts[0] == HH_NeuralNetwork::ROWS)
        layer1Activated++;

    if (layer1Activated == 2)
    {
        for (uint32_t j=1; j<6;j++)
        {

            if (signalingCounts[j] > (HH_NeuralNetwork::ROWS/2))
                AdjustSynapseConductanceForColumn(j, -0.00001);
            else if (signalingCounts[j] < (HH_NeuralNetwork::ROWS/2))
                AdjustSynapseConductanceForColumn(j, 0.00001);
        }

            for (uint32_t i=0; i<size;i++)
            {
                neurons[i].spikeCount = 0;
            }

            layer1Activated = 0;
    }

    for (uint32_t i=0; i<HH_NeuralNetwork::ROWS;i++)
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
            if (signalingCounts[j] > (HH_NeuralNetwork::ROWS/2))
                AdjustSynapseConductanceForColumn(j, -0.0002);
            else if (signalingCounts[j] < (HH_NeuralNetwork::ROWS/2))
                AdjustSynapseConductanceForColumn(j, 0.0002);
        }
        else
        {
            int grc = 1;
        }*/

/*
            if (signalingCounts[j] == 0)
                AdjustSynapseConductanceForColumn(j, 0.0000001);
            else if (signalingCounts[j] > (HH_NeuralNetwork::ROWS/2))
                AdjustSynapseConductanceForColumn(j, -0.00001);
            else if (signalingCounts[j] < (HH_NeuralNetwork::ROWS/2))
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

Vector HH_Neurons::GetMinMaxWeight()
{
    Vector minMaxWeight, weight;

    for (uint32_t j=0; j<HH_NeuralNetwork::LAYERS;j++)
        {
            for (uint32_t i=0; i<HH_NeuralNetwork::ROWS[j];i++)
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

    sprintf(graphicsText, "Temperature %.2f C, Error: %.2f, Total Error: %.2f", neurons[0][0].Tc, error, totalError);
    //GraphicsUtilities::DrawText2(graphicsText, ((HH_NeuralNetwork *) neuralNetwork)->pos.x, ((HH_NeuralNetwork *) neuralNetwork)->pos.y + 1000, 0.0, 0.7);
    GraphicsUtilities::DrawText2(graphicsText, ((HH_NeuralNetwork *) neuralNetwork)->pos.x, ((HH_NeuralNetwork *) neuralNetwork)->pos.y - 150, 0.0, 0.7);

    //sprintf(graphicsText, "Total Signaling Count for Layers");
    //GraphicsUtilities::DrawText2(graphicsText, GetNewGraphX(size-1) + Graph::X_GAP, ((HH_NeuralNetwork *) neuralNetwork)->pos.y + 200, 0.0, 0.9);

    //sprintf(graphicsText, "L1: %.d, L2: %.d", signalingCounts[0], signalingCounts[1], signalingCounts[2], signalingCounts[3], signalingCounts[4], signalingCounts[5]);
    //GraphicsUtilities::DrawText2(graphicsText, GetNewGraphX(size-1) + Graph::X_GAP, ((HH_NeuralNetwork *) neuralNetwork)->pos.y, 0.0, 0.9);

    Vector minMaxWeight = GetMinMaxWeight();

        for (uint32_t j=0; j<HH_NeuralNetwork::LAYERS;j++)
        {
            for (uint32_t i=0; i<HH_NeuralNetwork::ROWS[j];i++)
            {
                neurons[j][i].Draw(minMaxWeight);
            }
        }
}

void HH_Neurons::ResetHH()
{
    for (uint32_t j=0; j<HH_NeuralNetwork::LAYERS;j++)
        {
            for (uint32_t i=0; i<HH_NeuralNetwork::ROWS[j];i++)
            {
                neurons[j][i].ResetHH();
            }
        }
}

HH_Neurons::~HH_Neurons()
{
    delete [] neurons;
}
