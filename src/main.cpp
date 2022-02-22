/*
//HH_Sim

6.3 C = 2 spikes
11.17 = 3 spikes

stimulus = 20 nA for 10 msec
*/


#include <iostream>
#include <cmath>
#include <process.h>
#include <time.h>
#include <tgmath.h>

#include "GL/glew.h"
#include "GL/glut.h"
#include "HH_NeuralNetworks.h"
#include "Graph.h"
#include "GraphicsUtilities.h"
#include "TrainingSet.h"
#include "FitnessGraph.h"
#include "Agents.h"
#include "FoodForAgents.h"
#include "Simulation.h"
#include "Environment.h"

typedef double* double_ptr;
double minGraphHeight, maxGraphHeight;
double minGraphX = 0, maxGraphX = Graph::MAX_GRAPH_INDEX;
double graphMembraneVoltageData[Graph::MAX_GRAPH_INDEX];
double graph_m[Graph::MAX_GRAPH_INDEX];
double graph_h[Graph::MAX_GRAPH_INDEX];
double graph_n[Graph::MAX_GRAPH_INDEX];
double graph_NaCurrent[Graph::MAX_GRAPH_INDEX];
double graph_KCurrent[Graph::MAX_GRAPH_INDEX];
double graph_LCurrent[Graph::MAX_GRAPH_INDEX];
////double Simulation::deltaTime = 0.012;
double num_samples = 100000;

HH_NeuralNetworks *neuralNetworks;
HH_NeuralNetwork* loadedNeuralNetwork = NULL;
HH_NeuralNetwork* loadedNeuralNetwork2 = NULL;

FitnessGraph* fitnessGraph;
FitnessGraph* errorGraph;
FitnessGraph* errorLayerPercentageGraph;

double startGenerationTime = 0;
double startStimulusTime = 0;
////double generationDuration = 300;

uint32_t simCount = 0;
double rfErrorDuration = 0;
double rfSignallingDuration = 0;
double rfAvgNeuronErrorCount = 0;

double rfStimulusDuration = 1;
double rfStimulusInterval = 1;

bool processing = true;

char graphicsText[255];

char layer1Stimulus[4];

double referenceTemperature = 37.0;
double simTemperatures[10];
uint16_t simTemperaturesLength = 10;
uint16_t currentSimTemperatureIndex = 0;


double startSimTimeForTemperature;
double simDurationForTemperature = 300;

TrainingSet *trainingSet;
Vector pos(730,-3820,-15000);

double xRot = 0;
double yRot = 0;

double rotScale = 0.2;
double translateScale = 25;
double zoomScale = 1;

double fov = 44;

int prevX = -1;
int prevY = -1;

int currentButton = 0;

bool rfStimulus = false;

bool incNoise = false;
bool decNoise = false;
bool resetNoise = false;
bool prevIncNoise = false;

bool incDelay = false;
bool decDelay = false;

double maxSignalsCount = 0;

bool incStimulus = false;
bool decStimulus = false;
bool resetStimulus = false;

bool activate = false;
////double rfStimulusTemperature = 337.5;
////double rfStimulusTemperature = 37.5;
////double rfStimulusTemperature = 37.0;
////double rfStimulusTemperature = 37.001; //works for 30 x 9_30 //+0.1 of a degree, 37.101, 37.201, 37.301...results in essentially same layer start, 37.001 significantly later, further research 37.002, 37.003, 37.004...
////double rfStimulusTemperature = 48.01;
////double rfStimulusTemperature = 38.0;
////double rfStimulusTemperature = 37.000001;
////double rfStimulusTemperature = 37.0;

////double rfStimulusTemperatureStart = 337.5;
////double rfStimulusTemperatureInc = 0.001; //works for 30 x 9_30 //+0.1 of a degree, 37.101, 37.201, 37.301...results in essentially same layer start, 37.001 significantly later, further research 37.002, 37.003, 37.004...
////double rfStimulusTemperatureInc = 30.0;
////double rfStimulusTemperatureInc = 0.029;
////double rfStimulusTemperatureInc = 0.0001; //works for 30 x 9_30 //+0.0001 of a degree, 37.0001, 37.0001...37.0009
////double rfStimulusTemperatureStart = 37.00004; /////0.00007 starts effect
////double rfStimulusTemperatureInc = 0.00001; /////0.00007 starts effect
////double rfStimulusTemperatureStart = 37.0000001;
////double rfStimulusTemperatureInc = 0.0000001;

/*////
double rfStimulusTemperatureStart = 37.0001;
double rfStimulusTemperatureInc = 0.0001;
*/
////double rfStimulusTemperatureStart = 37.00004; //starts effect



double rfStimulusTemperatureStart = 37.00001;
double rfStimulusTemperatureInc = 0.00001;



/*////
double rfStimulusTemperatureStart = 48.0;
double rfStimulusTemperatureInc = 10;

double rfStimulusTemperatureStart = 37.0;
double rfStimulusTemperatureInc = 0;
*/


double noise = 0;

double signalsCountInterval = 100;
double startSignalsCountTime = 0;

uint32_t evolvedStage = 0;

uint32_t totalError = 0;
double totalFitness = 0;
double avgFitness = 0;

Agents* agents;
FoodForAgents* foodForAgents;

bool drawingGraphics = true;

bool decSpeed = false;
bool incSpeed = false;

////double Simulation::lag = 0;

////bool evolving = true;

double_ptr* electromagneticErrorsForLayerTotal;
double_ptr* electromagneticErrorPercentageForLayerTotal;
uint32_t electromagneticErrorForLayerAdditionCount = 0;

char errorGraphFileName[255];
char errorLayerPercentageGraphFileName[255];

bool clearScreen = true;

char textBuffer[255];

