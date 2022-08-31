#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "GL/glew.h"
#include "FitnessGraph.h"
#include "GraphicsUtilities.h"

typedef double* double_ptr;

FitnessGraph::FitnessGraph(Vector pos, uint32_t maxSeries)
{
    if (maxSeries != 0)
        MAX_SERIES = maxSeries;

    values = new double_ptr[MAX_SERIES];
    standardDeviationValues = new double[MAX_INDEX];
    seriesInformation = new SeriesInformation[MAX_SERIES];
    currentIndexes = new uint32_t[MAX_SERIES];

    memset(graphTitle, 0, 255);

    this->pos = pos;

    for(uint8_t j = 0; j < MAX_SERIES; j++)
    {
        values[j] = new double[MAX_INDEX];

        for(uint32_t i = 0; i < MAX_INDEX; i++)
        {
            values[j][i] = 0;
        }

        memset(seriesInformation[j].seriesTitle, 0, 255);

        currentIndexes[j] = 0;
    }

    GenerateSignalStrengthColors();
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

void FitnessGraph::SetSeriesColors(uint32_t index, double r, double g, double b)
{
    graphColors[index].r = r;
    graphColors[index].g = g;
	graphColors[index].b = b;

	seriesColorsSet = true;
}

void FitnessGraph::AddPoint(double value, uint8_t seriesIndex, int32_t index)
{
    if (index != -1) //if the x index is specified then it's used otherwise currentIndexes[seriesIndex] is used and incremented each time
        currentIndexes[seriesIndex] = index;

    if (currentIndexes[seriesIndex] == MAX_INDEX)
        currentIndexes[seriesIndex] = 0;


    values[seriesIndex][currentIndexes[seriesIndex]] = value;

    currentIndexes[seriesIndex]++;

    if ((seriesIndex + 1) > seriesCount)
        seriesCount = seriesIndex + 1;
}

void FitnessGraph::CalculateStandardDeviationValues()
{
    double totalValues[MAX_INDEX];
    memset(totalValues, 0, MAX_INDEX * 4);

    for(uint8_t j = 0; j < seriesCount; j++)
    {
        for(uint32_t i = 0; i < currentIndexes[j]; i++)
        {
            totalValues[i] += values[j][i];
        }
    }


    double totalDif, dif, totalStandardDeviation;

    for(uint32_t i = 0; i < currentIndexes[0]; i++)
    {
        totalDif = 0;

        for(uint8_t j = 0; j < seriesCount; j++)
        {
            dif = values[j][i] - (totalValues[i] / seriesCount);
            totalDif += dif * dif;
        }

        standardDeviationValues[i] = sqrt(totalDif/seriesCount);

        totalStandardDeviation += standardDeviationValues[i];
    }

    avgStandardDeviation = totalStandardDeviation/currentIndexes[0];
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


    uint32_t xLabels = 100;
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


    double totalValues[MAX_INDEX];

    memset(totalValues, 0, MAX_INDEX * 4);

    for(uint8_t j = 0; j < seriesCount; j++)
    {
        if (seriesColorsSet)
            colorIndex = j;
        else
            colorIndex = ((double) (MAX_SERIES - 1 - j)) / (MAX_SERIES - 1) * (colorsCount - 1);

        glColor3f(graphColors[colorIndex].r, graphColors[colorIndex].g, graphColors[colorIndex].b);

        glBegin(GL_LINE_STRIP);
        for(uint32_t i = 0; i < MAX_INDEX; i++)
        {
            glVertex2f(i * xScale, values[j][i] * scale);

            totalValues[i] += values[j][i];
        }
        glEnd();


        glColor3f(0, 1, 0);
    }

    if (drawAverage)
    {
        glColor3f(0, 1, 0);
        glBegin(GL_LINE_STRIP);
        for(uint32_t i = 0; i < MAX_INDEX; i++)
        {
            glVertex2f(i * xScale, totalValues[i] / seriesCount * scale);
        }
        glEnd();

        calcStandardDeviation = true;
    }

    if (calcStandardDeviation)
    {
        CalculateStandardDeviationValues();

        glColor3f(255.0/255, 145.0/255, 44.0/255);
        glBegin(GL_LINE_STRIP);
        for(uint32_t i = 0; i < MAX_INDEX; i++)
        {
            glVertex2f(i * xScale, standardDeviationValues[i] * scale);
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

    double totalValues[MAX_INDEX];
    memset(totalValues, 0, MAX_INDEX * 4);

    sprintf(textBuffer, "%s", graphTitle);
    length = strlen(textBuffer);
    fwrite(textBuffer, sizeof(char), length, file);

    for(uint8_t j = 0; j < seriesCount; j++)
    {
        if (currentIndexes[j] > 0)
        {
            sprintf(textBuffer, "%s,", seriesInformation[j].seriesTitle);
            length = strlen(textBuffer);
            fwrite(textBuffer, sizeof(char), length, file);

            for(uint32_t i = 0; i < currentIndexes[j]; i++)
            {
                if (i > 0)
                {
                    fwrite(",", sizeof(char), 1, file);
                }

                sprintf(textBuffer, "%f", values[j][i]);
                length = strlen(textBuffer);
                fwrite(textBuffer, sizeof(char), length, file);

                totalValues[i] += values[j][i];
            }

            fwrite("\n", sizeof(char), 1, file);
        }
    }

    if (currentIndexes[0] > 0)
    {
        fwrite("avg:,", sizeof(char), 5, file);
        for(uint32_t i = 0; i < currentIndexes[0]; i++)
        {
            if (i > 0)
            {
                fwrite(",", sizeof(char), 1, file);
            }

            sprintf(textBuffer, "%f", totalValues[i] / seriesCount);
            length = strlen(textBuffer);
            fwrite(textBuffer, sizeof(char), length, file);
        }
        fwrite("\n", sizeof(char), 1, file);
    }

    if (calcStandardDeviation)
    {
        CalculateStandardDeviationValues();

        sprintf(textBuffer, "standard deviation:,");
        length = strlen(textBuffer);
        fwrite(textBuffer, sizeof(char), length, file);

        for(uint32_t i = 0; i < currentIndexes[0]; i++)
        {
            if (i > 0)
            {
                fwrite(",", sizeof(char), 1, file);
            }

            sprintf(textBuffer, "%f", standardDeviationValues[i]);
            length = strlen(textBuffer);
            fwrite(textBuffer, sizeof(char), length, file);
        }

        fwrite("\n", sizeof(char), 1, file);
        sprintf(textBuffer, "avg standard deviation: %f", avgStandardDeviation);
        length = strlen(textBuffer);
        fwrite(textBuffer, sizeof(char), length, file);
        fwrite("\n", sizeof(char), 1, file);
    }

    fclose(file);
}
