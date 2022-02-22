#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "GL/glew.h"
#include "FitnessGraph.h"
#include "GraphicsUtilities.h"

FitnessGraph::FitnessGraph(Vector pos)
//FitnessGraph::FitnessGraph()
{
    this->pos = pos;

    for(uint8_t j = 0; j < MAX_SERIES; j++)
    {
        for(uint32_t i = 0; i < MAX_INDEX; i++)
        {
            values[j][i] = 0;
        }

        memset(seriesInformation[j].seriesTitle, 0, 255);

        currentIndexes[j] = 0;
    }

    /*////for(uint8_t j = 0; j < MAX_SERIES; j++)
    {
        graphColors[j].r = (double) rand() / RAND_MAX;
        graphColors[j].g = (double) rand() / RAND_MAX;
        graphColors[j].b = (double) rand() / RAND_MAX;
    }*/

    GenerateSignalStrengthColors();

    seriesColors[0].r = 1.0;
    seriesColors[0].g = 0.0;
	seriesColors[0].b = 0.0;

	seriesColors[1].r = 0.0;
    seriesColors[1].g = 1.0;
	seriesColors[1].b = 0.0;

	seriesColors[2].r = 1.0;
    seriesColors[2].g = 1.0;
	seriesColors[2].b = 0.0;

	seriesColors[3].r = 255.0/255;
    seriesColors[3].g = 145.0/255;
	seriesColors[3].b = 44.0/255;
}

void FitnessGraph::GenerateSignalStrengthColors()
{
	uint8_t stage = 0;

	float r = 255, g = 0, b = 0;

	uint32_t colorIndex = 0;

	while (stage != 4)
	{
		graphColors[colorIndex].r = r / 255;
		graphColors[colorIndex].g = g / 255;
		graphColors[colorIndex].b = b / 255;

		colorIndex++;

		switch (stage)
		{
		case (0):
			g++;

			if (g == 255)
				stage++;
			break;


		case (1):
			r--;

			if (r == 0)
				stage++;
			break;


		case (2):
			b++;

			if (b == 255)
				stage++;
			break;

		case (3):
			g--;

			if (g == 0)
				stage++;
			break;
		}
	}
}

void FitnessGraph::SetSeriesTitle(char* seriesTitle, uint8_t seriesIndex)
{
    strcpy(seriesInformation[seriesIndex].seriesTitle, seriesTitle);
}

void FitnessGraph::SetSpikesPerSecond(double spikesPerSecond, uint8_t seriesIndex)
{
    seriesInformation[seriesIndex].spikesPerSecond = spikesPerSecond;
}

void FitnessGraph::SetTotalSignalingDuration(double totalSignalingDuration, uint8_t seriesIndex)
{
    seriesInformation[seriesIndex].totalSignalingDuration = totalSignalingDuration;
}

void FitnessGraph::SetSimulationTime(double simulationTime, uint8_t seriesIndex)
{
    seriesInformation[seriesIndex].simulationTime = simulationTime;
}

void FitnessGraph::AddPoint(double value, uint8_t seriesIndex, int32_t index)
{
    if (index != -1)
        currentIndexes[seriesIndex] = index;

    if (currentIndexes[seriesIndex] == MAX_INDEX)
        currentIndexes[seriesIndex] = 0;

    //values[currentIndex].x = currentIndex;
    values[seriesIndex][currentIndexes[seriesIndex]] = value;
    //values[currentIndex].z = 0;

    ////values[0][seriesIndex] = value;

    /*////if (index != -1)
        currentIndex = index;

    if (currentIndex == MAX_INDEX)
        currentIndex = 0;

    //values[currentIndex].x = currentIndex;
    values[seriesIndex][currentIndexes[seriesIndex]] = value;
    */

    currentIndexes[seriesIndex]++;
    ////currentIndex++;

    if ((seriesIndex + 1) > seriesCount)
        seriesCount = seriesIndex + 1;
}