void display(void)
{
    /*////glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    double winWidth = glutGet(GLUT_WINDOW_WIDTH);
    double winHeight = glutGet(GLUT_WINDOW_HEIGHT);

	gluPerspective(fov, winWidth/ winHeight, 1.0, 20000.0);


    glMatrixMode(GL_MODELVIEW);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();

    glTranslatef(pos.x, pos.y, pos.z);
    glRotatef(45, 0.0, 0.0, 1.0);

    ////glColor3f(neuronActiveColor.r, neuronActiveColor.g, neuronActiveColor.b);

    glScalef(1.0, 1.0, 1.0);

    glBegin(GL_LINES);
        glVertex3f(0, 300, 0);
        glVertex3f(200, 0, 0);

        glVertex3f(200, 0, 0);
        glVertex3f(0, -300, 0);

        glVertex3f(0, -300, 0);
        glVertex3f(0, 300, 0);
    glEnd();

	/*///glBegin(GL_LINE_STRIP);
/*        glVertex3f(0, 300, 0);
        glVertex3f(200, 0, 0);
        glVertex3f(0, -300, 0);
        glVertex3f(0, 300, 0);
    glEnd();*/
/*
    glutSwapBuffers();


*/
    if (drawingGraphics)
    {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

	gluPerspective(fov, (GLdouble) glutGet(GLUT_WINDOW_WIDTH) / glutGet(GLUT_WINDOW_HEIGHT), 1.0, 20000.0);

	glMatrixMode(GL_MODELVIEW);

    ////if (clearScreen)
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        ////clearScreen = false;
    }

    glLoadIdentity();

	glTranslatef(pos.x, pos.y, pos.z);

	/*glColor3f(1, 1, 1);
    sprintf(graphicsText, "Training Set Index: %i", trainingSet->currentQAIndex);
    GraphicsUtilities::DrawTextStr(graphicsText, 0, 3700, 0.0, 3.0);
    */

    HH_NeuralNetwork* neuralNetwork = neuralNetworks->neuralNetworks[0];
    //uint8_t_ptr* layerSignaling = neuralNetwork->neurons->GetLayersSignaling();

        glColor3f(1, 1, 1);

        double textX = -1500;
        double textY = 800;

        //sprintf(graphicsText, "L1N0: %d, L1N1: %i, L1N2: %i, L1N3: %i", layerSignaling[0][0], layerSignaling[0][1], layerSignaling[0][2], layerSignaling[0][3]);
        //GraphicsUtilities::DrawTextStr(graphicsText, textX, textY + 600, 0.0, 0.9);

        /*////sprintf(graphicsText, "L1BufferN0: %d, L1BufferN1: %i, L1BufferN2: %i, L1BufferN3: %i", neuralNetwork->neurons->layer1SignalsBuffer[0], neuralNetwork->neurons->layer1SignalsBuffer[1], neuralNetwork->neurons->layer1SignalsBuffer[2], neuralNetwork->neurons->layer1SignalsBuffer[3]);
        GraphicsUtilities::DrawTextStr(graphicsText, textX, textY + 400, 0.9);

        //sprintf(graphicsText, "L2N0: %d, L2N1: %i, L2N2: %i, L2N3: %i", layerSignaling[1][0], layerSignaling[1][1], layerSignaling[1][2], layerSignaling[1][3]);
        //GraphicsUtilities::DrawTextStr(graphicsText, textX, textY + 200, 0.0, 0.9);

        sprintf(graphicsText, "L2BufferN0: %d, L2BufferN1: %i, L2BufferN2: %i, L2BufferN3: %i", neuralNetwork->neurons->resultLayerSignalsBuffer[0], neuralNetwork->neurons->resultLayerSignalsBuffer[1], neuralNetwork->neurons->resultLayerSignalsBuffer[2], neuralNetwork->neurons->resultLayerSignalsBuffer[3]);
        GraphicsUtilities::DrawTextStr(graphicsText, textX, textY + 200, 0.0, 0.9);
        */

        /*
        glColor3f(1, 1, 1);
        sprintf(graphicsText, "Average Error: %.2f", neuralNetworks->averageError);
        GraphicsUtilities::DrawTextStr(graphicsText, -1500, -1500, 0.0, 3.0);
        */

        glColor3f(1, 1, 1);
        ////sprintf(graphicsText, "Training Index, Sub Set Size: %d, %d, Average Error: %.2f", trainingSet->currentQAIndex, trainingSet->currentSubSetSize, neuralNetworks->averageError);
        ////sprintf(graphicsText, "Training Index, Sub Set Size: %d, %d, Electromagnetic(RF) SpikeCount Error: %.3f", trainingSet->currentQAIndex, trainingSet->currentSubSetSize, neuralNetworks->neuralNetworks[1]->neurons->totalElectromagneticError);
        ////sprintf(graphicsText, "Training Index, Sub Set Size: %d, %d, Electromagnetic(RF) SpikeCount Error Avg %%: %.3f", trainingSet->currentQAIndex, trainingSet->currentSubSetSize, neuralNetworks->neuralNetworks[1]->neurons->totalElectromagneticErrorAvg * 100);

        ////rfErrorDuration = neuralNetworks->neuralNetworks[1]->neurons->errorsToRefNeuronLayer[HH_NeuralNetwork::LAYERS-1].totalErrorDuration;

        /*sprintf(graphicsText, "Training Index, Sub Set Size: %d, %d, Electromagnetic(RF) Error Avg %%: %.3f", trainingSet->currentQAIndex, trainingSet->currentSubSetSize, neuralNetworks->neuralNetworks[1]->neurons->errorsToRefNeuronLayer[HH_NeuralNetwork::LAYERS-1]);
        GraphicsUtilities::DrawTextStr(graphicsText, -1500, -1500, 0.0, 3.0);

        sprintf(graphicsText, "Training Index, Sub Set Size: %d, %d, Electromagnetic(RF) Error Avg %%: %.3f", trainingSet->currentQAIndex, trainingSet->currentSubSetSize, rfErrorDuration);
        GraphicsUtilities::DrawTextStr(graphicsText, -1500, -1800, 0.0, 3.0);
        */

        /*////if (neuralNetworks->neuralNetworks[1]->neurons->errorsToRefNeuronLayer[HH_NeuralNetwork::LAYERS-1].currentErrorDurationStartTime > 0)
            rfErrorDuration += ((Simulation::currentSimTime - TrainingSet::InitializationInterval) - neuralNetworks->neuralNetworks[1]->neurons->errorsToRefNeuronLayer[HH_NeuralNetwork::LAYERS-1].currentErrorDurationStartTime);

            rfErrorDuration = rfErrorDuration/(Simulation::currentSimTime - TrainingSet::InitializationInterval) * 100;
            */

        ////sprintf(graphicsText, "Training Index, Sub Set Size: %d, %d, Electromagnetic(RF) Error Avg %%: %.3f", trainingSet->currentQAIndex, trainingSet->currentSubSetSize, rfErrorDuration);


        ////double rfAvgNeuronErrorCount = neuralNetworks->neuralNetworks[1]->neurons->errorsToRefNeuronLayer[HH_NeuralNetwork::LAYERS-1].totalNeuronsInErrorCount / neuralNetworks->neuralNetworks[1]->neurons->errorsToRefNeuronLayer[HH_NeuralNetwork::LAYERS-1].totalPatternErrorCount;

        /*////uint32_t spikeCounts[TrainingSet::resultLength];
        neuralNetworks->neuralNetworks[0]->neurons->GetSpikeCounts(spikeCounts);

        double avg1 = 0;
        for(uint8_t i=0; i<TrainingSet::resultLength;i++)
            avg1 += spikeCounts[i];

        avg1 /= TrainingSet::resultLength;

        double spikeCountsPerSecondRefTemperature = 1000/Simulation::currentSimTime * avg1;

        double avg2 = 0;
        if (neuralNetworks->neuralNetworksCount>1)
        {
        neuralNetworks->neuralNetworks[1]->neurons->GetSpikeCounts(spikeCounts);

        for(uint8_t i=0; i<TrainingSet::resultLength;i++)
            avg2 += spikeCounts[i];

        avg2 /= TrainingSet::resultLength;
        }

        double spikeCountsPerSecondAdjSimTemperature = 1000/Simulation::currentSimTime * avg2;

        ////fitnessGraph->SetSpikesPerSecond(spikeCountsPerSecondAdjSimTemperature, currentSimTemperatureIndex);


        ////sprintf(graphicsText, "Training Index, Sub Set Size: %d, %d, Electromagnetic(RF) Avg Neuron Error Count %: %.3f, Electromagnetic(RF) Avg Neuron Error Count Percentage %: %.3f", trainingSet->currentQAIndex, trainingSet->currentSubSetSize, rfAvgNeuronErrorCount, rfAvgNeuronErrorCount / HH_NeuralNetwork::ROWS[HH_NeuralNetwork::LAYERS-1] * 100);
        sprintf(graphicsText, "Training Index, Sub Set Size: %d, %d, Simulation Time: %f, Electromagnetic(RF) Avg Neuron Error Count : %.3f, Electromagnetic(RF) Avg Neuron Error Count Percentage %: %.3f, Electromagnetic(RF) Avg Error Duration %: %.3f", trainingSet->currentQAIndex, trainingSet->currentSubSetSize, Simulation::currentSimTime, rfAvgNeuronErrorCount, rfAvgNeuronErrorCount / HH_NeuralNetwork::ROWS[HH_NeuralNetwork::LAYERS-1] * 100, rfErrorDuration);
        //sprintf(graphicsText, "Training Index, Sub Set Size: %d, %d, Simulation Time: %f, Total Avg Spike Counts: %f, Electromagnetic(RF) Avg Neuron Error Count : %.3f, Electromagnetic(RF) Avg Neuron Error Count Percentage %: %.3f, Electromagnetic(RF) Avg Error Duration %: %.3f", trainingSet->currentQAIndex, trainingSet->currentSubSetSize, Simulation::currentSimTime, avg, rfAvgNeuronErrorCount, rfAvgNeuronErrorCount / HH_NeuralNetwork::ROWS[HH_NeuralNetwork::LAYERS-1] * 100, rfErrorDuration);
        GraphicsUtilities::DrawTextStr(graphicsText, -1500, -2100, 0.0, 3.0);

        //sprintf(graphicsText, "Total Avg Spike Counts NN 37.0 C: %f, NN %f C: %f", avg1, currentSimTemperature, avg2);
        //GraphicsUtilities::DrawTextStr(graphicsText, -1500, -2400, 0.0, 3.0);

        sprintf(graphicsText, "Avg Spike Counts per Second NN %f C: %f, NN %f C: %f", referenceTemperature, spikeCountsPerSecondRefTemperature, simTemperatures[currentSimTemperatureIndex], spikeCountsPerSecondAdjSimTemperature);
        GraphicsUtilities::DrawTextStr(graphicsText, -1500, -2700, 0.0, 3.0);

        sprintf(graphicsText, "Total Signaling Duration: %f", rfSignallingDuration);
        GraphicsUtilities::DrawTextStr(graphicsText, -1500, -3000, 0.0, 3.0);

        sprintf(graphicsText, "HH_NeuralNetworks::MAX_NEURAL_NOISE(* 1000): %f", HH_NeuralNetworks::MAX_NEURAL_NOISE * 1000);
        GraphicsUtilities::DrawTextStr(graphicsText, -1500, -3300, 0.0, 3.0);

        /*uint32_t signalsCount = neuralNetworks->neuralNetworks[1]->GetSignalingCount();

        if (signalsCount > maxSignalsCount)
            maxSignalsCount = signalsCount;

        sprintf(graphicsText, "Signals Count : %d", signalsCount);
        GraphicsUtilities::DrawTextStr(graphicsText, -1500, -3600, 0.0, 3.0);

        fitnessGraph->AddPoint((double) signalsCount, 0);
        */


        ////sprintf(graphicsText, "Total Error for fittest: %d", totalError);
        ////GraphicsUtilities::DrawTextStr(graphicsText, -1500, -3600, 0.0, 3.0);

        sprintf(graphicsText, "NN Layers: %d", neuralNetworks->neuralNetworks[0]->neurons->layers);
        GraphicsUtilities::DrawTextStr(graphicsText, -1500, -3600, 0.0, 3.0);

        sprintf(graphicsText, "Avg Fitness: %f", avgFitness);
        GraphicsUtilities::DrawTextStr(graphicsText, -1500, -4000, 0.0, 3.0);

        sprintf(graphicsText, "Generations: %i, Simulation Time: %f, Real Time: %i", Simulation::generations, Simulation::currentSimTime, (GetTickCount() - Simulation::simStartRealTime)/1000);
        GraphicsUtilities::DrawTextStr(graphicsText, -1500, -2100, 0.0, 3.0);

        sprintf(graphicsText, "Generation Duration: %f", Simulation::generationDuration);
        GraphicsUtilities::DrawTextStr(graphicsText, -1500, -4400, 0.0, 3.0);

        sprintf(graphicsText, "Noise Level: %f", HH_NeuralNetworks::MAX_NEURAL_NOISE);
        GraphicsUtilities::DrawTextStr(graphicsText, -1500, -4800, 0.0, 3.0);

        sprintf(graphicsText, "Stimulus Temperature Start: %f, Stimulus Temperature Increment: %f", rfStimulusTemperatureStart, rfStimulusTemperatureInc);
        GraphicsUtilities::DrawTextStr(graphicsText, -1500, -5200, 0.0, 3.0);

        ////fitnessGraph->AddPoint((double) totalError, 0);


        ////fitnessGraph->AddPoint(rfErrorDuration);


        ////Simulation::nnVisible = false;
        if (Simulation::nnVisible)
            neuralNetworks->Draw();

        glTranslatef(0, 0, 1);

        Environment::foodForAgents_ptr->Draw();
        agents->Draw();


    //if (loadedNeuralNetwork)
        //loadedNeuralNetwork->Draw();

    //neuralNetwork->Draw();


    fitnessGraph->Draw();
    errorGraph->Draw();
    errorLayerPercentageGraph->Draw();

	glutSwapBuffers();
    }
    glutPostRedisplay();
}

