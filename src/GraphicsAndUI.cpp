/*
 * Code System for the book "Solving Havana Syndrome and Biological Effects of RF
 * Using the Hodgkin-Huxley Neuron Model"
 * Copyright (C) 2022 by Clint Mclean <clint@mcleanresearchinstitute.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "GraphicsAndUI.h"
#include "GL/glew.h"
#include "GL/glut.h"

#include "Simulation.h"
#include "Environment.h"
#include "HH_NeuralNetworks.h"

GraphicsAndUI::GraphicsAndUI()
{
}

void GraphicsAndUI::InitGL()
{
    glutInitDisplayMode(GLUT_DEPTH |
		GLUT_DOUBLE |
		GLUT_RGBA |
		GLUT_MULTISAMPLE);

    glutInitWindowSize(glutGet(GLUT_SCREEN_WIDTH), glutGet(GLUT_SCREEN_HEIGHT));
    glutCreateWindow("Solving Havana Syndrome and Biological Effects of RF Using the Hodgkin-Huxley Neuron Model");

	GLenum err = glewInit();

	glutMouseFunc(GraphicsAndUI::MouseButtons);
	glutMotionFunc(GraphicsAndUI::MouseMotion);
	glutPassiveMotionFunc(GraphicsAndUI::PassiveMouseMotion);
	glutKeyboardFunc(GraphicsAndUI::ProcessKey);
	glutDisplayFunc(GraphicsAndUI::Render);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glPointSize(5);
	glShadeModel(GL_SMOOTH);
}

void GraphicsAndUI::DrawTextStr(char *string, float x, float y, float z, float scale, float angle)
{
    glPushMatrix();

    char *c;
    glMatrixMode(GL_MODELVIEW);

    glTranslatef(x, y, z);
    glScalef(scale, scale, scale);
    glRotatef(angle, 0, 0, 1);

    for (c = string; *c != '\0'; c++)
    {
        glutStrokeCharacter(GLUT_STROKE_ROMAN, *c);
    }

    glPopMatrix();
}

void GraphicsAndUI::DrawCircle(float cx, float cy, float r, int num_segments, bool filled)
{
    if (filled)
        glBegin(GL_TRIANGLE_FAN);
    else
        glBegin(GL_LINE_LOOP);

    for (int i = 0; i < num_segments; i++)
    {
        float theta = 2.0f * 3.1415926f * float(i) / float(num_segments);//get the current angle
        float x = r * cos(theta);//calculate the x component
        float y = r * sin(theta);//calculate the y component
        glVertex2f(x + cx, y + cy);//output vertex
    }
    glEnd();
}

void GraphicsAndUI::MouseButtons(int button, int state, int x, int y)
{
	currentMouseButton = button;

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
			fov -= zoomScale;
		else
			fov += zoomScale;

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

void GraphicsAndUI::MouseMotion(int x, int y)
{
	if (currentMouseButton == GLUT_LEFT_BUTTON)
	{
	}
    else if (currentMouseButton == GLUT_RIGHT_BUTTON || currentMouseButton == 1)
    {
        pos.x += (x - prevX)*translateScale;
        pos.y -= (y - prevY)*translateScale;
    }

	prevX = x;
	prevY = y;
}

void GraphicsAndUI::PassiveMouseMotion(int x, int y)
{
    prevX = x;
	prevY = y;
}

void GraphicsAndUI::ProcessKey(unsigned char key, int x, int y)
{
    uint32_t time;

    switch (key)
	{
        case ('a'):
            GraphicsAndUI::agentsVisible = !GraphicsAndUI::agentsVisible;
        break;
		case ('c'):
            GraphicsAndUI::connectionsVisible = !GraphicsAndUI::connectionsVisible;
        break;
		case ('g'):
		    if (GraphicsAndUI::nnVisible && GraphicsAndUI::graphsVisible)
                GraphicsAndUI::nnVisible = false;
            else if (GraphicsAndUI::graphsVisible)
                GraphicsAndUI::graphsVisible = false;
            else if (!GraphicsAndUI::nnVisible && !GraphicsAndUI::graphsVisible)
            {
                GraphicsAndUI::nnVisible = true;
            }
            else
                GraphicsAndUI::graphsVisible = true;
        break;
        case ('s'):
            Simulation::stimulus = !Simulation::stimulus;
        break;
        case ('S'):
            Simulation::resultsAndGraphs->SaveGraphs();
        break;
        case ('N'):
            incNoise = true;

            Simulation::MAX_NEURAL_NOISE += HH_NeuralNetworks::NEURAL_NOISE_DELTA;
        break;
        case ('n'):
            decNoise = true;

            Simulation::MAX_NEURAL_NOISE -= HH_NeuralNetworks::NEURAL_NOISE_DELTA;

                if (Simulation::MAX_NEURAL_NOISE < 0)
                    Simulation::MAX_NEURAL_NOISE = 0;
        break;
        case ('R'):
            resetNoise = true;
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
            Simulation::neuralNetworks->ResetHH();
        break;
        case ('>'):
            incSpeed = true;

            Simulation::lag-=1;

            if (Simulation::lag < 0)
                Simulation::lag = 0;

            incSpeed = false;
        break;
        case ('<'):
            decSpeed = true;
            Simulation::lag+=1;

            decSpeed = false;
        break;
        case ('v'):
            setVoltage = !setVoltage;
        break;
        case (' '):
            GraphicsAndUI::paused = !GraphicsAndUI::paused;
        break;
        case ('1'):
            pos = originalViewPos;
            fov = originalViewFov;
        break;
        case ('2'):
            pos.x = -19020;
            pos.y = -Simulation::resultsAndGraphs->errorLayerPercentageGraph->pos.y - Simulation::resultsAndGraphs->rfTemperatureLessNormalTemperatureCurrentSpikeCountGraph->graphScale * Graph::HEIGHT/2;
            fov = 48;
        break;
        case ('3'):
            pos.x = -19020;
            pos.y = -Simulation::resultsAndGraphs->increasedVoltageErrorGraph->pos.y - Simulation::resultsAndGraphs->rfTemperatureLessNormalTemperatureCurrentSpikeCountGraph->graphScale * Graph::HEIGHT/2;
            fov = 48;
        break;
        case ('4'):
            pos.x = -19020;
            pos.y = -Simulation::resultsAndGraphs->decreasedVoltageErrorGraph->pos.y - Simulation::resultsAndGraphs->rfTemperatureLessNormalTemperatureCurrentSpikeCountGraph->graphScale * Graph::HEIGHT/2;
            fov = 48;
        break;
        case ('5'):
            pos.x = -19020;
            pos.y = -Simulation::resultsAndGraphs->accruedErrorPercentageGraph->pos.y - Simulation::resultsAndGraphs->rfTemperatureLessNormalTemperatureCurrentSpikeCountGraph->graphScale * Graph::HEIGHT/2;
            fov = 48;
        break;
        case ('6'):
            pos.x = -19020;
            pos.y = -Simulation::resultsAndGraphs->rfTemperatureLessNormalTemperatureCurrentSpikeCountGraph->pos.y - Simulation::resultsAndGraphs->rfTemperatureLessNormalTemperatureCurrentSpikeCountGraph->graphScale * Graph::HEIGHT/2;
            fov = 48;
        break;
        case ('7'):
            pos.x = -19020;
            pos.y = -Simulation::resultsAndGraphs->rfTemperatureCurrentSpikeCountGraph->pos.y - Simulation::resultsAndGraphs->rfTemperatureLessNormalTemperatureCurrentSpikeCountGraph->graphScale * Graph::HEIGHT/2;
            fov = 48;
        break;
        default:
		break;
    }
}

void GraphicsAndUI::Resize()
{
    glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrtho(-1500, 15500, -3000, 4500, 1.0, 1000.0);

	glMatrixMode(GL_MODELVIEW);

	gluLookAt(0.0, 0.0, 0.0,
                0.0, 0.0, 0.0,
                0.0, 1.0, 0.);
}

void GraphicsAndUI::Render()
{
    if (drawingGraphics)
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        gluPerspective(fov, (GLdouble) glutGet(GLUT_WINDOW_WIDTH) / glutGet(GLUT_WINDOW_HEIGHT), 1.0, 20000.0);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glTranslatef(pos.x, pos.y, pos.z);

        HH_NeuralNetwork* neuralNetwork = Simulation::neuralNetworks->neuralNetworks[0];

        glColor3f(1, 1, 1);

        double textX = -1500;
        double textY = -2100;

        glColor3f(1, 1, 1);

        sprintf(graphicsText, "Generations: %i, Simulation Time: %f, Real Time: %i", Simulation::generations, Simulation::generationTime, (GetTickCount() - Simulation::simStartRealTime)/1000);
        GraphicsAndUI::DrawTextStr(graphicsText, -1500, textY, 0.0, 3.0);
        textY-=800;

        sprintf(graphicsText, "NN Layers: %d", Simulation::neuralNetworks->neuralNetworks[0]->neurons->layers);
        GraphicsAndUI::DrawTextStr(graphicsText, -1500, textY, 0.0, 3.0);
        textY-=800;

        sprintf(graphicsText, "Generation Duration: %f", Simulation::generationDuration);
        GraphicsAndUI::DrawTextStr(graphicsText, -1500, textY, 0.0, 3.0);
        textY-=800;

        sprintf(graphicsText, "Noise Level: %.16Lf", Simulation::MAX_NEURAL_NOISE);
        GraphicsAndUI::DrawTextStr(graphicsText, -1500, textY, 0.0, 3.0);
        textY-=800;

        sprintf(graphicsText, "RF Induced Temperatures From Pulses:");

        char temperatures[255];
        for (uint32_t i = 1; i<Simulation::agents->count; i++)
        {
            sprintf(temperatures, " %.16Lf", Simulation::rfInducedTemperaturesFromPulsesForAgents[i-1]);
            strcat(graphicsText, temperatures);
        }

        GraphicsAndUI::DrawTextStr(graphicsText, -1500, textY, 0.0, 3.0);
        textY-=800;

        if (Simulation::training)
            sprintf(graphicsText, "training: true");
        else
            sprintf(graphicsText, "training: false");

        GraphicsAndUI::DrawTextStr(graphicsText, -1500, textY, 0.0, 3.0);
        textY-=800;

        if (Simulation::testingLayerErrors)
            sprintf(graphicsText, "Testing Layer Errors");
        else
            sprintf(graphicsText, "AI Food Finding Task");

        GraphicsAndUI::DrawTextStr(graphicsText, -1500, textY, 0.0, 3.0);
        textY-=800;

        if (GraphicsAndUI::nnVisible)
            Simulation::neuralNetworks->Draw();

        glTranslatef(0, 0, 1);

        if (GraphicsAndUI::agentsVisible)
        {
            Environment::foodForAgents_ptr->Draw();
            Simulation::agents->Draw();
        }

        if (GraphicsAndUI::graphsVisible)
            Simulation::resultsAndGraphs->Draw();

        glutSwapBuffers();
    }

    glutPostRedisplay();
}

//sets the nns to overlay each other and the temperature affected nns' membrane voltage graphs to red
void GraphicsAndUI::SetNNsSameXYPosAndSetRedVoltages()
{
    for(uint32_t i = 1; i < Simulation::neuralNetworks->neuralNetworksCount; i++)
    {
        Simulation::neuralNetworks->neuralNetworks[i]->pos=Simulation::neuralNetworks->neuralNetworks[0]->pos;

        Simulation::neuralNetworks->neuralNetworks[i]->pos.z-=1;
        Simulation::neuralNetworks->neuralNetworks[i]->neurons->graphColor->Set(1,0,0);
    }
}

Vector GraphicsAndUI::originalViewPos(-14145,-6745,-15000);
double GraphicsAndUI::originalViewFov = 56;
Vector GraphicsAndUI::pos = GraphicsAndUI::originalViewPos;
double GraphicsAndUI::fov = GraphicsAndUI::originalViewFov;

double GraphicsAndUI::xRot = 0;
double GraphicsAndUI::yRot = 0;

double GraphicsAndUI::rotScale = 0.2;
double GraphicsAndUI::translateScale = 25;
double GraphicsAndUI::zoomScale = 1;

int GraphicsAndUI::prevX = -1;
int GraphicsAndUI::prevY = -1;

char GraphicsAndUI::graphicsText[] = {};

int GraphicsAndUI::currentMouseButton = 0;

bool GraphicsAndUI::setVoltage = false;

bool GraphicsAndUI::incNoise = false;
bool GraphicsAndUI::decNoise = false;
bool GraphicsAndUI::resetNoise = false;
bool GraphicsAndUI::prevIncNoise = false;

bool GraphicsAndUI::incStimulus = false;
bool GraphicsAndUI::decStimulus = false;
bool GraphicsAndUI::resetStimulus = false;

bool GraphicsAndUI::drawingGraphics = true;

bool GraphicsAndUI::decSpeed = false;
bool GraphicsAndUI::incSpeed = false;

bool GraphicsAndUI::paused = false;
double Simulation::lag = 0;

bool GraphicsAndUI::nnVisible = true;
bool GraphicsAndUI::graphsVisible = true;
bool GraphicsAndUI::agentsVisible = true;
bool GraphicsAndUI::connectionsVisible = true;
