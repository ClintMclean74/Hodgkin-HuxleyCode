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

#include "GL/glew.h"
#include "GL/glut.h"
#include "HH_NeuralNetworks.h"
//#include "HH_NeuralNetwork.h"
#include "Graph.h"
#include "GraphicsUtilities.h"
#include "TrainingSet.h"
#include "FitnessGraph.h"

/*
uint32_t neuralNetworksCount = 16;
uint32_t neuralNetworksColumns = 4;
uint32_t neuralNetworksRows = 4;
*/

//const static uint32_t Graph::MAX_GRAPH_INDEX = 100000;
//const double minGraphHeight = -1200, maxGraphHeight = 1200;
double minGraphHeight, maxGraphHeight;
//const double minGraphHeight = -300, maxGraphHeight = 300;
//const double minGraphX = 25000, maxGraphX = 30000;
//const double minGraphX = 0, maxGraphX = 100000;
double minGraphX = 0, maxGraphX = Graph::MAX_GRAPH_INDEX;
//const double minGraphX = 0, maxGraphX = 100;

double graphMembraneVoltageData[Graph::MAX_GRAPH_INDEX];
double graph_m[Graph::MAX_GRAPH_INDEX];
double graph_h[Graph::MAX_GRAPH_INDEX];
double graph_n[Graph::MAX_GRAPH_INDEX];
double graph_NaCurrent[Graph::MAX_GRAPH_INDEX];
double graph_KCurrent[Graph::MAX_GRAPH_INDEX];
double graph_LCurrent[Graph::MAX_GRAPH_INDEX];


//double stimulus = 0.0;
double delataTime = 0.01;
//double delataTime = 0.02;
double num_samples = 100000;

HH_NeuralNetworks neuralNetworks;

FitnessGraph* fitnessGraph;

//HH_NeuralNetwork* neuralNetwork;
//HH_NeuralNetwork* neuralNetwork2;


uint32_t simCount = 0;

double stimulusDuration = 1;
double stimulusInterval = 4;

//double stimulusDuration = 1;
//double stimulusDuration = 0.1;
//double stimulusDuration = 0.01; //For non HH sim

//double stimulusInterval = 1;

//double stimulusMagnitude = 0.01;
//double stimulusMagnitude = 0.14;
double stimulusMagnitude = 0.01;

bool processing = true;

char graphicsText[255];

//char layer1Stimulus[HH_NeuralNetwork::ROWS[0]];
char layer1Stimulus[4];

TrainingSet trainingSet(16);


Vector pos (-6720,-3620,-15000);

double xRot = 0;
double yRot = 0;

double rotScale = 0.2;
double translateScale = 10;
double zoomScale = 2;

double fov = 44;

int prevX = -1;
int prevY = -1;

int currentButton = 0;