/*void display2(void)
{
    simCount++;

    if (simCount % 10 == 0)
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glLoadIdentity();

		glTranslatef(0,0, -10);

		uint32_t* signalingCounts = neuralNetwork->neurons->GetLayersSignalingCount();
		uint32_t* signalingCounts2 = neuralNetwork2->neurons->GetLayersSignalingCount();

		double signalingCountRatios[6];

		for (int i=0; i<6;i++)
        {
            if (signalingCounts[i] != 0 && signalingCounts2[i] != 0)
                signalingCountRatios[i] = (double) signalingCounts2[i]/signalingCounts[i] * 100;
            else
                signalingCountRatios[i] = 0;
        }

        uint8_t_ptr* layerSignaling = neuralNetwork->neurons->GetLayersSignaling();

        glColor3f(1, 1, 1);

        sprintf(graphicsText, "L1N0: %d, L1N1: %i, L1N2: %i, L1N3: %i", layerSignaling[0][0], layerSignaling[0][1], layerSignaling[0][2], layerSignaling[0][3]);
        GraphicsUtilities::DrawTextStr(graphicsText, 1500, 1900, 0.0, 0.9);

        sprintf(graphicsText, "L1BufferN0: %d, L1BufferN1: %i, L1BufferN2: %i, L1BufferN3: %i", neuralNetwork->neurons->layer1SignalsBuffer[0], neuralNetwork->neurons->layer1SignalsBuffer[1], neuralNetwork->neurons->layer1SignalsBuffer[2], neuralNetwork->neurons->layer1SignalsBuffer[3]);
        GraphicsUtilities::DrawTextStr(graphicsText, 1500, 1600, 0.0, 0.9);

        sprintf(graphicsText, "L2N0: %d, L2N1: %i, L2N2: %i, L2N3: %i", layerSignaling[1][0], layerSignaling[1][1], layerSignaling[1][2], layerSignaling[1][3]);
        GraphicsUtilities::DrawTextStr(graphicsText, 1500, 1300, 0.0, 0.9);

        sprintf(graphicsText, "L2BufferN0: %d, L2BufferN1: %i, L2BufferN2: %i, L2BufferN3: %i", neuralNetwork->neurons->resultLayerSignalsBuffer[0], neuralNetwork->neurons->resultLayerSignalsBuffer[1], neuralNetwork->neurons->resultLayerSignalsBuffer[2], neuralNetwork->neurons->resultLayerSignalsBuffer[3]);
        GraphicsUtilities::DrawTextStr(graphicsText, 1500, 1100, 0.0, 0.9);

        neuralNetwork->Draw();
        neuralNetwork2->Draw();
    }


	/*    glLineWidth(4);

        glColor3f(0, 1, 0);
        glBegin(GL_LINES);
		for(uint32_t i = 0; i < Graph::MAX_GRAPH_INDEX; i++)
        {
			glVertex2f(i, graphMembraneVoltageData[i]);

			//std::cout << "V: " << graphMembraneVoltageData[j]<< "\n";
		}
		glEnd();


		glLineWidth(2);

        /*
        glPushMatrix();
        glScalef(1, 100, 1);

        glColor3f(0, 1, 0);
		glBegin(GL_LINES);
		for(uint32_t i = 0; i < Graph::MAX_GRAPH_INDEX; i++)
        {
			glVertex2f(i, graph_m[i]);
		}
		glEnd();


		glColor3f(0, 0, 1);
		glBegin(GL_LINES);
		for(uint32_t i = 0; i < Graph::MAX_GRAPH_INDEX; i++)
        {
			glVertex2f(i, graph_n[i]);
		}
		glEnd();

        glColor3f(1, 0, 1);
		glBegin(GL_LINES);
		for(uint32_t i = 0; i < Graph::MAX_GRAPH_INDEX; i++)
        {
			glVertex2f(i, graph_h[i]);
		}
		glEnd();

		glPopMatrix();
*/

/*        glScalef(1, 700, 1);

        glColor3f(1, 0, 0);
		glBegin(GL_LINES);
		for(uint32_t i = 0; i < Graph::MAX_GRAPH_INDEX; i++)
        {
			glVertex2f(i, graph_NaCurrent[i]);
		}
		glEnd();

        glColor3f(1, 0.75, 0.25);
		glBegin(GL_LINES);
		for(uint32_t i = 0; i < Graph::MAX_GRAPH_INDEX; i++)
        {
			glVertex2f(i, graph_KCurrent[i]);
		}
		glEnd();

        glColor3f(1, 1, 0);
		glBegin(GL_LINES);
		for(uint32_t i = 0; i < Graph::MAX_GRAPH_INDEX; i++)
        {
			glVertex2f(i, graph_LCurrent[i]);
		}
		glEnd();



/*		glColor3f(0, 1, 0);
		glBegin(GL_LINES);
		for(uint32_t i = 0; i < Graph::MAX_GRAPH_INDEX; i++)
        {
			glVertex2f(i, graph_NaCurrent[i] + graph_KCurrent[i] + graph_LCurrent[i]);
		}
		glEnd();
*/

/*
	glutSwapBuffers();

	glutPostRedisplay();
}
*/

void MouseButtons(int button, int state, int x, int y)
{
	currentButton = button;

	if (button == 1) // It's a wheel event
	{
		switch (state)
		{
			case(GLUT_UP):
			break;
			case(GLUT_DOWN):
			break;
		}
	}
	else
	// Wheel reports as button 3(scroll up) and button 4(scroll down)
	if (button == 3 || button == 4) // It's a wheel event
	{
		// Each wheel event reports like a button click, GLUT_DOWN then GLUT_UP
		if (state == GLUT_UP) return; // Disregard redundant GLUT_UP events

		if (button == 3)
			fov-=zoomScale;
		else
			fov+= zoomScale;

        if (fov>32)
            NN_Connections::MAX_GRAPHICS_CONNECTION_THICKNESS = NN_Connections::MAX_GRAPHICS_CONNECTION_THICKNESS_FAR;
        else
            NN_Connections::MAX_GRAPHICS_CONNECTION_THICKNESS = NN_Connections::MAX_GRAPHICS_CONNECTION_THICKNESS_NEAR;
	}

	if (button == GLUT_LEFT_BUTTON)
	{
		prevX = x;
		prevY = y;

		if (state == GLUT_DOWN)
		{
		}
		else
			if (state == GLUT_UP)
			{
			}
	}
	else
		if (button == GLUT_RIGHT_BUTTON)
		{
			if (state == GLUT_UP)
			{
			}
		}
}

void MouseMotion(int x, int y)
{
	if (currentButton == GLUT_LEFT_BUTTON)
	{
	}
	else
        if (currentButton == GLUT_RIGHT_BUTTON)
        {
            xRot += (y - prevY) * rotScale;
            yRot += (x - prevX) * rotScale;
        }
        else if (currentButton == GLUT_MIDDLE_BUTTON)
        {
            pos.x += (x - prevX)*translateScale;
            pos.y -= (y - prevY)*translateScale;
        }

	prevX = x;
	prevY = y;
}

void PassiveMouseMotion(int x, int y)
{
    prevX = x;
	prevY = y;
}

void ProcessKey(unsigned char key, int x, int y)
{
        switch (key)
		{
		    case ('e'):
                Simulation::trainingOrEvolution = !Simulation::trainingOrEvolution;
			break;
			case ('g'):
                Simulation::nnVisible = !Simulation::nnVisible;
			break;
			case ('G'):
                Simulation::generationDuration += 10;

                if (Simulation::generationDuration > 3000)
                    Simulation::generationDuration = 10;
			break;
			case ('f'):
			    if (FoodForAgents::maxFoodPerAgent > 0)
                    FoodForAgents::maxFoodPerAgent--;
			break;
			case ('F'):
                FoodForAgents::maxFoodPerAgent++;
			break;
		    case ('a'):
                activate = !activate;
			break;
		case ('s'):
                rfStimulus = !rfStimulus;
                Simulation::stimulus = !Simulation::stimulus;
			break;
        case ('N'):
                incNoise = true;
			break;
        case ('n'):
                decNoise = true;
			break;
        case ('R'):
                resetNoise = true;
			break;
        case ('D'):
                incDelay = true;
			break;
        case ('d'):
                decDelay= true;
			break;
        case (';'):
                incStimulus = true;
			break;
        case ('.'):
                decStimulus = true;
			break;
        case (','):
                resetStimulus = true;
			break;
        case ('r'):
                neuralNetworks->ResetHH();
                ////neuralNetworks->ResetTotalErrorAndResultsBuffer();
			break;
        case ('0'):
                trainingSet->currentQAIndex = 0;
			break;
			case ('1'):
                trainingSet->currentQAIndex = 1;
			break;
			case ('2'):
                trainingSet->currentQAIndex = 2;
			break;
			case ('3'):
                trainingSet->currentQAIndex = 3;
			break;
			case ('4'):
                trainingSet->currentQAIndex = 4;
			break;
			case ('5'):
                trainingSet->currentQAIndex = 5;
			break;
			case ('6'):
                trainingSet->currentQAIndex = 6;
			break;
			case ('7'):
                trainingSet->currentQAIndex = 7;
			break;
        case ('>'):
                incSpeed = true;
		break;
        case ('<'):
                decSpeed = true;
			break;
        case (' '):
		    Simulation::paused = !Simulation::paused;
			break;
		default:
			break;
		}

}

void Resize()
{
        glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		minGraphHeight = - (double) HH_NeuralNetwork::ROWS[1] /2 * (HH_NeuralNetwork::HEIGHT + HH_NeuralNetwork::Y_GAP);
		maxGraphHeight = (double) HH_NeuralNetwork::ROWS[1] /2 * (HH_NeuralNetwork::HEIGHT + HH_NeuralNetwork::Y_GAP);


		//glOrtho(-1000, 1000, minGraphHeight, maxGraphHeight, 1.0, 1000.0);
		//glOrtho(-1000, 11500, -1500, 8500, 1.0, 1000.0);
		//glOrtho(-10000, 11500, -15000, 8500, 1.0, 1000.0);

		glOrtho(-1500, 15500, -3000, 4500, 1.0, 1000.0);

		//glOrtho(-2500, 20000, -1500, 10000, 1.0, 1000.0);
		//glOrtho(-700, 0, -1000, 1000, 1.0, 1000.0);

		//glOrtho(-10000, 115000, -15000, 85000, 1.0, 1000.0);

		//glOrtho(-2500, 7500, -1500, 1500, 1.0, 1000.0);

		glMatrixMode(GL_MODELVIEW);

		gluLookAt(0.0, 0.0, 0.0,
			0.0, 0.0, 0.0,
			0.0, 1.0, 0.);
}


