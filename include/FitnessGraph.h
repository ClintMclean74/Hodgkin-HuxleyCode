#ifndef FITNESS_GRAPH_H
#define FITNESS_GRAPH_H

#include "HH_NeuralNetworks.h"
#include "Graph.h"
#include "Vector.h"
#include "Color.h"

#include "pstdint.h"

class SeriesInformation
{
    public:
        char seriesTitle[255];
        double spikesPerSecond;
        double totalSignalingDuration;
        double simulationTime;
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
        ////original static const uint32_t MAX_INDEX = 30;
        ////static const uint32_t MAX_SERIES = 31;
        static const uint32_t MAX_SERIES = HH_NeuralNetworks::NETWORKS_COLUMNS * HH_NeuralNetworks::NETWORKS_ROWS + 1;

        static const uint32_t colorsCount = 1020;
        Color graphColors[colorsCount];
        Color seriesColors[MAX_SERIES];

        double values[MAX_SERIES][MAX_INDEX];
        ////char* seriesTitles[MAX_SERIES];
        SeriesInformation seriesInformation[MAX_SERIES];

        char textBuffer[255];

    public:
        uint32_t currentIndexes[MAX_SERIES];
        uint32_t currentIndex = 0;
        uint32_t seriesCount = 0;

        double scale = 10;
        double xScale = 30;

        FitnessGraph(Vector pos);
        //FitnessGraph();

        void GenerateSignalStrengthColors();

        void SetSeriesTitle(char* seriesTitle, uint8_t seriesIndex);
        void SetSpikesPerSecond(double spikesPerSecond, uint8_t seriesIndex);
        void SetTotalSignalingDuration(double totalSignalingDuration, uint8_t seriesIndex);
        void SetSimulationTime(double simulationTime, uint8_t seriesIndex);
        void AddPoint(double value, uint8_t seriesIndex, int32_t index = -1);
        void Draw();
        void Load(char *fileName);
        void Save(char *fileName);
};

#endif