void display(void)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

	gluPerspective(fov, (GLdouble)glutGet(GLUT_WINDOW_HEIGHT) / glutGet(GLUT_WINDOW_WIDTH) * 2.7, 1.0, 20000.0);


	glMatrixMode(GL_MODELVIEW);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();

	glTranslatef(pos.x, pos.y, pos.z);

	/*glColor3f(1, 1, 1);
    sprintf(graphicsText, "Training Set Index: %i", trainingSet->currentQAIndex);
    GraphicsUtilities::DrawText2(graphicsText, 0, 3700, 0.0, 3.0);
    */

    HH_NeuralNetwork* neuralNetwork = neuralNetworks.neuralNetworks[0];
    //uint8_t_ptr* layerSignaling = neuralNetwork->neurons->GetLayersSignaling();

        glColor3f(1, 1, 1);

        double textX = -1500;
        double textY = 800;

        //sprintf(graphicsText, "L1N0: %d, L1N1: %i, L1N2: %i, L1N3: %i", layerSignaling[0][0], layerSignaling[0][1], layerSignaling[0][2], layerSignaling[0][3]);
        //GraphicsUtilities::DrawText2(graphicsText, textX, textY + 600, 0.0, 0.9);

        sprintf(graphicsText, "L1BufferN0: %d, L1BufferN1: %i, L1BufferN2: %i, L1BufferN3: %i", neuralNetwork->neurons->layer1SignalsBuffer[0], neuralNetwork->neurons->layer1SignalsBuffer[1], neuralNetwork->neurons->layer1SignalsBuffer[2], neuralNetwork->neurons->layer1SignalsBuffer[3]);
        GraphicsUtilities::DrawText2(graphicsText, textX, textY + 400, 0.9);

        //sprintf(graphicsText, "L2N0: %d, L2N1: %i, L2N2: %i, L2N3: %i", layerSignaling[1][0], layerSignaling[1][1], layerSignaling[1][2], layerSignaling[1][3]);
        //GraphicsUtilities::DrawText2(graphicsText, textX, textY + 200, 0.0, 0.9);

        sprintf(graphicsText, "L2BufferN0: %d, L2BufferN1: %i, L2BufferN2: %i, L2BufferN3: %i", neuralNetwork->neurons->resultLayerSignalsBuffer[0], neuralNetwork->neurons->resultLayerSignalsBuffer[1], neuralNetwork->neurons->resultLayerSignalsBuffer[2], neuralNetwork->neurons->resultLayerSignalsBuffer[3]);
        GraphicsUtilities::DrawText2(graphicsText, textX, textY + 200, 0.0, 0.9);

        /*
        glColor3f(1, 1, 1);
        sprintf(graphicsText, "Average Error: %.2f", neuralNetworks.averageError);
        GraphicsUtilities::DrawText2(graphicsText, -1500, -1500, 0.0, 3.0);
        */

        glColor3f(1, 1, 1);
        sprintf(graphicsText, "Training Index, Sub Set Size: %d, %d, Average Error: %.2f", trainingSet.currentQAIndex, trainingSet.currentSubSetSize, neuralNetworks.averageError);
        GraphicsUtilities::DrawText2(graphicsText, -1500, -1500, 0.0, 3.0);


    neuralNetworks.Draw();


    //neuralNetwork->Draw();

    fitnessGraph->Draw();

	glutSwapBuffers();

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
        GraphicsUtilities::DrawText2(graphicsText, 1500, 1900, 0.0, 0.9);

        sprintf(graphicsText, "L1BufferN0: %d, L1BufferN1: %i, L1BufferN2: %i, L1BufferN3: %i", neuralNetwork->neurons->layer1SignalsBuffer[0], neuralNetwork->neurons->layer1SignalsBuffer[1], neuralNetwork->neurons->layer1SignalsBuffer[2], neuralNetwork->neurons->layer1SignalsBuffer[3]);
        GraphicsUtilities::DrawText2(graphicsText, 1500, 1600, 0.0, 0.9);

        sprintf(graphicsText, "L2N0: %d, L2N1: %i, L2N2: %i, L2N3: %i", layerSignaling[1][0], layerSignaling[1][1], layerSignaling[1][2], layerSignaling[1][3]);
        GraphicsUtilities::DrawText2(graphicsText, 1500, 1300, 0.0, 0.9);

        sprintf(graphicsText, "L2BufferN0: %d, L2BufferN1: %i, L2BufferN2: %i, L2BufferN3: %i", neuralNetwork->neurons->resultLayerSignalsBuffer[0], neuralNetwork->neurons->resultLayerSignalsBuffer[1], neuralNetwork->neurons->resultLayerSignalsBuffer[2], neuralNetwork->neurons->resultLayerSignalsBuffer[3]);
        GraphicsUtilities::DrawText2(graphicsText, 1500, 1100, 0.0, 0.9);

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
    glutCreateWindow("SDR Reradiation Spectrum Analyzer");

	GLenum err = glewInit();

	glutMouseFunc(MouseButtons);
	glutMotionFunc(MouseMotion);
	glutPassiveMotionFunc(PassiveMouseMotion);
	glutDisplayFunc(display);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(0.0, 0.0, 0.0, 0.0);

	glPointSize(5);

	glShadeModel(GL_SMOOTH);

	//Resize();
}