/*
void Resize2()
{
    glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		//gluPerspective(45, (GLdouble)glutGet(GLUT_WINDOW_HEIGHT) / glutGet(GLUT_WINDOW_WIDTH) * 1.3, 1.0, 12000.0);
		//glOrtho(0, 100000, minGraphHeight, maxGraphHeight, 1.0, 1000.0);

		//glOrtho(0, 100000, minGraphHeight, maxGraphHeight, 1.0, 1000.0);
		//glOrtho(minGraphX-100, maxGraphX+100, minGraphHeight, maxGraphHeight, 1.0, 1000.0);


		minGraphHeight = neuralNetwork2->pos.y - Graph::HEIGHT;
		maxGraphHeight = neuralNetwork->pos.y + HH_NeuralNetwork::ROWS * (Graph::HEIGHT + Graph::Y_GAP) + Graph::HEIGHT + 1000;
		////glOrtho(neuralNetwork->pos.x-20, neuralNetwork->pos.x + (neuralNetwork->neurons->columns + 1) * (Graph::WIDTH + Graph::GAP) + 20, minGraphHeight, maxGraphHeight, 1.0, 1000.0);
		//glOrtho(neuralNetwork->pos.x-20, neuralNetwork->pos.x + (neuralNetwork->neurons->columns + 1) * (Graph::WIDTH + Graph::X_GAP) + 20, minGraphHeight, maxGraphHeight, 1.0, 1000.0);
		glOrtho(neuralNetwork->pos.x-20, neuralNetwork->pos.x + (neuralNetwork->neurons->columns + 1) * (Graph::WIDTH + Graph::X_GAP) + 4 * Graph::X_GAP, minGraphHeight, maxGraphHeight, 1.0, 1000.0);



		//glOrtho(neuralNetwork->pos.x-20, neuralNetwork->pos.x + (neuralNetwork->neurons->columns + 1) * (Graph::WIDTH + Graph::GAP) + 20, -(HH_NeuralNetwork::ROWS/2 * (Graph::HEIGHT + Graph::GAP)) - 8 * Graph::GAP, -(HH_NeuralNetwork::ROWS/2 * (Graph::HEIGHT + Graph::GAP)) + 2 * (Graph::HEIGHT + Graph::GAP) - 2 * Graph::GAP, 1.0, 1000.0);

		glMatrixMode(GL_MODELVIEW);

		gluLookAt(0.0, 0.0, 0.0,
			0.0, 0.0, 0.0,
			0.0, 1.0, 0.);
}
*/

void InitGL()
{
    glutInitDisplayMode(GLUT_DEPTH |
		GLUT_DOUBLE |
		GLUT_RGBA |
		GLUT_MULTISAMPLE);

    //Now create the graphics window
    glutInitWindowSize(glutGet(GLUT_SCREEN_WIDTH), glutGet(GLUT_SCREEN_HEIGHT));
    glutCreateWindow("Electromagnetic wave temperature increase effects on Hodgkin-Huxley neural networks");

	GLenum err = glewInit();

	glutMouseFunc(MouseButtons);
	glutMotionFunc(MouseMotion);
	glutPassiveMotionFunc(PassiveMouseMotion);
	glutKeyboardFunc(ProcessKey);
	glutDisplayFunc(display);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(0.0, 0.0, 0.0, 0.0);
	////glClearColor(1.0, 1.0, 1.0, 1.0);


	glPointSize(5);

	glShadeModel(GL_SMOOTH);

	//Resize();
}

/*////void CalculateErrorDurationPercentageAndAddPointToGraph()
{
    for (uint8_t i = 0; i < HH_NeuralNetwork::LAYERS; i++)
    {
        rfErrorDuration = neuralNetworks->neuralNetworks[1]->neurons->errorsToRefNeuronLayer[i].totalErrorDuration;
        rfSignallingDuration = neuralNetworks->neuralNetworks[1]->neurons->errorsToRefNeuronLayer[i].totalSignallingDuration;

        if (neuralNetworks->neuralNetworks[1]->neurons->errorsToRefNeuronLayer[i].currentErrorDurationStartTime > 0)
            ////rfErrorDuration += ((Simulation::currentSimTime - TrainingSet::InitializationInterval) - neuralNetworks->neuralNetworks[1]->neurons->errorsToRefNeuronLayer[i].currentErrorDurationStartTime);
            rfErrorDuration += (Simulation::currentSimTime - neuralNetworks->neuralNetworks[1]->neurons->errorsToRefNeuronLayer[i].currentErrorDurationStartTime);

        if (neuralNetworks->neuralNetworks[1]->neurons->errorsToRefNeuronLayer[i].currentSignallingDurationStartTime> 0)
            ////rfSignallingDuration += ((Simulation::currentSimTime - TrainingSet::InitializationInterval) - neuralNetworks->neuralNetworks[1]->neurons->errorsToRefNeuronLayer[i].currentSignallingDurationStartTime);
            rfSignallingDuration += (Simulation::currentSimTime - neuralNetworks->neuralNetworks[1]->neurons->errorsToRefNeuronLayer[i].currentSignallingDurationStartTime);

        ////rfErrorDuration = rfErrorDuration/(Simulation::currentSimTime - TrainingSet::InitializationInterval) * 100;
        rfErrorDuration = rfErrorDuration/rfSignallingDuration * 100;

        ////fitnessGraph->AddPoint(rfErrorDuration, 30, i);
        ////rfErrorDuration = (double) rand() / RAND_MAX * 100;
        ////original fitnessGraph->AddPoint(rfErrorDuration, currentSimTemperatureIndex, i);

        if (neuralNetworks->neuralNetworks[1]->neurons->errorsToRefNeuronLayer[i].totalPatternErrorCount == 0)
            rfAvgNeuronErrorCount = 0;
        else
            rfAvgNeuronErrorCount = neuralNetworks->neuralNetworks[1]->neurons->errorsToRefNeuronLayer[i].totalNeuronsInErrorCount / neuralNetworks->neuralNetworks[1]->neurons->errorsToRefNeuronLayer[i].totalPatternErrorCount;

        ////fitnessGraph->AddPoint(rfAvgNeuronErrorCount / HH_NeuralNetwork::ROWS[HH_NeuralNetwork::LAYERS-1] * 100, 0, i);
    }

    double result, x;

    /*fitnessGraph->currentIndex++;

    for (double membraneVoltage = -1000; membraneVoltage < 1000; membraneVoltage++)
    {
    if (membraneVoltage < -HH_Neuron::MINMAX_VOLTAGE)
        {
            x = membraneVoltage + HH_Neuron::MINMAX_VOLTAGE;

            x *= -1;

            result = 1 / (1 + exp(-x*0.0233));

            result = (result - 0.5) * 2;

            result = -HH_Neuron::MINMAX_VOLTAGE - result * 75;

            ////membraneVoltage = result;

            //std::cout << result << "\n";
        }
        else
        if (membraneVoltage > HH_Neuron::MINMAX_VOLTAGE)
        {
            x = membraneVoltage - HH_Neuron::MINMAX_VOLTAGE;

            result = 1 / (1 + exp(-x*0.0233));

            result = (result - 0.5) * 2;

            result = HH_Neuron::MINMAX_VOLTAGE + result * 75;

            ////membraneVoltage = result;

            //std::cout << result << "\n";
        }
        else
            result = membraneVoltage;

        fitnessGraph->AddPoint(result, 0, membraneVoltage+1000);
    }
    */


    /*uint16_t range = 4;

    uint32_t index = 0;

    for (double i = -range; i < range; i+=0.1)
    {
        result = 1 / (1 + exp(-i*1.3));

        result = result * 2 - 1;

        result *= 10;

        fitnessGraph->AddPoint(result, 1, index++);
    }
    */


    /*for (int16_t i = -range; i < range; i++)
    {
        result = 1 / (1 + exp(-i*1.3));

        result *= 10;

        fitnessGraph->AddPoint(result, 1, i+range);



        /*
        result = 1 / (1 + exp(-i*0.06));

        //result *= 100;

        fitnessGraph->AddPoint(result, 2, i+range);



        result = 1 / (1 + exp(-i*0.09));

        //result *= 100;

        fitnessGraph->AddPoint(result, 3, i+range);
        */
    ////}
////}

/*void ActivateNeuronsFromFoodAngle(FoodForAgents* foodForAgents)
{
    agents->ActivateNeuronsFromFoodAngle(foodForAgents);
}*/

/*void ProcessIfEating()
{
    agents->ProcessIfEating(foodForAgents);
}*/

/*////void GetQAAndSetQToNetworks()
{
    neuralNetworks->DetermineErrorsForPreviousQA(0);

    TrainingQA* currentQA = trainingSet->GetNextQA();

    for(uint32_t i = 0; i < neuralNetworks->neuralNetworksCount; i++)
    {
        neuralNetworks->SetQA(currentQA, i);
    }
}*/

void SetNNsSamePosAndSetRedVoltages()
{
            for(uint32_t i = 1; i < neuralNetworks->neuralNetworksCount; i++)
            {
                neuralNetworks->neuralNetworks[i]->pos=neuralNetworks->neuralNetworks[0]->pos;

                neuralNetworks->neuralNetworks[i]->neurons->graphColor->Set(1,0,0);
            }
}

