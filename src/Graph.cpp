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
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "GL/glew.h"
#include "Graph.h"
#include "GraphicsAndUI.h"
#include "Simulation.h"

typedef double* double_ptr;

Graph::Graph(Vector pos, uint32_t maxSeries)
{
    if (maxSeries != 0)
        MAX_SERIES = maxSeries;

    values = new double_ptr[MAX_SERIES];
    standardDeviationValues = new double[MAX_INDEX];
    seriesInformation = new SeriesInformation[MAX_SERIES];
    currentIndexes = new uint32_t[MAX_SERIES];

    ////rollingAvgRange = 10 /*ms*/ / Simulation::deltaTime; //number of deltas for specified milliseconds

    rollingAvgRange = 10 /*ms*/ / Simulation::deltaTime; //number of deltas for specified milliseconds

    rollingAvgRange /= Simulation::graphDataPointIntervalDeltas; //number of graph data points for deltas


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

    avgSeriesColor.r = 0.0;
    avgSeriesColor.g = 1.0;
    avgSeriesColor.b = 0.0;

    rollingAvgSeriesColor.r = 0.0;
    rollingAvgSeriesColor.g = 1.0;
    rollingAvgSeriesColor.b = 0.0;
}

void Graph::GenerateSignalStrengthColors()
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

void Graph::SetGraphFileName(char* root, char* graphFolder, char* fileName, uint32_t time)
{
    sprintf(this->fileName, "%s/%s/%s%lu.txt", Simulation::saveRootFolder, Simulation::saveResultGraphsFolder, fileName, time);
}

void Graph::SetSeriesTitle(char* seriesTitle, uint8_t seriesIndex)
{
    strcpy(seriesInformation[seriesIndex].seriesTitle, seriesTitle);
}

void Graph::SetSpikesPerSecond(double spikesPerSecond, uint8_t seriesIndex)
{
    seriesInformation[seriesIndex].spikesPerSecond = spikesPerSecond;
}

void Graph::SetSeriesColors(uint32_t index, double r, double g, double b)
{
    graphColors[index].r = r;
    graphColors[index].g = g;
	graphColors[index].b = b;

	seriesColorsSet = true;
}

void Graph::SetMaxSeriesColor(double r, double g, double b)
{
    maxSeriesColor.r = r;
    maxSeriesColor.g = g;
    maxSeriesColor.b = b;

	maxSeriesColorSet = true;
}

void Graph::SetAvgSeriesColor(double r, double g, double b)
{
    avgSeriesColor.r = r;
    avgSeriesColor.g = g;
    avgSeriesColor.b = b;
}

void Graph::SetRollingAvgSeriesColor(double r, double g, double b)
{
    rollingAvgSeriesColor.r = r;
    rollingAvgSeriesColor.g = g;
    rollingAvgSeriesColor.b = b;
}

void Graph::AddPoint(double value, uint8_t seriesIndex, int32_t index)
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

void Graph::Scroll()
{
    for(uint32_t i = 0; i < seriesCount; i++)
    {
        memcpy(&values[i][0], &values[i][1], (Graph::MAX_INDEX-1) * sizeof(double));
    }
}

void Graph::CalculateStandardDeviationValues()
{
    double totalValues[MAX_INDEX];
    memset(totalValues, 0, MAX_INDEX * sizeof(double));

    double avgValues[MAX_INDEX];
    memset(avgValues, 0, MAX_INDEX * sizeof(double));

    for(uint8_t j = 0; j < seriesCount; j++)
    {
        for(uint32_t i = 0; i < currentIndexes[j]; i++)
        {
            totalValues[i] += values[j][i];
        }
    }

    for(uint32_t i = 0; i < MAX_INDEX; i++)
    {
        avgValues[i] = totalValues[i] / seriesCount;
    }


    double totalDif = 0, dif, totalStandardDeviation = 0;

    for(uint32_t i = 0; i < currentIndexes[0]; i++)
    {
        totalDif = 0;

        for(uint8_t j = 0; j < seriesCount; j++)
        {
            ////dif = values[j][i] - (totalValues[i] / seriesCount);
            dif = values[j][i] - avgValues[i];
            totalDif += dif * dif;
        }

        standardDeviationValues[i] = sqrt(totalDif/seriesCount);

        totalStandardDeviation += standardDeviationValues[i];
    }

    avgStandardDeviation = totalStandardDeviation/currentIndexes[0];
}