void FitnessGraph::Draw()
{
    glPushMatrix();
    glTranslatef(pos.x, pos.y, pos.z);
    glScalef(10, 10, 10);

    glLineWidth(4);

    glColor3f(1, 1, 1);

	glBegin(GL_LINE_STRIP);
	glVertex2f(0, 0);
	glVertex2f(FitnessGraph::WIDTH, 0);
	glEnd();

	glBegin(GL_LINE_STRIP);
	glVertex2f(0, 0);
	glVertex2f(0, FitnessGraph::HEIGHT);
	glEnd();


	uint32_t yLabels = 11;
	double yLabelInc = 10;
	double yLabelValue = 0;

	double x = -(double) FitnessGraph::MARGIN_X;
	double y = 0;

	for(uint8_t i = 0; i < yLabels; i++)
    {
        sprintf(textBuffer, "%.0f", yLabelValue);

        GraphicsUtilities::DrawTextStr(textBuffer, x, y * scale, 0, 0.1, 0);

        yLabelValue += yLabelInc;
        y += yLabelInc;
    }


    uint32_t xLabels = 30;
	double xLabelInc = 1;
	double xLabelValue = 0;

	x = 0;
	y = -(double) FitnessGraph::MARGIN_Y;

	for(uint8_t i = 0; i < xLabels; i++)
    {
        sprintf(textBuffer, "%.0f", xLabelValue);

        GraphicsUtilities::DrawTextStr(textBuffer, x * xScale, y, 0, 0.1, 0);

        xLabelValue += xLabelInc;
        x += xLabelInc;
    }

    glLineWidth(2);

    uint32_t colorIndex;

    ////for(uint8_t j = 0; j < MAX_SERIES; j++)
    for(uint8_t j = 0; j < seriesCount; j++)
    {

        colorIndex = ((double) (MAX_SERIES - 1 - j)) / (MAX_SERIES - 1) * (colorsCount - 1);
        glColor3f(graphColors[colorIndex].r, graphColors[colorIndex].g, graphColors[colorIndex].b);


        ////glColor3f(seriesColors[j].r, seriesColors[j].g, seriesColors[j].b);

        glBegin(GL_LINE_STRIP);
        for(uint32_t i = 0; i < MAX_INDEX; i++)
        {
            glVertex2f(i * xScale, values[j][i] * scale);
        }
        glEnd();
    }

	glPopMatrix();
}

void FitnessGraph::Load(char *fileName)
{
    FILE* file = fopen(fileName, "r");
    if (file)
    {
    char textBuffer[9999];
    memset(textBuffer, 0, 9999);

    char *ptr;
    double simTemperature, value;
    uint8_t seriesIndex = 0, index = 0;;

    while (fgets(textBuffer, 9999, file))
    {
        ptr = strtok(textBuffer, ",");
        SetSeriesTitle(ptr, seriesIndex);

        ptr = strtok(NULL, ",");
        SetSpikesPerSecond(atof(ptr), seriesIndex);

        ptr = strtok(NULL, ",");
        SetTotalSignalingDuration(atof(ptr), seriesIndex);

        ptr = strtok(NULL, ",");
        SetSimulationTime(atof(ptr), seriesIndex);

        index = 0;
        while (ptr != NULL)
        {
            ptr = strtok (NULL, ",");

            value = atof(ptr);

            AddPoint(value, seriesIndex, index++);
        }

        seriesIndex++;
    }

    fclose(file);
    }
}

void FitnessGraph::Save(char *fileName)
{
    FILE* file = fopen(fileName, "w");

    char textBuffer[255];
    uint32_t length;

    for(uint8_t j = 0; j < MAX_SERIES; j++)
    {
        /*double temperature = 37.0001;
        sprintf(textBuffer, "%f", temperature);
        length = strlen(textBuffer);
        fwrite(textBuffer, sizeof(char), length, file);
        */

        ////if (strcmp(seriesInformation[j].seriesTitle, "") != 0)
        {

        if (currentIndexes[j] > 0)
        {
        sprintf(textBuffer, "%s, ", seriesInformation[j].seriesTitle);
        length = strlen(textBuffer);
        fwrite(textBuffer, sizeof(char), length, file);

        /*////
        sprintf(textBuffer, "%f, ", seriesInformation[j].spikesPerSecond);
        length = strlen(textBuffer);
        fwrite(textBuffer, sizeof(char), length, file);

        sprintf(textBuffer, "%f, ", seriesInformation[j].totalSignalingDuration);
        length = strlen(textBuffer);
        fwrite(textBuffer, sizeof(char), length, file);

        sprintf(textBuffer, "%f", seriesInformation[j].simulationTime);
        length = strlen(textBuffer);
        fwrite(textBuffer, sizeof(char), length, file);
        */

        ////for(uint32_t i = 0; i < MAX_INDEX; i++)

        for(uint32_t i = 0; i < currentIndexes[j]; i++)
        {
            if (i > 0)
            {
                sprintf(textBuffer, ", ");
                length = strlen(textBuffer);
                fwrite(", ", sizeof(char), 2, file);
            }

            if (values[j][i] > 0)
                int grc = 1;

            sprintf(textBuffer, "%f", values[j][i]);
            length = strlen(textBuffer);
            fwrite(textBuffer, sizeof(char), length, file);
        }

        fwrite("\n", sizeof(char), 1, file);
        }
        }
    }

    fclose(file);
}