void GenerateNextGeneration()
{
    /*totalFitness = agents->GetTotalFitness();
    avgFitness = agents->GetAvgFitness();

    ////fitnessGraph->AddPoint((double) totalFitness, 0);
    fitnessGraph->AddPoint(avgFitness*10, 0);
    */

    uint32_t* fittestNNIndexes = neuralNetworks->GetFittestNNIndexes();
    {
        ////if (false)
        if (neuralNetworks->neuralNetworks[fittestNNIndexes[0]]->agent->fitness>=0)//// && neuralNetworks->neuralNetworks[0]->layers <= 30)
        {
            fittestNNIndexes[1] = fittestNNIndexes[0];
            neuralNetworks->CopyWeights(fittestNNIndexes); //generate from fittest two

            neuralNetworks->SetResultsBuffers(neuralNetworks->neuralNetworks[fittestNNIndexes[0]]->neurons->resultsBuffers);

            sprintf(textBuffer, "fittestNN%i.txt", Simulation::simStartRealTime);
            neuralNetworks->neuralNetworks[fittestNNIndexes[0]]->Save(textBuffer);

            drawingGraphics = false;
            Simulation::nnVisible = false;
            Sleep(1000);
            ////original agents->AddNewNNHiddenLayerToAgents();
            ////Simulation::nnVisible = true;
            drawingGraphics = true;

            HH_NeuralNetworks::electromagneticRF_HH = true;////original
            Simulation::trainingOrEvolution = false;////original

            if (Simulation::testingLayerErrors)
                Simulation::generationDuration = Simulation::generationDurationLayerTesting;
            else
                Simulation::generationDuration = Simulation::generationDurationFoodTesting;

            neuralNetworks->ResetSpikeCounts();

            SetNNsSamePosAndSetRedVoltages();
        }
        else
        {
            sprintf(textBuffer, "NN%i.txt", Simulation::simStartRealTime);
            neuralNetworks->neuralNetworks[fittestNNIndexes[0]]->Save(textBuffer);
            ////neuralNetworks->neuralNetworks[fittestNNIndexes[0]]->Save("NN.txt");

            neuralNetworks->CopyWeights(fittestNNIndexes); //generate from fittest two

            ////neuralNetworks->Mutate(neuralNetworks->neuralNetworks[0]->layers-1, neuralNetworks->neuralNetworks[0]->layers-1);
            /*original if (neuralNetworks->neuralNetworks[0]->layers>2)
                ////neuralNetworks->Mutate(neuralNetworks->neuralNetworks[0]->layers-2, neuralNetworks->neuralNetworks[0]->layers-2);
                neuralNetworks->Mutate(neuralNetworks->neuralNetworks[0]->layers-2, neuralNetworks->neuralNetworks[0]->layers-1);
            else
                neuralNetworks->Mutate();
                */

            neuralNetworks->Mutate();
        }

        agents->ReGenerate();
        Environment::foodForAgents_ptr->ReGenerate();
    }
}



void AgentThread(void *param)
{

}

void DetermineAndGraphElectromagneticSpikeCountErrors(FitnessGraph* graph,FitnessGraph* graph2, Agent* agent1, Agent* agent2)
{
    int32_t electromagneticErrorsForLayer[agents->count][HH_NeuralNetwork::LAYERS];
    uint32_t totalSpikeCountForLayerAgent1;
    uint32_t totalSpikeCountForLayerAgent2;

    double error;
    uint32_t layerErrorCount;

    electromagneticErrorForLayerAdditionCount++;

    for (uint32_t agentIndex=1; agentIndex<agents->count; agentIndex++)
    {
        char textBuffer[255];
        sprintf(textBuffer, "Temperature %f, results %i", agents->agents[agentIndex]->GetTemperature(), electromagneticErrorForLayerAdditionCount);

        graph->SetSeriesTitle(textBuffer, agentIndex);

    for (uint32_t j=0; j<HH_NeuralNetwork::LAYERS;j++)
    {
        electromagneticErrorsForLayer[agentIndex][j] = 0;

        ////totalSpikeCountForLayerAgent1 = 0;
        ////totalSpikeCountForLayerAgent2 = 0;

        layerErrorCount = 0;
        for (uint32_t i=0; i<((HH_NeuralNetwork*) agent1->neuralNetwork)->neurons->rowCountsForColumns[j];i++)
        {
            ////for (uint32_t agentIndex=1; agentIndex<agents->count; agentIndex++)
            {
                error = abs((int32_t) agents->agents[0]->spikeCountsArrays[j][i] - (int32_t) agents->agents[agentIndex]->spikeCountsArrays[j][i]);
                ////electromagneticError += ((HH_NeuralNetwork*) agent1->neuralNetwork)->neurons->neurons[j][i].spikeCount - ((HH_NeuralNetwork*) agent2->neuralNetwork)->neurons->neurons[j][i].spikeCount;
                electromagneticErrorsForLayer[agentIndex][j] += error;

                ////totalSpikeCountForLayerAgent1 += agent1->spikeCountsArrays[j][i];
                ////totalSpikeCountForLayerAgent2 += agent2->spikeCountsArrays[j][i];

                if (error != 0)
                    layerErrorCount++;
            }
        }


        electromagneticErrorsForLayerTotal[agentIndex][j] += electromagneticErrorsForLayer[agentIndex][j];

        electromagneticErrorPercentageForLayerTotal[agentIndex][j] += (double) layerErrorCount/((HH_NeuralNetwork*) agent1->neuralNetwork)->neurons->rowCountsForColumns[j] * 100;

        ////graph->AddPoint(totalSpikeCountForLayerAgent1, 1, j);
        ////graph->AddPoint(totalSpikeCountForLayerAgent2, 0, j);

        ////graph->AddPoint(electromagneticErrorForLayerTotal[agentIndex][j], agentIndex + 1, j);
        graph->AddPoint(electromagneticErrorsForLayerTotal[agentIndex][j]/electromagneticErrorForLayerAdditionCount, agentIndex, j);
        graph2->AddPoint(electromagneticErrorPercentageForLayerTotal[agentIndex][j]/electromagneticErrorForLayerAdditionCount, agentIndex, j);
        }
    }
}