void Graph::Draw()
{
    glPushMatrix();
    glTranslatef(pos.x, pos.y, pos.z);
    glScalef(graphScale, graphScale, graphScale );

    glLineWidth(4);

    glColor3f(1, 1, 1);

	glBegin(GL_LINE_STRIP);
	glVertex2f(0, 0);
	glVertex2f(Graph::WIDTH, 0);
	glEnd();

	glBegin(GL_LINE_STRIP);
	glVertex2f(0, 0);
	glVertex2f(0, Graph::HEIGHT);
	glEnd();

	double yLabelValue = 0;

	double x = -(double) Graph::MARGIN_X;
	double y = 0;

	for(uint8_t i = 0; i < yLabels; i++)
    {
        sprintf(textBuffer, "%.0f", yLabelValue);

        GraphicsAndUI::DrawTextStr(textBuffer, x, y * yScale, 0, 0.1, 0);

        yLabelValue += yLabelInc;
        y += yLabelInc;
    }


    uint32_t xLabels = 100;
	double xLabelInc = 1;
	double xLabelValue = 0;

	x = 0;
	y = -(double) Graph::MARGIN_Y;

	for(uint8_t i = 0; i < xLabels; i++)
    {
        sprintf(textBuffer, "%.0f", xLabelValue);

        GraphicsAndUI::DrawTextStr(textBuffer, x * xScale, y, 0, 0.1, 0);

        xLabelValue += xLabelInc;
        x += xLabelInc;
    }

    glLineWidth(2);
    uint32_t colorIndex;


    double totalValues[MAX_INDEX];
    memset(totalValues, 0, MAX_INDEX * sizeof(double));

    double avgValues[MAX_INDEX];
    memset(avgValues, 0, MAX_INDEX * sizeof(double));

    if (drawValues)
    {
    for(uint8_t j = 0; j < seriesCount; j++)
    {
        if (seriesColorsSet)
        {
            colorIndex = j;
            glColor3f(graphColors[colorIndex].r, graphColors[colorIndex].g, graphColors[colorIndex].b);
        }
        else
            if (maxSeriesColorSet && j == seriesCount - 1)
                glColor3f(maxSeriesColor.r, maxSeriesColor.g, maxSeriesColor.b);
            else
            {
                colorIndex = ((double) (MAX_SERIES - 1 - j)) / (MAX_SERIES - 1) * (colorsCount - 1);
                glColor3f(graphColors[colorIndex].r, graphColors[colorIndex].g, graphColors[colorIndex].b);
            }

        glBegin(GL_LINE_STRIP);
        for(uint32_t i = 0; i < MAX_INDEX; i++)
        {
            glVertex2f(i * xScale, values[j][i] * yScale);
        }
        glEnd();


        glColor3f(0, 1, 0);
    }
    }

    for(uint8_t j = 0; j < seriesCount; j++)
        for(uint32_t i = 0; i < MAX_INDEX; i++)
        {
            totalValues[i] += values[j][i];

        }


        for(uint32_t i = 0; i < MAX_INDEX; i++)
        {
            avgValues[i] = totalValues[i] / seriesCount;
        }

    if (drawAverage && seriesCount>0)
    {
        glColor3f(avgSeriesColor.r, avgSeriesColor.g, avgSeriesColor.b);
        glBegin(GL_LINE_STRIP);
        for(uint32_t i = 0; i < MAX_INDEX; i++)
        {
            glVertex2f(i * xScale, avgValues[i] * yScale);
        }
        glEnd();
    }

    if (drawAverage || drawRollingAverage)
        calcStandardDeviation = true;

    if (calcStandardDeviation)
    {
        CalculateStandardDeviationValues();

        if (drawStd)
        {
            glColor3f(255.0/255, 145.0/255, 44.0/255);
            glBegin(GL_LINE_STRIP);
            for(uint32_t i = 0; i < MAX_INDEX; i++)
            {
                glVertex2f(i * xScale, standardDeviationValues[i] * yScale);
            }
            glEnd();
        }
    }

    if (drawRollingAverage && seriesCount > 0)
    {
        double value;

        int32_t rollingAvgStart;
        double rollingAverageTotalForRange = 0;
        double rollingAverage = 0;

        totalRollingAverage = 0;

        glColor3f(rollingAvgSeriesColor.r, rollingAvgSeriesColor.g, rollingAvgSeriesColor.b);
        glBegin(GL_LINE_STRIP);
        for(int32_t i = 0; i < MAX_INDEX; i++)
        {
            rollingAvgStart = i - rollingAvgRange;

                if (rollingAvgStart > 0)
                    rollingAverageTotalForRange -= avgValues[rollingAvgStart -1];

                rollingAverageTotalForRange += avgValues[i];

                totalRollingAverage += avgValues[i];

                glVertex2f(i * xScale, rollingAverageTotalForRange / (i - rollingAvgStart) * yScale);
        }
        glEnd();
    }

	glPopMatrix();
}