void SimThreadFunction(void *param)
 {
     double startStimulusTime, endStimulusTime, currentSimTime;

     currentSimTime = 0;

     startStimulusTime  = - stimulusDuration - stimulusInterval;

     TrainingQA* currentQA = trainingSet.GetCurrentQA();
     TrainingQA intervalQA(HH_NeuralNetwork::ROWS[0], HH_NeuralNetwork::ROWS[HH_NeuralNetwork::LAYERS-1]);

     double simHHInterval = 0;

     while (processing)
     {
        currentSimTime += delataTime;

        //neuralNetworks.Process(delataTime);

        if (currentSimTime >= startStimulusTime + stimulusDuration + stimulusInterval)
        {
            startStimulusTime = currentSimTime;

            //if (trainingSet.currentQAIndex == trainingSet.size)
            if (trainingSet.currentQAIndex == trainingSet.currentSubSetSize)
            {
                for(uint32_t i = 0; i < neuralNetworks.neuralNetworksCount; i++)
                {
                    neuralNetworks.neuralNetworks[i]->neurons->newLayer1Signals = true;
                }

                for(uint32_t i = 0; i < neuralNetworks.neuralNetworksCount; i++)
                {
                    //neuralNetworks.SetTemperature(37, i);
                    neuralNetworks.SetQA(currentQA, i);
                    //neuralNetworks.DetermineError(i);
                    //neuralNetworks.SetLayer1Stimulus(currentQA->q, i);
                }

                neuralNetworks.DetermineErrors();
                neuralNetworks.DetermineError(0, true);

                fitnessGraph->AddPoint(neuralNetworks.GetAverageError());

                uint32_t* fittestNNIndexes = neuralNetworks.GetFittestNNIndexes();

                neuralNetworks.CopyWeights(fittestNNIndexes);

                if (neuralNetworks.neuralNetworks[fittestNNIndexes[0]]->neurons->totalError > 0)
                //if (neuralNetworks.GetAverageTotalError() > 0)
                {
                    neuralNetworks.Mutate();
                }
                else
                    if (trainingSet.currentSubSetSize < trainingSet.maxSubsetSize)
                        trainingSet.currentSubSetSize++;

                if (neuralNetworks.neuralNetworks[fittestNNIndexes[0]]->neurons->totalError <= 0 && trainingSet.currentSubSetSize >= trainingSet.maxSubsetSize)
                {
                    stimulusDuration = TrainingSet::stimulusDuration;
                    stimulusInterval = TrainingSet::stimulusInterval;

                    neuralNetworks.visible = true;

                    //HH_NeuralNetworks::simHH = true;

                    simHHInterval++;

                    if (simHHInterval>1)
                    {
                        if (!HH_NeuralNetworks::simHH)
                            trainingSet.currentSubSetSize = 1;

                        HH_NeuralNetworks::simHH = true;
                        neuralNetworks.ResetHH();
                    }
                }
                else
                {
                    if (HH_NeuralNetworks::simHH)
                    {
                        stimulusDuration = TrainingSet::stimulusDuration;
                        stimulusInterval = TrainingSet::stimulusInterval;
                    }
                    else
                    {
                        stimulusDuration = TrainingSet::nonHHStimulusDuration;
                        stimulusInterval = TrainingSet::nonHHstimulusInterval;
                    }
                }

                neuralNetworks.ResetTotalError();

                //neuralNetworks.ResetHH();
            }

            neuralNetworks.ResetHH();
            currentQA = trainingSet.GetCurrentQA();


            for(uint32_t i = 0; i < neuralNetworks.neuralNetworksCount; i++)
            {
                //neuralNetworks.SetTemperature(37, i);
                neuralNetworks.SetQA(currentQA, i);
                //neuralNetworks.DetermineError(i);
                //neuralNetworks.SetLayer1Stimulus(currentQA->q, i);
            }

        }

        if (stimulusDuration == 0)
        {
        for(uint32_t i = 0; i < neuralNetworks.neuralNetworksCount; i++)
            {
                neuralNetworks.SetTemperature(37, i);
                //neuralNetworks.SetQA(currentQA, i);
                //neuralNetworks.SetLayer1Stimulus(layer1Stimulus, i);
            }

            for(uint32_t i = 0; i < neuralNetworks.neuralNetworksCount; i++)
            {
                //neuralNetworks.SetTemperature(37, i);
                neuralNetworks.SetQA(currentQA, i);
                //neuralNetworks.SetLayer1Stimulus(currentQA->q, i);
            }

            neuralNetworks.Process(delataTime);
            neuralNetworks.DetermineErrors();
            neuralNetworks.DetermineError(0, true);
        }


         /*if (currentSimTime - startStimulusTime < stimulusDuration)
         {
            for(uint32_t i = 0; i < neuralNetworks.neuralNetworksCount; i++)
            {
                neuralNetworks.SetTemperature(37, i);
                neuralNetworks.SetQA(currentQA, i);
                //neuralNetworks.SetLayer1Stimulus(layer1Stimulus, i);
            }
         }
        else
            */
        if (currentSimTime - startStimulusTime > stimulusDuration)
        {
            layer1Stimulus[0] = 0;
            layer1Stimulus[1] = 0;
            layer1Stimulus[2] = 0;
            layer1Stimulus[3] = 0;

            for(uint32_t i = 0; i < neuralNetworks.neuralNetworksCount; i++)
            {
                neuralNetworks.SetTemperature(37, i);
                //neuralNetworks.SetQA(&intervalQA, i);
                //neuralNetworks.SetLayer1Stimulus(intervalQA.q, i);
            }

            /*for(uint32_t i = 0; i < HH_NeuralNetwork::ROWS; i++)
            {
                layer1Stimulus[i] = (uint8_t) ((double) rand()/ RAND_MAX * 1 + 0.5);
            }*/

            currentQA = &intervalQA;

            for(uint32_t i = 0; i < neuralNetworks.neuralNetworksCount; i++)
            {
                //neuralNetworks.SetTemperature(37, i);
                neuralNetworks.SetQA(currentQA, i, false);
                //neuralNetworks.SetLayer1Stimulus(currentQA->q, i);
            }

        }
        //else


        neuralNetworks.Process(delataTime);
        neuralNetworks.DetermineErrors();
        neuralNetworks.DetermineError(0, true);

        neuralNetworks.GetAverageError();

         //fitnessGraph->AddPoint(neuralNetworks.averageError);

         //Sleep(1);
     }
 }