void SimThreadFunction(void *param)
{
    uint32_t setCount = 0;

    HH_NeuralNetworks::simHH = true;

    DWORD dwWaitResultStimInterval;

    dwWaitResultStimInterval = 0;

    DWORD dwWaitResult;

    dwWaitResult = WAIT_OBJECT_0;

    SetNNsSamePosAndSetRedVoltages();

    if (Simulation::trainingOrEvolution == false)
    {
        HH_NeuralNetworks::electromagneticRF_HH = true;////original

        if (Simulation::testingLayerErrors)
            Simulation::generationDuration = Simulation::generationDurationLayerTesting;
        else
            Simulation::generationDuration = Simulation::generationDurationFoodTesting;
    }

    while (processing)
    {
        ////DetermineAndGraphElectromagneticSpikeCountErrors(errorGraph, agents->agents[0], agents->agents[1]);

        if (incSpeed)
        {
            Simulation::lag-=1;

            incSpeed = false;

            ////HH_NeuralNetwork::Layer1ActiveSynapticConductanceHH += 0.0001;
        }

        if (decSpeed)
        {
            Simulation::lag+=1;

            decSpeed = false;

            ////HH_NeuralNetwork::Layer1ActiveSynapticConductanceHH -= 0.0001;
        }

        if (incStimulus)
        {
            rfStimulusTemperatureInc+=0.5;

            incStimulus = false;
        }
        else
            if (decStimulus)
            {
                rfStimulusTemperatureInc-=0.5;

                decStimulus = false;
            }

        if (incNoise)
        {
            HH_NeuralNetworks::MAX_NEURAL_NOISE += HH_NeuralNetworks::NEURAL_NOISE_DELTA;

            incNoise = false;
        }
        else
            if (decNoise)
            {
                HH_NeuralNetworks::MAX_NEURAL_NOISE -= HH_NeuralNetworks::NEURAL_NOISE_DELTA;

                if (HH_NeuralNetworks::MAX_NEURAL_NOISE < 0)
                    HH_NeuralNetworks::MAX_NEURAL_NOISE = 0;

                decNoise = false;
            }

        if (Simulation::lag < 0)
            Simulation::lag = 0;

        if (!Simulation::paused)
        {
            uint32_t neuronsCount = neuralNetworks->neuralNetworks[0]->neurons->count;

            for (uint32_t i = 0; i < neuronsCount; i++)
            {
                HH_NeuralNetworks::NEURAL_NOISE[i] = (double) rand()/RAND_MAX * HH_NeuralNetworks::MAX_NEURAL_NOISE - HH_NeuralNetworks::MAX_NEURAL_NOISE/2;
            }

            ////std::cout << "Main SimThread: ReleaseSemaphore(Simulation::startNextGenerationSemaphore, agents->count, NULL);\n\n";
            ////ReleaseSemaphore(Simulation::startNextGenerationSemaphore, agents->count, NULL);

            Simulation::currentSimTime = agents->agents[0]->generationTime;

            /*////double startTemp = rfStimulusTemperatureStart;
            for (int i=1;i<neuralNetworks->neuralNetworksCount;i++)
            {
                neuralNetworks->SetTemperature(startTemp, i);

                ////startTemp += 0.5;
                ////startTemp += 3;

                ////startTemp += 10;
                ////startTemp += 0.1;
                ////startTemp += 0.000001;
                ////startTemp = referenceTemperature + rfStimulusTemperatureInc * pow(10, i);
                ////startTemp = referenceTemperature + rfStimulusTemperatureInc * (i + 1);
                startTemp += rfStimulusTemperatureInc;
            }*/

            ////if (true)
            if (!Simulation::trainingOrEvolution)
            {
            if (Simulation::currentSimTime > startStimulusTime + Simulation::stimulusDuration + Simulation::stimulusInterval)
            {
                startStimulusTime = Simulation::currentSimTime;
            }

            if (Simulation::currentSimTime - startStimulusTime < Simulation::stimulusDuration)
            {
            //stimulus = stimulusMagnitude;



            ////double startTemp = referenceTemperature + rfStimulusTemperatureInc;
            double startTemp = rfStimulusTemperatureStart;
            uint32_t j = 2;
            for (int i=1;i<neuralNetworks->neuralNetworksCount;i++)
            {
                neuralNetworks->SetTemperature(startTemp, i);

                ////startTemp += 0.5;
                ////startTemp += 3;

                ////startTemp += 10;
                ////startTemp += 0.1;
                ////startTemp += 0.000001;
                startTemp = referenceTemperature + rfStimulusTemperatureInc * pow(10, 2);
                ////startTemp = referenceTemperature + rfStimulusTemperatureInc * (i + 1);
                ////startTemp += rfStimulusTemperatureInc;
                j+=2;
            }



            /*////
            double startTemp = referenceTemperature + rfStimulusTemperatureInc;
            for (int i=1;i<neuralNetworks->neuralNetworksCount;i++)
            {
                neuralNetworks->SetTemperature(startTemp, i);

                startTemp = referenceTemperature + rfStimulusTemperatureInc * pow(10, i);
            }
            */



/*
            neuralNetwork->SetTemperature(37);
            //neuralNetwork2->SetTemperature(37.25);
            //neuralNetwork2->SetTemperature(37.3);
            neuralNetwork2->SetTemperature(37.3);

            /*
            layer1Stimulus[0] = 1;
            layer1Stimulus[1] = 0;
            layer1Stimulus[2] = 1;
            layer1Stimulus[3] = 0;
            */

/*
            neuralNetwork->SetLayer1Stimulus(layer1Stimulus);

            //neuralNetwork->SetReceivingStimulus(true);
            //neuralNetwork2->SetReceivingStimulus(true);
            */
        }
        else
        {
            //stimulus = 0;

            /*////layer1Stimulus[0] = 0;
/*            layer1Stimulus[1] = 0;
            layer1Stimulus[2] = 0;
            layer1Stimulus[3] = 0;

            neuralNetwork->SetLayer1Stimulus(layer1Stimulus);
            */

            neuralNetworks->SetTemperature(referenceTemperature);

            /*
            for(uint32_t i = 0; i < HH_NeuralNetwork::ROWS; i++)
            {
                layer1Stimulus[i] = (uint8_t) ((double) rand()/ RAND_MAX * 1 + 0.5);
            }
            */
        }
            }

            if (Simulation::threading)
            {
                if (dwWaitResultStimInterval == WAIT_OBJECT_0)
                    agents->ReleaseStimIntervalSemaphores();

                ////std::cout << "Main SimThread: WaitForMultipleObjects(agents->count, agents->agentProcessedGenerationSemaphores, true, 100000);\n\n";
                ////dwWaitResultStimInterval = WaitForMultipleObjects(agents->count, agents->agentProcessedStimIntervalSemaphores, true, 10);
                ////dwWaitResultStimInterval = WaitForMultipleObjects(agents->count, agents->agentProcessedStimIntervalSemaphores, true, 10000);
                dwWaitResultStimInterval = WaitForMultipleObjects(agents->count, agents->agentProcessedStimIntervalSemaphores, true, 10);

                switch (dwWaitResultStimInterval)
                {
                    case WAIT_OBJECT_0:
                        Simulation::currentSimTime = agents->agents[0]->generationTime;
                    break;

                    case WAIT_TIMEOUT:
                    break;

                    default:
                    break;
                }

                if (dwWaitResult == WAIT_OBJECT_0)
                    agents->ReleaseSemaphores();

                ////std::cout << "Main SimThread: WaitForMultipleObjects(agents->count, agents->agentProcessedGenerationSemaphores, true, 100000);\n\n";
                ////dwWaitResult = WaitForMultipleObjects(agents->count, agents->agentProcessedGenerationSemaphores, true, 10);
                ////dwWaitResult = WaitForMultipleObjects(agents->count, agents->agentProcessedGenerationSemaphores, true, 10000);
                dwWaitResult = WaitForMultipleObjects(agents->count, agents->agentProcessedGenerationSemaphores, true, 10);

                int grc;
                switch (dwWaitResult)
                {
                    case WAIT_OBJECT_0:
                        grc = 1;
                    break;

                    case WAIT_TIMEOUT:
                    break;

                    default:
                    break;
                }
            }
            else
            {
                dwWaitResultStimInterval = WAIT_OBJECT_0;
                dwWaitResult = WAIT_OBJECT_0;
            }

            ////Simulation::currentSimTime += Simulation::deltaTime;

            if (dwWaitResultStimInterval == WAIT_OBJECT_0 && Simulation::trainingOrEvolution == false && Simulation::threading)
            {
                DetermineAndGraphElectromagneticSpikeCountErrors(errorGraph, errorLayerPercentageGraph, agents->agents[0], agents->agents[1]);
            }

            //if (false)
            if (dwWaitResult == WAIT_OBJECT_0)
            if (Simulation::threading || (Simulation::currentSimTime >= startGenerationTime + Simulation::generationDuration))
            {
                ////char errorGraphFileName[255];
                ////char errorLayerPercentageGraphFileName[255];

                ////sprintf(errorGraphFileName, "errorGraph%i.txt", GetTickCount());
                errorGraph->Save(errorGraphFileName);

                ////sprintf(graphFileName, "errorLayerPercentageGraph%i.txt", GetTickCount());
                errorLayerPercentageGraph->Save(errorLayerPercentageGraphFileName);
                ////totalFitness = agents->GetTotalFitness();
                ////avgFitness = agents->GetAvgFitness();

                ////fitnessGraph->AddPoint((double) totalFitness, 0);
                ////fitnessGraph->AddPoint(avgFitness, 0);


                for (uint32_t i=0; i<agents->count;i++)
                {
                    fitnessGraph->AddPoint(agents->agents[i]->fitness, i);
                }


                startGenerationTime = Simulation::currentSimTime;

                if (Simulation::trainingOrEvolution)
                {
                    GenerateNextGeneration();

                    agents->ResetTraces();
                    clearScreen = true;
                }
                else
                {
                    agents->ReGenerate();
                    Environment::foodForAgents_ptr->ReGenerate();

                    agents->ResetTraces();
                    clearScreen = true;
                }

                ////startStimulusTime = Simulation::currentSimTime;
                Simulation::currentSimTime = 0;
                startStimulusTime = 0;

                Simulation::generations++;
            }

            /*////
            if (rfStimulus)
                agents->SetTemperature(rfStimulusTemperature);
            else
                agents->SetTemperature(referenceTemperature);

            Environment::foodForAgents_ptr->SetFoodColor(Food::FoodDefaultColor); //set to green, closest food to agent in ActivateNeuronsFromFoodAngle() is set to gold
            */

            if (!Simulation::threading)
            {
                /*////if (Simulation::trainingOrEvolution)
                    agents->Process();
                else
                    agents->agents[0]->Process();
                */

                agents->Process();
            }


            ////WaitForMultipleObjects(agents->count, agents->agentProcessedGenerationSemaphores, true, 1000);
            ////Simulation::currentSimTime += Simulation::deltaTime;

            ////Sleep(Simulation::lag);
        }
    }
}

/*void SimThreadFunction3(void *param)
 {
     double startStimulusTime, endStimulusTime, Simulation::currentSimTime;

     Simulation::currentSimTime = 0;

     startStimulusTime  = - stimulusDuration - stimulusInterval;

     TrainingQA* currentQA = trainingSet->GetNextQA();
     TrainingQA intervalQA(HH_NeuralNetwork::ROWS);

     while (processing)
     {
         Simulation::currentSimTime += Simulation::deltaTime;

        if (Simulation::currentSimTime >= startStimulusTime + stimulusDuration + stimulusInterval)
        {
            startStimulusTime = Simulation::currentSimTime;

            //if (trainingSet->currentQAIndex == trainingSet->size)
            if (trainingSet->currentQAIndex == trainingSet->currentSubSetSize)
            {
                uint32_t fittestNNIndex = neuralNetworks->GetFittestNNIndex();

                neuralNetworks->CopyWeights(fittestNNIndex);

                if (neuralNetworks->neuralNetworks[fittestNNIndex]->neurons->totalError > 0)
                //if (neuralNetworks->GetAverageTotalError() > 0)
                {
                    neuralNetworks->Mutate();
                }
                else
                    if (trainingSet->currentSubSetSize < trainingSet->maxSubsetSize)
                        trainingSet->currentSubSetSize++;

                if (neuralNetworks->neuralNetworks[fittestNNIndex]->neurons->totalError <= 0 && trainingSet->currentSubSetSize >= trainingSet->maxSubsetSize)
                {
                    stimulusDuration = 1;
                    stimulusInterval = 1;

                    neuralNetworks->visible = true;
                }
                else
                {
                    stimulusDuration = 0.0000000001;
                    stimulusInterval = 0.0000000001;
                }

                neuralNetworks->ResetTotalError();
            }

            currentQA = trainingSet->GetNextQA();

            for(uint32_t i = 0; i < neuralNetworks->neuralNetworksCount; i++)
            {
                neuralNetworks->SetTemperature(37, i);
                //neuralNetworks->SetQA(currentQA, i);
                //neuralNetworks->SetLayer1Stimulus(layer1Stimulus, i);
            }
        }

         /*if (Simulation::currentSimTime - startStimulusTime < stimulusDuration)
         {
            for(uint32_t i = 0; i < neuralNetworks->neuralNetworksCount; i++)
            {
                neuralNetworks->SetTemperature(37, i);
                neuralNetworks->SetQA(currentQA, i);
                //neuralNetworks->SetLayer1Stimulus(layer1Stimulus, i);
            }
         }
        else
            */
