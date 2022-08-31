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

#ifndef FITNESS_GRAPH_H
#define FITNESS_GRAPH_H

#include "HH_NeuralNetworks.h"
#include "MembraneVoltageGraph.h"
#include "Vector.h"
#include "Color.h"

#include "pstdint.h"

class SeriesInformation
{
    public:
        char seriesTitle[255];
        double spikesPerSecond;
        double totalSignalingDuration;
};

class Graph
{
    private:
        uint32_t rollingAvgRange = 0; //number of deltas for specified milliseconds

        ////uint32_t MAX_SERIES = HH_NeuralNetworks::NETWORKS_COLUMNS * HH_NeuralNetworks::NETWORKS_ROWS + 1;
        uint32_t MAX_SERIES = 200;

        static const uint32_t colorsCount = 1020;
        Color graphColors[colorsCount];

        double** values;
        double* standardDeviationValues;
        double avgStandardDeviation;

        SeriesInformation* seriesInformation;

        char textBuffer[255];

        bool seriesColorsSet = false;
        bool maxSeriesColorSet = false;
        Color maxSeriesColor;
        Color avgSeriesColor;
        Color rollingAvgSeriesColor;

    public:
        char graphTitle[255];
        char fileName[255];

        Vector pos;
        double graphScale = 10;

        static const uint32_t WIDTH = 1000;
        static const uint32_t HEIGHT = 1000;
        static const uint32_t MARGIN_X = 30;
        static const uint32_t MARGIN_Y = 10;

        static const uint32_t MAX_INDEX = 60000;

        uint32_t* currentIndexes;
        uint32_t currentIndex = 0;
        uint32_t seriesCount = 0;

        double yScale = 10;
        double xScale = 30;

        uint32_t yLabels = 11;
        double yLabelInc = 10;

        bool drawValues = true;
        bool drawAverage = false;
        bool drawRollingAverage = false;
        bool calcStandardDeviation = false;
        bool drawStd = false;

        double totalRollingAverage = 0;

        Graph(Vector pos, uint32_t maxSeries = 0);

        void GenerateSignalStrengthColors();
        void SetGraphFileName(char* root, char* graphFolder, char* fileName, uint32_t time);

        void SetSeriesTitle(char* seriesTitle, uint8_t seriesIndex);
        void SetSpikesPerSecond(double spikesPerSecond, uint8_t seriesIndex);
        void SetSeriesColors(uint32_t index, double r, double g, double b);
        void SetMaxSeriesColor(double r, double g, double b);
        void SetAvgSeriesColor(double r, double g, double b);
        void SetRollingAvgSeriesColor(double r, double g, double b);
        void AddPoint(double value, uint8_t seriesIndex, int32_t index = -1);
        void Scroll();
        void CalculateStandardDeviationValues();
        void Draw();
        void Load(char *fileName);
        void Save(char *fileName, double resolution = 1);
        void ResetIndexes();
};

#endif