/*void SimThreadFunction3(void *param)
 {
     double startStimulusTime, endStimulusTime, currentSimTime;

     currentSimTime = 0;

     startStimulusTime  = - stimulusDuration - stimulusInterval;

     TrainingQA* currentQA = trainingSet.GetCurrentQA();
     TrainingQA intervalQA(HH_NeuralNetwork::ROWS);

     while (processing)
     {
         currentSimTime += delataTime;

        if (currentSimTime >= startStimulusTime + stimulusDuration + stimulusInterval)
        {
            startStimulusTime = currentSimTime;

            //if (trainingSet.currentQAIndex == trainingSet.size)
            if (trainingSet.currentQAIndex == trainingSet.currentSubSetSize)
            {
                uint32_t fittestNNIndex = neuralNetworks.GetFittestNNIndex();

                neuralNetworks.CopyWeights(fittestNNIndex);

                if (neuralNetworks.neuralNetworks[fittestNNIndex]->neurons->totalError > 0)
                //if (neuralNetworks.GetAverageTotalError() > 0)
                {
                    neuralNetworks.Mutate();
                }
                else
                    if (trainingSet.currentSubSetSize < trainingSet.maxSubsetSize)
                        trainingSet.currentSubSetSize++;

                if (neuralNetworks.neuralNetworks[fittestNNIndex]->neurons->totalError <= 0 && trainingSet.currentSubSetSize >= trainingSet.maxSubsetSize)
                {
                    stimulusDuration = 1;
                    stimulusInterval = 1;

                    neuralNetworks.visible = true;
                }
                else
                {
                    stimulusDuration = 0.0000000001;
                    stimulusInterval = 0.0000000001;
                }

                neuralNetworks.ResetTotalError();
            }

            currentQA = trainingSet.GetCurrentQA();

            for(uint32_t i = 0; i < neuralNetworks.neuralNetworksCount; i++)
            {
                neuralNetworks.SetTemperature(37, i);
                //neuralNetworks.SetQA(currentQA, i);
                //neuralNetworks.SetLayer1Stimulus(layer1Stimulus, i);
            }
        }

         /*if (currentSimTime - startStimulusTime < stimulusDuration)
         {
            for(uint32_t i = 0; i < neuralNetworks.neuralNetworksCount; i++)
            {
                neuralNetworks.SetTemperature(37, i);
                neuralNetworks.SetQA(currentQA, i);
                //neuralNetworks.SetLayer1Stimulus(layer1Stimulus, i);
            }
         }
        else
            */