/*        if (Simulation::currentSimTime - startStimulusTime > stimulusDuration)
        {
            layer1Stimulus[0] = 0;
            layer1Stimulus[1] = 0;
            layer1Stimulus[2] = 0;
            layer1Stimulus[3] = 0;

            for(uint32_t i = 0; i < neuralNetworks->neuralNetworksCount; i++)
            {
                neuralNetworks->SetTemperature(37, i);
                //neuralNetworks->SetQA(&intervalQA, i);
                //neuralNetworks->SetLayer1Stimulus(intervalQA.q, i);
            }

            /*for(uint32_t i = 0; i < HH_NeuralNetwork::ROWS; i++)
            {
                layer1Stimulus[i] = (uint8_t) ((double) rand()/ RAND_MAX * 1 + 0.5);
            }*/

/*            currentQA = &intervalQA;
        }

        for(uint32_t i = 0; i < neuralNetworks->neuralNetworksCount; i++)
            {
                //neuralNetworks->SetTemperature(37, i);
                neuralNetworks->SetQA(currentQA, i);
                //neuralNetworks->SetLayer1Stimulus(currentQA->q, i);
            }

         neuralNetworks->Process(Simulation::deltaTime);

         Sleep(1);
     }
 }
 */


 /*
 void SimThreadFunction2(void *param)
 {
     double startStimulusTime, endStimulusTime, Simulation::currentSimTime;

     Simulation::currentSimTime = startStimulusTime = 0;

     while (processing)
     {
         Simulation::currentSimTime += Simulation::deltaTime;

        if (Simulation::currentSimTime > startStimulusTime + stimulusDuration + stimulusInterval)
        {
            //startStimulusTime = GetTickCount();
            startStimulusTime = Simulation::currentSimTime;

            //stimulusMagnitude += 0.001;
        }

        //currentTime = GetTickCount();
        //Simulation::currentSimTime += Simulation::deltaTime;

         if (Simulation::currentSimTime - startStimulusTime < stimulusDuration)
         {
            //stimulus = stimulusMagnitude;


            /*double startTemp = 39;
            for (int i=10;i<20;i++)
            {
                //neuralNetwork->SetTemperature(47, 11);
                neuralNetwork->SetTemperature(startTemp, i);
                neuralNetwork->SetTemperature(startTemp, i + 10);

                startTemp += 2;
            }
            */
/*
            neuralNetwork->SetTemperature(37);
            //neuralNetwork2->SetTemperature(37.25);
            //neuralNetwork2->SetTemperature(37.3);
            neuralNetwork2->SetTemperature(37.3);

            /*
            layer1Stimulus[0] = 1;
            layer1Stimulus[1] = 0;
            layer1Stimulus[2] = 1;
            layer1Stimulus[3] = 0;
            */

/*
            neuralNetwork->SetLayer1Stimulus(layer1Stimulus);

            //neuralNetwork->SetReceivingStimulus(true);
            //neuralNetwork2->SetReceivingStimulus(true);
         }
        else
        {
            //stimulus = 0;

            layer1Stimulus[0] = 0;
            layer1Stimulus[1] = 0;
            layer1Stimulus[2] = 0;
            layer1Stimulus[3] = 0;

            neuralNetwork->SetLayer1Stimulus(layer1Stimulus);

            neuralNetwork->SetTemperature(37);
            neuralNetwork2->SetTemperature(37);

            /*
            for(uint32_t i = 0; i < HH_NeuralNetwork::ROWS; i++)
            {
                layer1Stimulus[i] = (uint8_t) ((double) rand()/ RAND_MAX * 1 + 0.5);
            }
            */

/*
        }
*/
        //neuralNetwork->SetTemperature(47);
        //neuralNetwork2->SetTemperature(37);

            //stimulus = 0.007;

         //stimulusMagnitude += 0.000003;

         //stimulus = stimulusMagnitude;

         //neuralNetwork->Process(stimulus, Simulation::deltaTime, 100, 70000);
         //neuralNetwork->SetSynStimulus(stimulus);
/*
         neuralNetwork->Process(Simulation::deltaTime);
         neuralNetwork2->Process(Simulation::deltaTime);

         Sleep(1);
     }
 }
 */