void Graph::Load(char *fileName)
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

void Graph::Save(char *fileName, double resolution)
{
    FILE* file;

    if (strlen(fileName)>0)
        file = fopen(fileName, "w");
    else
        file = fopen(this->fileName, "w");

    char textBuffer[255];
    uint32_t length;

    double totalValues[MAX_INDEX];
    memset(totalValues, 0, MAX_INDEX * sizeof(double));

    double avgValues[MAX_INDEX];
    memset(avgValues, 0, MAX_INDEX * sizeof(double));

    sprintf(textBuffer, "%s", graphTitle);
    length = strlen(textBuffer);
    fwrite(textBuffer, sizeof(char), length, file);

    double inc = 1;
    double maxIndex = 0;

    uint32_t k = 0;
    for(uint8_t j = 0; j < seriesCount; j++)
    {
        ////if (currentIndexes[j] > 0)
        {
            sprintf(textBuffer, "%s,", seriesInformation[j].seriesTitle);
            length = strlen(textBuffer);
            fwrite(textBuffer, sizeof(char), length, file);

            if (currentIndexes[j] > maxIndex)
                maxIndex = currentIndexes[j];

            inc = currentIndexes[j] / (currentIndexes[j] * resolution);

            k = 0;
            for(uint32_t i = 0; i < currentIndexes[j]; i+=inc)
            {
                if (i > 0)
                {
                    fwrite(",", sizeof(char), 1, file);
                }

                sprintf(textBuffer, "%f", values[j][i]);
                length = strlen(textBuffer);
                fwrite(textBuffer, sizeof(char), length, file);

                totalValues[k] += values[j][i];
                k++;
            }

            fwrite("\n", sizeof(char), 1, file);
        }
    }

    for(uint32_t i = 0; i < k; i++)
    {
        avgValues[i] = totalValues[i] / seriesCount;
    }

    ////if (currentIndexes[0] > 0)
    {
        inc = maxIndex / (maxIndex * resolution);

        fwrite("avg:,", sizeof(char), 5, file);
        for(uint32_t i = 0; i < k; i++)
        {
            if (i > 0)
            {
                fwrite(",", sizeof(char), 1, file);
            }

            sprintf(textBuffer, "%f", avgValues[i]);
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

        for(uint32_t i = 0; i < k; i++)
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



    if (drawRollingAverage && seriesCount > 0)
    {
        double value;

        int32_t rollingAvgStart;
        double rollingAverageTotalForRange = 0;
        double rollingAverage = 0;

        double rollingAvgRangeRes = rollingAvgRange * resolution;

        totalRollingAverage = 0;

        fwrite("rollingavg:,", sizeof(char), 12, file);
        for(uint32_t i = 0; i < k; i++)
        {
            if (i > 0)
            {
                fwrite(",", sizeof(char), 1, file);
            }

            rollingAvgStart = i - rollingAvgRangeRes;

                if (rollingAvgStart > 0)
                    rollingAverageTotalForRange -= avgValues[rollingAvgStart -1];

                rollingAverageTotalForRange += avgValues[i];

                totalRollingAverage += avgValues[i];

                sprintf(textBuffer, "%f", rollingAverageTotalForRange / (i - rollingAvgStart));
                length = strlen(textBuffer);
                fwrite(textBuffer, sizeof(char), length, file);
        }

        fwrite("\n", sizeof(char), 1, file);
    }

    fclose(file);
}

void Graph::ResetIndexes()
{
    for(uint32_t i = 0; i < seriesCount; i++)
    {
        currentIndexes[i] = 0;
    }
}

