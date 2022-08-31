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

class FitnessGraph
{
    private:
        Vector pos;

        static const uint32_t WIDTH = 1000;
        static const uint32_t HEIGHT = 1000;
        static const uint32_t MARGIN_X = 30;
        static const uint32_t MARGIN_Y = 10;

        static const uint32_t MAX_INDEX = 10000;
        uint32_t MAX_SERIES = HH_NeuralNetworks::NETWORKS_COLUMNS * HH_NeuralNetworks::NETWORKS_ROWS + 1;

        static const uint32_t colorsCount = 1020;
        Color graphColors[colorsCount];

        double** values;
        double* standardDeviationValues;
        double avgStandardDeviation;

        SeriesInformation* seriesInformation;

        char textBuffer[255];

        bool seriesColorsSet = false;

    public:
        char graphTitle[255];

        uint32_t* currentIndexes;
        uint32_t currentIndex = 0;
        uint32_t seriesCount = 0;

        double scale = 10;
        double xScale = 30;

        uint32_t yLabels = 11;
        double yLabelInc = 10;

        bool drawAverage = false;
        bool calcStandardDeviation = false;

        FitnessGraph(Vector pos, uint32_t maxSeries = 0);

        void GenerateSignalStrengthColors();

        void SetSeriesTitle(char* seriesTitle, uint8_t seriesIndex);
        void SetSpikesPerSecond(double spikesPerSecond, uint8_t seriesIndex);
        void SetSeriesColors(uint32_t index, double r, double g, double b);
        void AddPoint(double value, uint8_t seriesIndex, int32_t index = -1);
        void CalculateStandardDeviationValues();
        void Draw();
        void Load(char *fileName);
        void Save(char *fileName);
};

#endif