/*        if (currentSimTime - startStimulusTime > stimulusDuration)
        {
            layer1Stimulus[0] = 0;
            layer1Stimulus[1] = 0;
            layer1Stimulus[2] = 0;
            layer1Stimulus[3] = 0;

            for(uint32_t i = 0; i < neuralNetworks.neuralNetworksCount; i++)
            {
                neuralNetworks.SetTemperature(37, i);
                //neuralNetworks.SetQA(&intervalQA, i);
                //neuralNetworks.SetLayer1Stimulus(intervalQA.q, i);
            }

            /*for(uint32_t i = 0; i < HH_NeuralNetwork::ROWS; i++)
            {
                layer1Stimulus[i] = (uint8_t) ((double) rand()/ RAND_MAX * 1 + 0.5);
            }*/

/*            currentQA = &intervalQA;
        }

        for(uint32_t i = 0; i < neuralNetworks.neuralNetworksCount; i++)
            {
                //neuralNetworks.SetTemperature(37, i);
                neuralNetworks.SetQA(currentQA, i);
                //neuralNetworks.SetLayer1Stimulus(currentQA->q, i);
            }

         neuralNetworks.Process(delataTime);

         Sleep(1);
     }
 }
 */


 /*
 void SimThreadFunction2(void *param)
 {
     double startStimulusTime, endStimulusTime, currentSimTime;

     currentSimTime = startStimulusTime = 0;

     while (processing)
     {
         currentSimTime += delataTime;

        if (currentSimTime > startStimulusTime + stimulusDuration + stimulusInterval)
        {
            //startStimulusTime = GetTickCount();
            startStimulusTime = currentSimTime;

            //stimulusMagnitude += 0.001;
        }

        //currentTime = GetTickCount();
        //currentSimTime += delataTime;

         if (currentSimTime - startStimulusTime < stimulusDuration)
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

         //neuralNetwork->Process(stimulus, delataTime, 100, 70000);
         //neuralNetwork->SetSynStimulus(stimulus);
/*
         neuralNetwork->Process(delataTime);
         neuralNetwork2->Process(delataTime);

         Sleep(1);
     }
 }
 */

int main(int argc, char *argv[])
{
    if(argc < 2)
    {
        std::cerr << "requires number of benchmarking loops as command line argument #1";
        return 1;
    }

    Vector pos;

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

    HANDLE threadHandle = (HANDLE)_beginthread(SimThreadFunction, 0, NULL);

    double num_loops = atoi(argv[1]);

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

    Vector fitnessGraphPos(10000, 0, 0);
    fitnessGraph = new FitnessGraph(fitnessGraphPos);


    glutMainLoop();

    getchar();
}