int main(int argc, char *argv[])
{
    /*if(argc < 2)
    {
        std::cerr << "requires number of benchmarking loops as command line argument #1";
        return 1;
    }*/


/*////
double a = 0.1;
    unsigned int control_word;
    int err;

    // Show original FP control word and do calculation.
    err = _controlfp_s(&control_word, 0, 0);
    if ( err ) ;

    printf( "Original: 0x%.4x\n", control_word );
    printf( "%1.1f * %1.1f = %.15e\n", a, a, a * a );

    // Set precision to 24 bits and recalculate.
    err = _controlfp_s(&control_word, _PC_64, MCW_PC);
    if ( err ) ;

    printf( "64-bit:   0x%.4x\n", control_word );
    printf( "%1.1f * %1.1f = %.15e\n", a, a, a * a );
    */

    Simulation::simStartRealTime = GetTickCount();

    time_t t;
    srand((unsigned) time(&t));

    Vector pos;

    agents = new Agents();
    Environment::foodForAgents_ptr = new FoodForAgents();

    neuralNetworks = new HH_NeuralNetworks(agents);
    ////agents->Add();

    ////trainingSet = new TrainingSet(256);


    /*////double inc = 0.001;

    simTemperatures[0] = 37.001;
    for (uint8_t i=1; i<simTemperaturesLength; i++)
        simTemperatures[i] = simTemperatures[i-1] + inc;
        */


    double inc = 1;

    simTemperatures[0] = 38;
    for (uint8_t i=1; i<simTemperaturesLength; i++)
        simTemperatures[i] = simTemperatures[i-1] + inc;


    /*////double inc = 0.1;

    simTemperatures[0] = 37.1;
    for (uint8_t i=1; i<simTemperaturesLength; i++)
        simTemperatures[i] = simTemperatures[i-1] + inc;
        */

    HH_NeuralNetworks::electromagneticRF_HH = false;

    ////HH_NeuralNetworks::electromagneticRF_HH = true;
    ////Simulation::trainingOrEvolution = false;

    /*////if (!Simulation::trainingOrEvolution)
    {
        ////loadedNeuralNetwork = HH_NeuralNetwork::Load("neuralnetworks/nn1col_1.txt");
        ////loadedNeuralNetwork = HH_NeuralNetwork::Load("neuralnetworks/nn2col_2.txt");
        ////loadedNeuralNetwork = HH_NeuralNetwork::Load("neuralnetworks/nn4cols_8_3_8_3.txt");
        ////loadedNeuralNetwork = HH_NeuralNetwork::Load("neuralnetworks/nn2cols_3_8.txt");

        ////loadedNeuralNetwork = HH_NeuralNetwork::Load("neuralnetworks/nn2cols_3_8_+1.txt");

        /*////original loadedNeuralNetwork = HH_NeuralNetwork::Load("neuralnetworks/nn2cols_8_3.txt");

        /*////for(uint8_t i = 0; i<3; i++)
        {
            loadedNeuralNetwork2 = HH_NeuralNetwork::Load("neuralnetworks/nn2cols_3_8_+1.txt");
            loadedNeuralNetwork->AddLayersFromNN(loadedNeuralNetwork2);

            loadedNeuralNetwork2 = HH_NeuralNetwork::Load("neuralnetworks/nn2cols_8_3.txt");
            loadedNeuralNetwork->AddLayersFromNN(loadedNeuralNetwork2);
        }

        loadedNeuralNetwork2 = HH_NeuralNetwork::Load("neuralnetworks/nn2cols_3_8_+1.txt");
        loadedNeuralNetwork->AddLayersFromNN(loadedNeuralNetwork2);

        ////loadedNeuralNetwork->RecurrentJoin();
        */


        ////loadedNeuralNetwork = new HH_NeuralNetwork(HH_NeuralNetwork::LAYERS, HH_NeuralNetwork::ROWS, &pos);
        ////loadedNeuralNetwork->JoinLayer(1, HH_NeuralNetwork::LAYERS-1);

        ////loadedNeuralNetwork = HH_NeuralNetwork::Load("fittestNN23_9.txt");
        ////loadedNeuralNetwork = HH_NeuralNetwork::Load("neuralnetworks/fittestNN_2_9.txt");
        ////loadedNeuralNetwork = HH_NeuralNetwork::Load("neuralnetworks/fittestNN30_9.txt");
        ////loadedNeuralNetwork = HH_NeuralNetwork::Load("fittestNN.txt");
        ////loadedNeuralNetwork = HH_NeuralNetwork::Load("neuralnetworks/fittestNN6_9.txt");
        ////loadedNeuralNetwork = HH_NeuralNetwork::Load("neuralnetworks/fittestNN10_9_30.txt");

/*////        loadedNeuralNetwork = HH_NeuralNetwork::Load("neuralnetworks/fittestNN551888375.txt");


        HH_NeuralNetwork::LAYERS = loadedNeuralNetwork->layers;

        loadedNeuralNetwork->SetTemperature(referenceTemperature);
        neuralNetworks->Add(loadedNeuralNetwork);

        loadedNeuralNetwork->agent = new Agent();
        loadedNeuralNetwork->agent->SetNN(loadedNeuralNetwork);
        agents->Add(loadedNeuralNetwork->agent);


        loadedNeuralNetwork2 = new HH_NeuralNetwork(loadedNeuralNetwork->layers, loadedNeuralNetwork->rowCountsForColumns, &pos);
        loadedNeuralNetwork2->agent = new Agent();
        loadedNeuralNetwork2->agent->SetNN(loadedNeuralNetwork2);
        agents->Add(loadedNeuralNetwork2->agent);

        loadedNeuralNetwork2->Copy(loadedNeuralNetwork);
        loadedNeuralNetwork2->neurons->CopyResultsBuffers(loadedNeuralNetwork->neurons->resultsBuffers);

        loadedNeuralNetwork2->SetTemperature(referenceTemperature);
        loadedNeuralNetwork2->neurons->graphColor->Set(1,0,0);
        neuralNetworks->Add(loadedNeuralNetwork2);

        ////trainingSet->currentSubSetSize = trainingSet->maxSubsetSize;

        HH_NeuralNetworks::simHH = true;
        ////HH_NeuralNetworks::simHH = false;
    }
    else
    {
        HH_NeuralNetworks::simHH = false;
        ////neuralNetworks->InitializeEvolutionNNs();
        ////neuralNetworks->InitializeEvolutionNNs("neuralnetworks/fittestNN_6.txt");
        //neuralNetworks->InitializeEvolutionNNs("test.txt");
        ////neuralNetworks->InitializeEvolutionNNs("neuralnetworks/fittestNN10_9_30.txt");
        neuralNetworks->InitializeNNs("");
        ////neuralNetworks->InitializeEvolutionNNs("NN.txt");
        ////neuralNetworks->InitializeEvolutionNNs("neuralnetworks/fittestNN30_9.txt");
        ////neuralNetworks->InitializeEvolutionNNs("NN26_9.txt");
        ////neuralNetworks->InitializeEvolutionNNs("fittestNN23_9.txt");
        ////neuralNetworks->InitializeEvolutionNNs("fittestNN.txt");
        ////neuralNetworks->InitializeEvolutionNNs("neuralnetworks/fittestNN_6_9.txt");
        HH_NeuralNetwork::LAYERS = neuralNetworks->neuralNetworks[0]->layers;
        ////neuralNetworks->InitializeEvolutionNNs("neuralnetworks/fittestNN23_9.txt");
    }*/

    ////neuralNetworks->InitializeNNs("neuralnetworks/fittestNN551888375.txt");
    ////neuralNetworks->InitializeNNs("neuralnetworks/fittestNN642343890.txt");
    neuralNetworks->InitializeNNs("");
    HH_NeuralNetwork::LAYERS = neuralNetworks->neuralNetworks[0]->layers;

    Vector foodPos;
    for (uint32_t i=0; i<FoodForAgents::maxFoodPerAgent; i++)
    {
        foodPos.Set((double) rand() / RAND_MAX * Environment::GRID_WIDTH, (double) rand() / RAND_MAX * Environment::GRID_HEIGHT, 0);

        for (uint32_t j=0; j<Agents::count; j++)
            Environment::foodForAgents_ptr->Add(NULL, j, &foodPos);
    }


    Environment::foodForAgents_ptr->ReGenerate();//saves food


    /* original for random weights
    if (HH_NeuralNetworks::electromagneticRF_HH)
    {
        //loadedNeuralNetwork = HH_NeuralNetwork::Load("neuralnetworks/nn10cols_4_20_6_new.txt");
        //loadedNeuralNetwork = HH_NeuralNetwork::Load("neuralnetworks/nn4cols_8_20_20_8.txt");
        //loadedNeuralNetwork = HH_NeuralNetwork::Load("neuralnetworks/nn5cols_8_30_30_30_8.txt");
        //loadedNeuralNetwork = HH_NeuralNetwork::Load("neuralnetworks/nn5cols_8_10_10_10_8.txt");
        //loadedNeuralNetwork = HH_NeuralNetwork::Load("neuralnetworks/nn3cols_8_8_8.txt");
        //loadedNeuralNetwork = HH_NeuralNetwork::Load("neuralnetworks/nn3cols_8_8_30.txt");
        //loadedNeuralNetwork = HH_NeuralNetwork::Load("neuralnetworks/nn3cols_8_8_12.txt");
        //loadedNeuralNetwork = HH_NeuralNetwork::Load("neuralnetworks/nn5cols_8_8_8_8_12.txt");

        loadedNeuralNetwork = new HH_NeuralNetwork(HH_NeuralNetwork::LAYERS, HH_NeuralNetwork::ROWS, &pos);
        //loadedNeuralNetwork = new HH_NeuralNetwork::HH_NeuralNetwork());

        loadedNeuralNetwork->SetTemperature(referenceTemperature);
        neuralNetworks->Add(loadedNeuralNetwork);

        ////loadedNeuralNetwork2 = new HH_NeuralNetwork(loadedNeuralNetwork->layers, loadedNeuralNetwork->rowCountsForColumns[layers-1]);
        ////loadedNeuralNetwork2->Copy(loadedNeuralNetwork);

        //loadedNeuralNetwork2 = new HH_NeuralNetwork::HH_NeuralNetwork();
        loadedNeuralNetwork2 = new HH_NeuralNetwork(HH_NeuralNetwork::LAYERS, HH_NeuralNetwork::ROWS, &pos);
        loadedNeuralNetwork2->Copy(loadedNeuralNetwork);

        loadedNeuralNetwork2->SetTemperature(referenceTemperature);
        loadedNeuralNetwork2->neurons->graphColor->Set(1,0,0);
        neuralNetworks->Add(loadedNeuralNetwork2);

        trainingSet->currentSubSetSize = trainingSet->maxSubsetSize;

        HH_NeuralNetworks::simHH = true;
    }
    else
    {
        HH_NeuralNetworks::simHH = false;
        neuralNetworks->InitializeEvolutionNNs();
    }
    */


    //loadedNeuralNetwork = HH_NeuralNetwork::Load("test.txt");

    /*pos.y = Graph::HEIGHT;
    neuralNetwork = new HH_NeuralNetwork(HH_NeuralNetwork::ROWS * HH_NeuralNetwork::LAYERS, pos);

    pos.y = -(HH_NeuralNetwork::ROWS * (Graph::HEIGHT + Graph::Y_GAP));
    neuralNetwork2 = new HH_NeuralNetwork(HH_NeuralNetwork::ROWS * HH_NeuralNetwork::LAYERS, pos);

    neuralNetwork2->CopyWeights(neuralNetwork);
    */

    //neuralNetworks = new HH_NeuralNetwork_ptr[neuralNetworksCount];

    //neuralNetwork2->neurons->pos.y -= HH_NeuralNetwork::ROWS * (Graph::WIDTH + Graph::Y_GAP);


    glutInit(&argc, argv);
    InitGL();

    //neuralNetworks->InitializeEvolutionNNs();

    Vector fitnessGraphPos(10000, -16000, 0);
    fitnessGraph = new FitnessGraph(fitnessGraphPos);

    Vector errorGraphPos(10000, -30000, 0);
    errorGraph = new FitnessGraph(errorGraphPos);

    Vector errorLayerPercentageGraphPos(10000, -44000, 0);
    errorLayerPercentageGraph = new FitnessGraph(errorLayerPercentageGraphPos);

    ////char graphFileName[255];

    sprintf(errorGraphFileName, "errorGraph%i.txt", GetTickCount());
    ////errorGraph->Save(errorGraphFileName);

    sprintf(errorLayerPercentageGraphFileName, "errorLayerPercentageGraph%i.txt", GetTickCount());
    ////errorLayerPercentageGraph->Save(errorLayerPercentageGraphFileName);

    ////electromagneticErrorForLayerTotal = new double[HH_NeuralNetwork::LAYERS];
    electromagneticErrorsForLayerTotal = new double_ptr[agents->count];
    electromagneticErrorPercentageForLayerTotal = new double_ptr[agents->count];

    for (int32_t j = 0; j < agents->count; j++)
    {
        electromagneticErrorsForLayerTotal[j] = new double[HH_NeuralNetwork::LAYERS];
        electromagneticErrorPercentageForLayerTotal[j] = new double[HH_NeuralNetwork::LAYERS];

        for (int32_t i = 0; i < HH_NeuralNetwork::LAYERS; i++)
        {
            electromagneticErrorsForLayerTotal[j][i] = 0;
            electromagneticErrorPercentageForLayerTotal[j][i] = 0;
        }
    }

    ////electromagneticErrorForLayerTotal = new double[HH_NeuralNetwork::LAYERS];

    ////fitnessGraph->Load("graphtest.txt");

    /*////
    uint16_t range = 1;

    uint32_t index = 0;

    double result;

    for (double i = -range; i < range; i+=0.1)
    {
        result = 1 / (1 + exp(-i*4));

        result = result * 2 - 1;

        result *= 10;

        fitnessGraph->AddPoint(result, 1, index++);
    }
    */


    /*////index = 0;

    for (double i = -range; i < range; i+=0.1)
    {
        result = 1 / (1 + exp(-i*0.2));

        result = result * 2 - 1;

        result *= 10;

        fitnessGraph->AddPoint(result, 30, index++);
    }*/



    /*for (int16_t i = -range; i < range; i++)
    {
        result = 1 / (1 + exp(-i*1.3));

        result *= 10;

        fitnessGraph->AddPoint(result, 1, i+range);



        /*
        result = 1 / (1 + exp(-i*0.06));

        //result *= 100;

        fitnessGraph->AddPoint(result, 2, i+range);



        result = 1 / (1 + exp(-i*0.09));

        //result *= 100;

        fitnessGraph->AddPoint(result, 3, i+range);
        */
    ////}

    ////Simulation::startNextGenerationSemaphore = CreateSemaphore(NULL, 0, 10, NULL);

    ////if (Simulation::startNextGenerationSemaphore == NULL)
        ////std::cout << "CreateSemaphoreObject error: " << GetLastError();

    if (Simulation::threading)
        agents->LaunchProcessingThreads();

    HANDLE threadHandle = (HANDLE)_beginthread(SimThreadFunction, 0, NULL);

    Sleep(1000);
    ////agents->AddNewNNHiddenLayerToAgents();

    //double num_loops = atoi(argv[1]);

    /*ENa = (gasConstant_R * Tk) / (Na_Valence * faradayConstant) * lnToLogConversionFactor;
    double logValue = (double) Na_Concentration_out/Na_Concentration_in;
    ENa *= log10(logValue);

    EK = (gasConstant_R * Tk) / (K_Valence * faradayConstant) * lnToLogConversionFactor;
    logValue = (double) K_Concentration_out/K_Concentration_in;
    EK *= log10(logValue);
    */


    /*ENa = 60.0;
    EK = -88.0;
    EL = -61.0;
    */

    ////Vector fitnessGraphPos(10000, 0, 0);
    ////fitnessGraph = new FitnessGraph(fitnessGraphPos);


    glutMainLoop();

    getchar();
}
